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
  currentDpad(),
  currentButtons(0),
  buttonsUpdated(false),
  mIsConnected(false)
{}

bool UsbGamepad::isButtonPressed()
{
  return (currentDpad[DPAD_UP] || currentDpad[DPAD_DOWN] || currentDpad[DPAD_LEFT]
    || currentDpad[DPAD_RIGHT] || currentButtons != 0
    || currentLeftAnalog[0] != 0 || currentLeftAnalog[1] != 0 || currentLeftAnalog[2] != 0
    || currentRightAnalog[0] != 0 || currentRightAnalog[1] != 0 || currentRightAnalog[2] != 0);
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
void UsbGamepad::setAnalogThumbX(bool isLeft, int8_t x)
{
  int8_t lastX = 0;
  if (isLeft)
  {
    lastX = currentLeftAnalog[0];
    currentLeftAnalog[0] = x;
  }
  else
  {
    lastX = currentRightAnalog[0];
    currentRightAnalog[0] = x;
  }
  buttonsUpdated = buttonsUpdated || (x != lastX);
}

void UsbGamepad::setAnalogThumbY(bool isLeft, int8_t y)
{
  int8_t lastY = 0;
  if (isLeft)
  {
    lastY = currentLeftAnalog[1];
    currentLeftAnalog[1] = y;
  }
  else
  {
    lastY = currentRightAnalog[1];
    currentRightAnalog[1] = y;
  }
  buttonsUpdated = buttonsUpdated || (y != lastY);
}

void UsbGamepad::setAnalogTrigger(bool isLeft, int8_t z)
{
  int8_t lastZ = 0;
  if (isLeft)
  {
    lastZ = currentLeftAnalog[2];
    currentLeftAnalog[2] = z;
  }
  else
  {
    lastZ = currentRightAnalog[2];
    currentRightAnalog[2] = z;
  }
  buttonsUpdated = buttonsUpdated || (z != lastZ);
}

int8_t UsbGamepad::getAnalogThumbX(bool isLeft)
{
  if (isLeft)
  {
    return currentLeftAnalog[0];
  }
  else
  {
    return currentRightAnalog[0];
  }
}

int8_t UsbGamepad::getAnalogThumbY(bool isLeft)
{
  if (isLeft)
  {
    return currentLeftAnalog[1];
  }
  else
  {
    return currentRightAnalog[1];
  }
}

int8_t UsbGamepad::getAnalogTrigger(bool isLeft)
{
  if (isLeft)
  {
    return currentLeftAnalog[2];
  }
  else
  {
    return currentRightAnalog[2];
  }
}

void UsbGamepad::setDigitalPad(UsbGamepad::DpadButtons button, bool isPressed)
{
  bool oldValue = currentDpad[button];
  currentDpad[button] = isPressed;
  buttonsUpdated = buttonsUpdated || (oldValue != currentDpad[button]);
}

void UsbGamepad::setButtonMask(uint16_t mask, bool isPressed)
{
  uint16_t lastButtons = currentButtons;
  if (isPressed)
  {
    currentButtons |= mask;
  }
  else
  {
    currentButtons &= ~mask;
  }
  buttonsUpdated = buttonsUpdated || (lastButtons != currentButtons);
}

void UsbGamepad::setButton(uint8_t button, bool isPressed)
{
  setButtonMask(1 << button, isPressed);
}

void UsbGamepad::updateAllReleased()
{
  if (isButtonPressed())
  {
    currentLeftAnalog[0] = 0;
    currentLeftAnalog[1] = 0;
    currentLeftAnalog[2] = 0;
    currentRightAnalog[0] = 0;
    currentRightAnalog[1] = 0;
    currentRightAnalog[2] = 0;
    currentDpad[DPAD_UP] = false;
    currentDpad[DPAD_DOWN] = false;
    currentDpad[DPAD_LEFT] = false;
    currentDpad[DPAD_RIGHT] = false;
    currentButtons = 0;
    buttonsUpdated = true;
  }
}

uint8_t UsbGamepad::getHatValue()
{
  if (currentDpad[DPAD_UP])
  {
    if (currentDpad[DPAD_LEFT])
    {
      return GAMEPAD_HAT_UP_LEFT;
    }
    else if (currentDpad[DPAD_RIGHT])
    {
      return GAMEPAD_HAT_UP_RIGHT;
    }
    else
    {
      return GAMEPAD_HAT_UP;
    }
  }
  else if (currentDpad[DPAD_DOWN])
  {
    if (currentDpad[DPAD_LEFT])
    {
      return GAMEPAD_HAT_DOWN_LEFT;
    }
    else if (currentDpad[DPAD_RIGHT])
    {
      return GAMEPAD_HAT_DOWN_RIGHT;
    }
    else
    {
      return GAMEPAD_HAT_DOWN;
    }
  }
  else if (currentDpad[DPAD_LEFT])
  {
    return GAMEPAD_HAT_LEFT;
  }
  else if (currentDpad[DPAD_RIGHT])
  {
    return GAMEPAD_HAT_RIGHT;
  }
  else
  {
    return GAMEPAD_HAT_CENTERED;
  }
}

bool UsbGamepad::sendKeys(bool force)
{
  if (!mIsConnected) return false;
  uint8_t modifier = 0; // This class doesn't allow modifier keys
  if (buttonsUpdated || force)
  {
    bool sent = tud_hid_gamepad_report(reportId,
                                       currentLeftAnalog[0],
                                       currentLeftAnalog[1],
                                       currentLeftAnalog[2],
                                       currentRightAnalog[2],
                                       currentRightAnalog[0],
                                       currentRightAnalog[1],
                                       getHatValue(),
                                       currentButtons);
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
  report.x = currentLeftAnalog[0];
  report.y = currentLeftAnalog[1];
  report.z = currentLeftAnalog[2];
  report.rz = currentRightAnalog[0];
  report.rx = currentRightAnalog[1];
  report.ry = currentRightAnalog[2];
  report.hat = getHatValue();
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