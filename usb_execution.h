#ifndef __USB_EXECUTION_H__
#define __USB_EXECUTION_H__

#include "UsbControllerDevice.h"
#include "UsbKeyboard.h"
#include "UsbGamepad.h"
#include <stdint.h>
#include "configuration.h"

#ifdef USE_GAMEPAD
UsbGamepad& getDevice(uint8_t controllerIndex);
#endif
#ifdef USE_KEYBOARD
UsbKeyboard& getDevice(uint8_t controllerIndex);
#endif

void usb_init();
void usb_task();

#endif // __USB_EXECUTION_H__
