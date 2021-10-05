#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

// Define only one of the following
//#define PLAYER1_USB_KEYBOARD
#define PLAYER1_USB_GAMEPAD

// Define only one or neither of the following
//#define PLAYER2_USB_KEYBOARD
#define PLAYER2_USB_GAMEPAD

// When true: Forces USB to always be connected
// When false: USB will only connect when player 1 controller is detected.
#define USB_ALWAYS_CONNECTED false
// Delay in seconds between no controllers detected and when USB is disconnected.
#define DISCONNECT_DELAY_S 10.0

// Genesis controller 1 Pico pin definitions
#define GENESIS_CONTROLLER_1_PIN_1 9
#define GENESIS_CONTROLLER_1_PIN_2 10
#define GENESIS_CONTROLLER_1_PIN_3 11
#define GENESIS_CONTROLLER_1_PIN_4 12
#define GENESIS_CONTROLLER_1_PIN_6 13
#define GENESIS_CONTROLLER_1_PIN_7 14
#define GENESIS_CONTROLLER_1_PIN_9 15

// Genesis controller 2 Pico pin definitions
#define GENESIS_CONTROLLER_2_PIN_1 22
#define GENESIS_CONTROLLER_2_PIN_2 21
#define GENESIS_CONTROLLER_2_PIN_3 20
#define GENESIS_CONTROLLER_2_PIN_4 19
#define GENESIS_CONTROLLER_2_PIN_6 18
#define GENESIS_CONTROLLER_2_PIN_7 17
#define GENESIS_CONTROLLER_2_PIN_9 16

#define PLAYER1_KEYBOARD_MAPPING \
{ \
  HID_KEY_RETURN,       /* START */ \
  HID_KEY_W,            /* UP */ \
  HID_KEY_S,            /* DOWN */ \
  HID_KEY_A,            /* LEFT */ \
  HID_KEY_D,            /* RIGHT */ \
  HID_KEY_K,            /* A */ \
  HID_KEY_L,            /* B */ \
  HID_KEY_SEMICOLON,    /* C */ \
  HID_KEY_I,            /* X */ \
  HID_KEY_O,            /* Y */ \
  HID_KEY_P,            /* Z */ \
  HID_KEY_BACKSLASH     /* MODE */ \
}

#define PLAYER2_KEYBOARD_MAPPING \
{ \
  HID_KEY_KEYPAD_ENTER, /* START */ \
  HID_KEY_ARROW_UP,     /* UP */ \
  HID_KEY_ARROW_DOWN,   /* DOWN */ \
  HID_KEY_ARROW_LEFT,   /* LEFT */ \
  HID_KEY_ARROW_RIGHT,  /* RIGHT */ \
  HID_KEY_KEYPAD_1,     /* A */ \
  HID_KEY_KEYPAD_2,     /* B */ \
  HID_KEY_KEYPAD_3,     /* C */ \
  HID_KEY_KEYPAD_4,     /* X */ \
  HID_KEY_KEYPAD_5,     /* Y */ \
  HID_KEY_KEYPAD_6,     /* Z */ \
  HID_KEY_KEYPAD_ADD    /* MODE */ \
}

#if 1
// Configuration specifically for Retropie - If all else fails, this will work with sdl2 driver in
// "fallback configuration" if it can't find a configuration for some reason. If you want XYZ and
// mode buttons to work, make sure you have the setting enabled to use 6 button controller in
// genesis settings (which is usually set to 3 button by default).
#define GAMEPAD_MAPPING \
{ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON6,      /* START */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON11,     /* UP */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON12,     /* DOWN */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON13,     /* LEFT */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON14,     /* RIGHT */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON2,      /* A */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON0,      /* B */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON1,      /* C */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON9,      /* X */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON3,      /* Y */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON10,     /* Z */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON4       /* MODE */ \
}
#else
// This is the configuration as recommended by the Tiny USB library
#define GAMEPAD_MAPPING \
{ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON11,     /* START */ \
  UsbGamepadGenesisControllerObserver::MAP_DPAD_UP,      /* UP */ \
  UsbGamepadGenesisControllerObserver::MAP_DPAD_DOWN,    /* DOWN */ \
  UsbGamepadGenesisControllerObserver::MAP_DPAD_LEFT,    /* LEFT */ \
  UsbGamepadGenesisControllerObserver::MAP_DPAD_RIGHT,   /* RIGHT */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON0,      /* A */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON1,      /* B */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON2,      /* C */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON3,      /* X */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON4,      /* Y */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON5,      /* Z */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON12      /* MODE */ \
}
#endif

#endif // __CONFIGURATION_H__
