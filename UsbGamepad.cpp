#include "UsbGamepad.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

// Only use this code if Controller descriptors are defined
#ifdef USE_GAMEPAD

static bool gIsConnected = false;

static const int16_t INVALID_INDEX = -1;

inline int16_t report_id_to_index( uint8_t report_id )
{
  if (report_id < REPORT_ID_CONTROLLER1 ||
      report_id >= UsbGamepad::NUMBER_OF_CONTROLLERS + REPORT_ID_CONTROLLER1)
  {
    return INVALID_INDEX;
  }
  else
  {
    return report_id - REPORT_ID_CONTROLLER1;
  }
}

UsbGamepad::UsbGamepad(uint8_t reportId) :
  reportId(reportId),
  currentDpad(0),
  currentButtons(0),
  buttonsUpdated(false),
  mIsConnected(false)
{}

bool UsbGamepad::isButtonPressed()
{
  return (currentDpad != 0 || currentButtons != 0);
}

UsbGamepad& UsbGamepad::getGamepad(uint8_t controllerIndex)
{
  static UsbGamepad controllers[NUMBER_OF_CONTROLLERS] =
    { UsbGamepad(REPORT_ID_CONTROLLER1), UsbGamepad(REPORT_ID_CONTROLLER2) };
  return controllers[controllerIndex % NUMBER_OF_CONTROLLERS];
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void UsbGamepad::ledTask()
{
  static bool ledOn = false;
  bool somethingPressed = false;
  for (uint32_t i = 0; i < UsbGamepad::NUMBER_OF_CONTROLLERS; ++i)
  {
    if (UsbGamepad::getGamepad(i).isButtonPressed())
    {
      somethingPressed = true;
    }
  }
  if (gIsConnected)
  {
    // When connected, LED is ON only when no key is pressed
    ledOn = !somethingPressed;
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
    ledOn = ledOn && somethingPressed;
  }
  board_led_write(ledOn);
}

//--------------------------------------------------------------------+
// EXTERNAL API
//--------------------------------------------------------------------+
bool UsbGamepad::updatePressed(button_e button)
{
  uint8_t lastDpad = currentDpad;
  uint32_t lastButtons = currentButtons;
  bool invalid = false;
  switch(button)
  {
    case BUTTON_UP:
      currentDpad |= GAMEPAD_HAT_UP;
      break;
    case BUTTON_DOWN:
      currentDpad |= GAMEPAD_HAT_DOWN;
      break;
    case BUTTON_LEFT:
      currentDpad |= GAMEPAD_HAT_LEFT;
      break;
    case BUTTON_RIGHT:
      currentDpad |= GAMEPAD_HAT_RIGHT;
      break;

    case BUTTON_A:
      currentButtons |= GAMEPAD_BUTTON_A;
      break;
    case BUTTON_B:
      currentButtons |= GAMEPAD_BUTTON_B;
      break;
    case BUTTON_C:
      currentButtons |= GAMEPAD_BUTTON_C;
      break;
    case BUTTON_X:
      currentButtons |= GAMEPAD_BUTTON_X;
      break;
    case BUTTON_Y:
      currentButtons |= GAMEPAD_BUTTON_Y;
      break;
    case BUTTON_Z:
      currentButtons |= GAMEPAD_BUTTON_Z;
      break;
    case BUTTON_START:
      currentButtons |= GAMEPAD_BUTTON_START;
      break;
    case BUTTON_MODE:
      currentButtons |= GAMEPAD_BUTTON_MODE;
      break;

    default:
      invalid = true;
      break;
  }
  if (!invalid)
  {
    buttonsUpdated = (lastDpad != currentDpad || lastButtons != currentButtons);
  }
  return !invalid;
}

bool UsbGamepad::updateReleased(button_e button)
{
  uint8_t lastDpad = currentDpad;
  uint32_t lastButtons = currentButtons;
  bool invalid = false;
  switch(button)
  {
    case BUTTON_UP:
      currentDpad &= ~GAMEPAD_HAT_UP;
      break;
    case BUTTON_DOWN:
      currentDpad &= ~GAMEPAD_HAT_DOWN;
      break;
    case BUTTON_LEFT:
      currentDpad &= ~GAMEPAD_HAT_LEFT;
      break;
    case BUTTON_RIGHT:
      currentDpad &= ~GAMEPAD_HAT_RIGHT;
      break;

    case BUTTON_A:
      currentButtons &= ~GAMEPAD_BUTTON_A;
      break;
    case BUTTON_B:
      currentButtons &= ~GAMEPAD_BUTTON_B;
      break;
    case BUTTON_C:
      currentButtons &= ~GAMEPAD_BUTTON_C;
      break;
    case BUTTON_X:
      currentButtons &= ~GAMEPAD_BUTTON_X;
      break;
    case BUTTON_Y:
      currentButtons &= ~GAMEPAD_BUTTON_Y;
      break;
    case BUTTON_Z:
      currentButtons &= ~GAMEPAD_BUTTON_Z;
      break;
    case BUTTON_START:
      currentButtons &= ~GAMEPAD_BUTTON_START;
      break;
    case BUTTON_MODE:
      currentButtons &= ~GAMEPAD_BUTTON_MODE;
      break;

    default:
      invalid = true;
      break;
  }
  if (!invalid)
  {
    buttonsUpdated = (lastDpad != currentDpad || lastButtons != currentButtons);
  }
  return !invalid;
}

void UsbGamepad::updateAllReleased()
{
  if (isButtonPressed())
  {
    currentDpad = 0;
    currentButtons = 0;
    buttonsUpdated = true;
  }
}

bool UsbGamepad::sendKeys(bool force)
{
  if (!mIsConnected) return false;
  uint8_t modifier = 0; // This class doesn't allow modifier keys
  if (buttonsUpdated || force)
  {
    bool sent = tud_hid_gamepad_report(reportId, 0, 0, 0, 0, 0, 0, currentDpad, currentButtons);
    if (sent)
    {
      buttonsUpdated = false;
    }
    return sent;
  }
  else
  {
    return true;
  }
}

void UsbGamepad::getReport(uint8_t *buffer, uint16_t reqlen)
{
  // Build the report
  hid_gamepad_report_t report;
  report.x = 0;
  report.y = 0;
  report.z = 0;
  report.rz = 0;
  report.rx = 0;
  report.ry = 0;
  report.hat = currentDpad;
  report.buttons = currentButtons;
  // Copy report into buffer
  uint16_t setLen = (sizeof(report) <= reqlen) ? sizeof(report) : reqlen;
  memcpy(buffer, &report, setLen);
}

void UsbGamepad::updateConnected(bool connected)
{
  mIsConnected = connected;
}

bool UsbGamepad::isConnected()
{
  return mIsConnected;
}

void UsbGamepad::init()
{
    tusb_init();
}

void UsbGamepad::task()
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
  for (uint32_t i = 0; i < UsbGamepad::NUMBER_OF_CONTROLLERS; ++i)
  {
    UsbGamepad::getGamepad(i).updateConnected(true);
  }
  gIsConnected = true;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  for (uint32_t i = 0; i < UsbGamepad::NUMBER_OF_CONTROLLERS; ++i)
  {
    UsbGamepad::getGamepad(i).updateConnected(false);
  }
  gIsConnected = false;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  for (uint32_t i = 0; i < UsbGamepad::NUMBER_OF_CONTROLLERS; ++i)
  {
    UsbGamepad::getGamepad(i).updateConnected(false);
  }
  gIsConnected = false;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  for (uint32_t i = 0; i < UsbGamepad::NUMBER_OF_CONTROLLERS; ++i)
  {
    UsbGamepad::getGamepad(i).updateConnected(true);
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
    UsbGamepad &controller = UsbGamepad::getGamepad(idx);
    controller.getReport(buffer, reqlen);
    // Return the size of the report
    return sizeof(hid_gamepad_report_t);
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

#endif // USE_GAMEPAD