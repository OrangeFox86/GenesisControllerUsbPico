#include "usb_execution.h"

#include "UsbControllerDevice.h"
#include "UsbKeyboard.h"
#include "UsbGamepad.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

// I'm having issues declaring these outside of static variables in functions, so here they are

#ifdef USE_GAMEPAD
UsbGamepad& getDevice(uint8_t controllerIndex)
{
  static UsbGamepad controllers[NUMBER_OF_DEVICES] =
    { UsbGamepad(REPORT_ID_DEVICE1), UsbGamepad(REPORT_ID_DEVICE2) };
  return controllers[controllerIndex % NUMBER_OF_DEVICES];
}
#endif
#ifdef USE_KEYBOARD
UsbKeyboard& getDevice(uint8_t controllerIndex)
{
  static UsbKeyboard controllers[NUMBER_OF_DEVICES] =
    { UsbKeyboard(REPORT_ID_DEVICE1), UsbKeyboard(REPORT_ID_DEVICE2) };
  return controllers[controllerIndex % NUMBER_OF_DEVICES];
}
#endif

bool gIsConnected = false;

const int16_t INVALID_INDEX = -1;

inline int16_t report_id_to_index( uint8_t report_id )
{
  if (report_id < REPORT_ID_DEVICE1 ||
      report_id >= NUMBER_OF_DEVICES + REPORT_ID_DEVICE1)
  {
    return INVALID_INDEX;
  }
  else
  {
    return report_id - REPORT_ID_DEVICE1;
  }
}

void led_task()
{
  static bool ledOn = false;
  bool keyPressed = false;
  for (uint32_t i = 0; i < NUMBER_OF_DEVICES; ++i)
  {
    if (getDevice(i).isButtonPressed())
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
    static const uint32_t BLINK_TIME_MS = 100;
    uint32_t t = board_millis() - startMs;
    if (t >= BLINK_TIME_MS)
    {
      startMs += BLINK_TIME_MS;
      ledOn = !ledOn;
    }
    ledOn = ledOn && keyPressed;
  }
  board_led_write(ledOn);
}

void usb_init()
{
    tusb_init();
}

void usb_task()
{
  tud_task(); // tinyusb device task
  led_task();
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  for (uint32_t i = 0; i < NUMBER_OF_DEVICES; ++i)
  {
    getDevice(i).updateConnected(true);
  }
  gIsConnected = true;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  for (uint32_t i = 0; i < NUMBER_OF_DEVICES; ++i)
  {
    getDevice(i).updateConnected(false);
  }
  gIsConnected = false;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  for (uint32_t i = 0; i < NUMBER_OF_DEVICES; ++i)
  {
    getDevice(i).updateConnected(false);
  }
  gIsConnected = false;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  for (uint32_t i = 0; i < NUMBER_OF_DEVICES; ++i)
  {
    getDevice(i).updateConnected(true);
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
    getDevice(idx).getReport(buffer, reqlen);
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