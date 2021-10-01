#ifndef __USB_DESCRITORS_H__
#define __USB_DESCRITORS_H__

#include "configuration.h"

#if defined(USE_KEYBOARD) && defined(USE_GAMEPAD)
#error Must only define USE_KEYBOARD or USE_GAMEPAD, not both
#elif !defined(USE_KEYBOARD) && !defined(USE_GAMEPAD)
#error Must define one of USE_KEYBOARD or USE_GAMEPAD
#endif

//! Report IDs for all available USB devices set in usb_descriptors.c
enum usbReportId_e
{
  REPORT_ID_DEVICE1 = 1,
  REPORT_ID_DEVICE2
};

#define NUMBER_OF_DEVICES 2

#endif // __USB_DESCRITORS_H__
