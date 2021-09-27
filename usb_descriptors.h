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
#if defined(USE_KEYBOARD)
  REPORT_ID_KEYBOARD1 = 1,
  REPORT_ID_KEYBOARD2
#elif defined(USE_GAMEPAD)
  REPORT_ID_CONTROLLER1 = 1,
  REPORT_ID_CONTROLLER2
#endif
};

#endif // __USB_DESCRITORS_H__
