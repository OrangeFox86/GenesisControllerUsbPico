#ifndef __USB_DESCRITORS_H__
#define __USB_DESCRITORS_H__

#include "configuration.h"
#include <stdbool.h>

#if (!defined(PLAYER2_USB_KEYBOARD) && !defined(PLAYER2_USB_GAMEPAD))
  #define NUMBER_OF_DEVICES 1
#else
  #define NUMBER_OF_DEVICES 2
#endif

enum {
    ITF_NUM_HID1 = 0,
#if (NUMBER_OF_DEVICES > 1)
    ITF_NUM_HID2,
#endif
    ITF_NUM_TOTAL
};

//! Enable or disable player 1 only descriptor
void set_usb_descriptor_player1_only(bool set);
//! @returns true iff the current descriptor is setup for player 1 only
bool is_usb_descriptor_player1_only();

#endif // __USB_DESCRITORS_H__
