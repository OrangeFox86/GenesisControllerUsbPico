#ifndef __USB_EXECUTION_H__
#define __USB_EXECUTION_H__

#include "IUsbControllerDevice.h"
#include "UsbKeyboard.h"
#include "UsbGamepad.h"
#include <stdint.h>
#include "configuration.h"

void set_usb_devices(IUsbControllerDevice** devices, uint8_t n);

void usb_init();
void usb_task();

#endif // __USB_EXECUTION_H__
