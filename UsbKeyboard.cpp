#include "UsbKeyboard.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

UsbKeyboard gKeyboards[] = { UsbKeyboard(0) };

UsbKeyboard::UsbKeyboard(uint8_t reportId) :
  reportId(reportId),
  currentKeycodes(),
  keycodesUpdated(false),
  keyPressed(false)
{}

bool UsbKeyboard::isKeyPressed()
{
  return keyPressed;
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void UsbKeyboard::ledTask()
{
  bool keyPressed = false;
  bool isConnected = false;
  UsbKeyboard *pKeyboard = gKeyboards;
  for (uint32_t i = sizeof(gKeyboards) / sizeof(gKeyboards[0]); i > 0; --i, ++pKeyboard)
  {
    if (pKeyboard->isKeyPressed())
    {
      keyPressed = true;
    }
    if (pKeyboard->isConnected())
    {
      isConnected = true;
    }
  }
  board_led_write(isConnected && !keyPressed);
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
  uint8_t modifier = 0;
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
  uint8_t modifier = 0;
  memset(currentKeycodes, 0, sizeof(currentKeycodes));
}

bool UsbKeyboard::sendKeys(bool force)
{
  if (!mIsConnected) return false;
  uint8_t modifier = 0;
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
  report.modifier = 0;
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

  static uint32_t start_ms = 0;
  static bool key_down = false;
  static uint32_t key_count = 0;

#if 0
  // TEST CODE
  // Blink every interval ms
  uint32_t t = board_millis() - start_ms;
  if (key_down && t >= 100)
  {
    gKeyboards[0].updateReleased(HID_KEY_A);
    gKeyboards[0].sendKeys();
    key_down = false;
  }
  else if (t >= 1000)
  {
    start_ms += 1000;
    if (key_count < 5)
    {
      gKeyboards[0].updatePressed(HID_KEY_A);
      gKeyboards[0].sendKeys();
      key_down = true;
      key_count++;
    }
  }
#endif
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  UsbKeyboard *pKeyboard = gKeyboards;
  for (uint32_t i = sizeof(gKeyboards) / sizeof(gKeyboards[0]); i > 0; --i, ++pKeyboard)
  {
    pKeyboard->updateConnected(true);
  }
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  UsbKeyboard *pKeyboard = gKeyboards;
  for (uint32_t i = sizeof(gKeyboards) / sizeof(gKeyboards[0]); i > 0; --i, ++pKeyboard)
  {
    pKeyboard->updateConnected(false);
  }
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  UsbKeyboard *pKeyboard = gKeyboards;
  for (uint32_t i = sizeof(gKeyboards) / sizeof(gKeyboards[0]); i > 0; --i, ++pKeyboard)
  {
    pKeyboard->updateConnected(false);
  }
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  UsbKeyboard *pKeyboard = gKeyboards;
  for (uint32_t i = sizeof(gKeyboards) / sizeof(gKeyboards[0]); i > 0; --i, ++pKeyboard)
  {
    pKeyboard->updateConnected(true);
  }
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
  (void) report_type;

  // Build the report for the given report ID
  UsbKeyboard &keyboard = gKeyboards[report_id];
  keyboard.getReport(buffer, reqlen);
  // Return the size of the report
  return sizeof(hid_keyboard_report_t);
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const *buffer,
                           uint16_t bufsize)
{
    // Only 1 report possible here
    (void) report_id;
    (void) report_type;

    // echo back anything we received from host
    tud_hid_report(0, buffer, bufsize);
}