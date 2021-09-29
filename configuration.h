#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

// Define only one of the following
//#define USE_KEYBOARD
#define USE_GAMEPAD

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

#define KEYBOARD_MAPPING \
{ \
  /* Player 1 */ \
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
  }, \
  /* Player 2 */ \
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
  } \
}

#define GAMEPAD_MAPPING \
{ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON11,       /* START */ \
  UsbGamepadGenesisControllerObserver::MAP_DPAD_UP,        /* UP */ \
  UsbGamepadGenesisControllerObserver::MAP_DPAD_DOWN,      /* DOWN */ \
  UsbGamepadGenesisControllerObserver::MAP_DPAD_LEFT,      /* LEFT */ \
  UsbGamepadGenesisControllerObserver::MAP_DPAD_RIGHT,     /* RIGHT */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON0,        /* A */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON1,        /* B */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON2,        /* C */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON3,        /* X */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON4,        /* Y */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON5,        /* Z */ \
  UsbGamepadGenesisControllerObserver::MAP_BUTTON12        /* MODE */ \
}

#endif // __CONFIGURATION_H__
