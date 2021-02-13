#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "UsbKeyboard.h"
#include "GenesisController.h"
#include "UsbKeyboardGenesisControllerObserver.h"
#include "timers.h"

#define GPIO_PIN_9  9
#define GPIO_PIN_10 10
#define GPIO_PIN_11 11
#define GPIO_PIN_12 12
#define GPIO_PIN_13 13
#define GPIO_PIN_14 14
#define GPIO_PIN_15 15
#define GPIO_PIN_16 16
#define GPIO_PIN_17 17
#define GPIO_PIN_18 18
#define GPIO_PIN_19 19
#define GPIO_PIN_20 20
#define GPIO_PIN_21 21
#define GPIO_PIN_22 22

uint8_t keyLookup[2][IGenesisControllerObserver::KEY_COUNT] =
{
  // Player 1
  {
    HID_KEY_RETURN,       // START
    HID_KEY_W,            // UP
    HID_KEY_S,            // DOWN
    HID_KEY_A,            // LEFT
    HID_KEY_D,            // RIGHT
    HID_KEY_K,            // A
    HID_KEY_L,            // B
    HID_KEY_SEMICOLON,    // C
    HID_KEY_I,            // X
    HID_KEY_O,            // Y
    HID_KEY_P,            // Z
    HID_KEY_BACKSLASH     // MODE
  },
  // Player 2
  {
    HID_KEY_KEYPAD_ENTER, // START
    HID_KEY_ARROW_UP,     // UP
    HID_KEY_ARROW_DOWN,   // DOWN
    HID_KEY_ARROW_LEFT,   // LEFT
    HID_KEY_ARROW_RIGHT,  // RIGHT
    HID_KEY_KEYPAD_1,     // A
    HID_KEY_KEYPAD_2,     // B
    HID_KEY_KEYPAD_3,     // C
    HID_KEY_KEYPAD_4,     // X
    HID_KEY_KEYPAD_5,     // Y
    HID_KEY_KEYPAD_6,     // Z
    HID_KEY_KEYPAD_ADD    // MODE
  }
};

UsbKeyboardGenesisControllerObserver observers[2] =
{
  UsbKeyboardGenesisControllerObserver(gKeyboards[0], keyLookup[0]),
  UsbKeyboardGenesisControllerObserver(gKeyboards[0], keyLookup[1])
};

GenesisController gControllers[2] =
{
  GenesisController(0,
                    GPIO_PIN_9,
                    GPIO_PIN_10,
                    GPIO_PIN_11,
                    GPIO_PIN_12,
                    GPIO_PIN_13,
                    GPIO_PIN_14,
                    GPIO_PIN_15,
                    &observers[0]),
  GenesisController(1,
                    GPIO_PIN_22,
                    GPIO_PIN_21,
                    GPIO_PIN_20,
                    GPIO_PIN_19,
                    GPIO_PIN_18,
                    GPIO_PIN_17,
                    GPIO_PIN_16,
                    &observers[1])
};

/*------------- MAIN -------------*/
int main(void)
{
  board_init();

  UsbKeyboard::init();

  GenesisController* pController = gControllers;
  for (uint32_t i = sizeof(gControllers) / sizeof(gControllers[0]); i > 0; --i, ++pController)
  {
    pController->pinInit();
  }

  init_timer(gControllers, sizeof(gControllers) / sizeof(gControllers[0]));

  multicore_launch_core1(timer_process);

  while (1)
  {
    UsbKeyboard::task();
  }

  return 0;
}


