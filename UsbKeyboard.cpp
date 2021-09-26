#include "UsbKeyboard.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

// Only use this code if keyboard descriptors are defined
#ifdef USE_KEYBOARD

static bool gIsConnected = false;

static const int16_t INVALID_INDEX = -1;

inline int16_t report_id_to_index( uint8_t report_id )
{
  if (report_id < REPORT_ID_KEYBOARD1 ||
      report_id >= UsbKeyboard::NUMBER_OF_KEYBOARDS + REPORT_ID_KEYBOARD1)
  {
    return INVALID_INDEX;
  }
  else
  {
    return report_id - REPORT_ID_KEYBOARD1;
  }
}

UsbKeyboard::UsbKeyboard(uint8_t reportId) :
  reportId(reportId),
  currentKeycodes(),
  keycodesUpdated(false),
  keyPressed(false),
  mIsConnected(false)
{}

bool UsbKeyboard::isKeyPressed()
{
  return keyPressed;
}

UsbKeyboard& UsbKeyboard::getKeyboard(uint8_t keyboardIndex)
{
  static UsbKeyboard keyboards[NUMBER_OF_KEYBOARDS] =
    { UsbKeyboard(REPORT_ID_KEYBOARD1), UsbKeyboard(REPORT_ID_KEYBOARD2) };
  return keyboards[keyboardIndex % NUMBER_OF_KEYBOARDS];
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void UsbKeyboard::ledTask()
{
  static bool ledOn = false;
  bool keyPressed = false;
  for (uint32_t i = 0; i < UsbKeyboard::NUMBER_OF_KEYBOARDS; ++i)
  {
    if (UsbKeyboard::getKeyboard(i).isKeyPressed())
    {
      keyPressed = true;
    }
  }
  if (gIsConnected)
  {
    // When connected, LED is ON only when no key is pressed
    ledOn = !keyPressed;
  }
  else
  {
    // When not connected, LED blinks when key is pressed
    static uint32_t startMs = 0;
    static const uint32_t KEY_PRESS_BLINK_TIME_MS = 100;
    uint32_t t = board_millis() - startMs;
    if (t >= KEY_PRESS_BLINK_TIME_MS)
    {
      startMs += KEY_PRESS_BLINK_TIME_MS;
      ledOn = !ledOn;
    }
    ledOn = ledOn && keyPressed;
  }
  board_led_write(ledOn);
}

//--------------------------------------------------------------------+
// EXTERNAL API
//--------------------------------------------------------------------+
bool UsbKeyboard::updatePressed(uint8_t keycode)
{
  bool foundDuplicate = false;
  int32_t slot = -1;
  uint8_t modifier = 0; // This class doesn't allow modifier keys
  for (uint32_t i = 0; i < sizeof(currentKeycodes) / sizeof(currentKeycodes[0]); ++i)
  {
    if (slot == -1 && currentKeycodes[i] == 0)
    {
      slot = i;
    }
    else if (currentKeycodes[i] == keycode)
    {
      foundDuplicate = true;
    }
  }
  if (slot != -1 && !foundDuplicate)
  {
    keyPressed = true;
    currentKeycodes[slot] = keycode;
    keycodesUpdated = true;
  }
  return (slot != -1 || foundDuplicate);
}

bool UsbKeyboard::updateReleased(uint8_t keycode)
{
  bool found = false;
  uint8_t modifier = 0; // This class doesn't allow modifier keys
  for (uint32_t i = 0; i < sizeof(currentKeycodes) / sizeof(currentKeycodes[0]); ++i)
  {
    if (found)
    {
      // Shift everything 1 to the left once key is found
      currentKeycodes[i - 1] = currentKeycodes[i];
      currentKeycodes[i] = 0;
    }
    else if (currentKeycodes[i] == keycode)
    {
      // Remove this key
      found = true;
      currentKeycodes[i] = 0;
      keycodesUpdated = true;
      // If this is the first position and the next position is empty, assume no more keys pressed
      if (i == 0 && currentKeycodes[1] == 0)
      {
        keyPressed = false;
      }
    }
  }
  return found;
}

void UsbKeyboard::updateAllReleased()
{
  uint8_t modifier = 0; // This class doesn't allow modifier keys
  memset(currentKeycodes, 0, sizeof(currentKeycodes));
  keyPressed = false;
  keycodesUpdated = true;
}

bool UsbKeyboard::sendKeys(bool force)
{
  if (!mIsConnected) return false;
  uint8_t modifier = 0; // This class doesn't allow modifier keys
  if (keycodesUpdated || force)
  {
    bool sent = tud_hid_keyboard_report(reportId, modifier, currentKeycodes);
    if (sent)
    {
      keycodesUpdated = false;
    }
    return sent;
  }
  else
  {
    return true;
  }
}

void UsbKeyboard::getReport(uint8_t *buffer, uint16_t reqlen)
{
  // Build the report
  hid_keyboard_report_t report;
  report.modifier = 0; // This class doesn't allow modifier keys
  report.reserved = 0;
  memcpy(report.keycode, currentKeycodes, sizeof(report.keycode));
  // Copy report into buffer
  uint16_t setLen = (sizeof(report) <= reqlen) ? sizeof(report) : reqlen;
  memcpy(buffer, &report, setLen);
}

void UsbKeyboard::updateConnected(bool connected)
{
  mIsConnected = connected;
}

bool UsbKeyboard::isConnected()
{
  return mIsConnected;
}

void UsbKeyboard::init()
{
    tusb_init();
}

void UsbKeyboard::task()
{
  tud_task(); // tinyusb device task
  ledTask();
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  for (uint32_t i = 0; i < UsbKeyboard::NUMBER_OF_KEYBOARDS; ++i)
  {
    UsbKeyboard::getKeyboard(i).updateConnected(true);
  }
  gIsConnected = true;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  for (uint32_t i = 0; i < UsbKeyboard::NUMBER_OF_KEYBOARDS; ++i)
  {
    UsbKeyboard::getKeyboard(i).updateConnected(false);
  }
  gIsConnected = false;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  for (uint32_t i = 0; i < UsbKeyboard::NUMBER_OF_KEYBOARDS; ++i)
  {
    UsbKeyboard::getKeyboard(i).updateConnected(false);
  }
  gIsConnected = false;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  for (uint32_t i = 0; i < UsbKeyboard::NUMBER_OF_KEYBOARDS; ++i)
  {
    UsbKeyboard::getKeyboard(i).updateConnected(true);
  }
  gIsConnected = true;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
  (void) instance;
  (void) report_type;
  int16_t idx = report_id_to_index( report_id );
  if (idx == INVALID_INDEX)
  {
    return 0;
  }
  else
  {
    // Build the report for the given report ID
    UsbKeyboard &keyboard = UsbKeyboard::getKeyboard(idx);
    keyboard.getReport(buffer, reqlen);
    // Return the size of the report
    return sizeof(hid_keyboard_report_t);
  }
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance,
                           uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const *buffer,
                           uint16_t bufsize)
{
  (void) instance;
  (void) report_type;

  // echo back anything we received from host
  tud_hid_report(report_id, buffer, bufsize);
}

#endif // USE_KEYBOARD