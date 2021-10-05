#include "usb_execution.h"

#include "UsbControllerDevice.h"
#include "UsbKeyboard.h"
#include "UsbGamepad.h"
#include "configuration.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"
#include "device/dcd.h"
#include "usb_descriptors.h"

bool usbEnabled = false;

UsbControllerDevice** pAllUsbDevices = nullptr;

uint8_t numUsbDevices = 0;

bool usbDisconnecting = false;
absolute_time_t usbDisconnectTime;

void set_usb_devices(UsbControllerDevice** devices, uint8_t n)
{
  pAllUsbDevices = devices;
  numUsbDevices = n;
}

bool gIsConnected = false;

const int16_t INVALID_INDEX = -1;

void led_task()
{
  static bool ledOn = false;
  static uint32_t startMs = 0;
  if (usbDisconnecting)
  {
    // Currently counting down to disconnect; flash LED
    static const uint32_t BLINK_TIME_MS = 500;
    uint32_t t = board_millis() - startMs;
    if (t >= BLINK_TIME_MS)
    {
      startMs += BLINK_TIME_MS;
      ledOn = !ledOn;
    }
  }
  else
  {
    bool keyPressed = false;
    UsbControllerDevice** pdevs = pAllUsbDevices;
    for (uint32_t i = numUsbDevices; i > 0; --i, ++pdevs)
    {
      if ((*pdevs)->isButtonPressed())
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
      static const uint32_t BLINK_TIME_MS = 100;
      uint32_t t = board_millis() - startMs;
      if (t >= BLINK_TIME_MS)
      {
        startMs += BLINK_TIME_MS;
        ledOn = !ledOn;
      }
      ledOn = ledOn && keyPressed;
    }
  }
  board_led_write(ledOn);
}

void usb_init()
{
  tusb_init();
#if (!USB_ALWAYS_CONNECTED)
  // Initialize with USB in disconnected state, connect later once controller detected
  dcd_disconnect(0);
#endif
}

void start_disconnecting_countdown()
{
  update_us_since_boot(&usbDisconnectTime, time_us_64() + (1000000UL * DISCONNECT_DELAY_S));
  usbDisconnecting = true;
}

void usb_connection_state_machine()
{
  static bool lastPlayer1Connected = false;
  static bool lastPlayer2Connected = false;
  bool player1Connected = false;
  bool player2Connected = false;
  if (numUsbDevices > 0)
  {
    player1Connected = pAllUsbDevices[0]->isControllerConnected();
  }
  if (numUsbDevices > 1)
  {
    player2Connected = pAllUsbDevices[1]->isControllerConnected();
  }

  // Handle disconnecting state (wait complete is always true if disconnect delay is 0)
  bool disconnectWaitComplete = (DISCONNECT_DELAY_S <= 0.0);
  if (usbDisconnecting && !disconnectWaitComplete)
  {
    absolute_time_t currentTime;
    update_us_since_boot(&currentTime, time_us_64());
    if ((player1Connected && (player2Connected || is_usb_descriptor_player1_only()))
        || to_us_since_boot(currentTime) >= to_us_since_boot(usbDisconnectTime))
    {
      // Back to normal or time elapsed
      disconnectWaitComplete = true;
      usbDisconnecting = false;
    }
    else if ((player1Connected && !lastPlayer1Connected)
             || (player2Connected && !lastPlayer2Connected && !is_usb_descriptor_player1_only()))
    {
      // Player 1 or 2 reconnected; restart countdown
      start_disconnecting_countdown();
    }
  }

  // Handle normal state
  if (!usbDisconnecting || disconnectWaitComplete)
  {
    // Enable USB iff player 1 is connected
    if (player1Connected != usbEnabled)
    {
      if (player1Connected)
      {
        // Just connected player 1 controller - enable USB
        set_usb_descriptor_player1_only(!player2Connected);
        dcd_connect(0);
        usbEnabled = true;
      }
      else
      {
        if (!disconnectWaitComplete)
        {
          // Just disconnected - wait the necessary amount of time before acting
          start_disconnecting_countdown();
        }
        else
        {
          // Countdown elapsed
          dcd_disconnect(0);
          usbEnabled = false;
        }
      }
    }
    else
    {
      if (usbEnabled && is_usb_descriptor_player1_only() == player2Connected)
      {
        if (!player2Connected && !disconnectWaitComplete)
        {
          // Player 2 just disconnected - wait the necessary amount of time before acting
          start_disconnecting_countdown();
        }
        else
        {
          // Player 2 was added or was disconnected with countdown elapsed
          // Need to switch on or off player 2 over USB
          // Disconnecting then reconnecting is the only way I know how to refresh the USB config
          dcd_disconnect(0);
          sleep_ms(50);
          tud_task(); // tinyusb device task
          led_task();
          set_usb_descriptor_player1_only(!player2Connected);
          sleep_ms(100);
          dcd_connect(0);
        }
      }
    }
  }
  lastPlayer1Connected = player1Connected;
  lastPlayer2Connected = player2Connected;
}

void usb_task()
{
#if (!USB_ALWAYS_CONNECTED)
  usb_connection_state_machine();
#endif
  tud_task(); // tinyusb device task
  led_task();
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  UsbControllerDevice** pdevs = pAllUsbDevices;
  for (uint32_t i = numUsbDevices; i > 0; --i, ++pdevs)
  {
    if (!is_usb_descriptor_player1_only() || i == numUsbDevices)
    {
      (*pdevs)->updateUsbConnected(true);
    }
  }
  gIsConnected = true;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  UsbControllerDevice** pdevs = pAllUsbDevices;
  for (uint32_t i = numUsbDevices; i > 0; --i, ++pdevs)
  {
    (*pdevs)->updateUsbConnected(false);
  }
  gIsConnected = false;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  UsbControllerDevice** pdevs = pAllUsbDevices;
  for (uint32_t i = numUsbDevices; i > 0; --i, ++pdevs)
  {
    (*pdevs)->updateUsbConnected(false);
  }
  gIsConnected = false;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  UsbControllerDevice** pdevs = pAllUsbDevices;
  for (uint32_t i = numUsbDevices; i > 0; --i, ++pdevs)
  {
    if (!is_usb_descriptor_player1_only() || i == numUsbDevices)
    {
      (*pdevs)->updateUsbConnected(true);
    }
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
  (void) report_id;
  (void) report_type;
  if (instance < numUsbDevices)
  {
    return 0;
  }
  else
  {
    // Build the report for the given report ID
    pAllUsbDevices[instance]->getReport(buffer, reqlen);
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