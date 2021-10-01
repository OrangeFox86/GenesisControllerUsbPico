#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "UsbKeyboard.h"
#include "UsbGamepad.h"
#include "GenesisController.h"
#include "UsbKeyboardGenesisControllerObserver.h"
#include "UsbGamepadGenesisControllerObserver.h"
#include "timers.h"
#include "usb_descriptors.h"
#include "usb_execution.h"
#include "configuration.h"

#ifdef USE_KEYBOARD

uint8_t keyLookup[NUMBER_OF_DEVICES][IGenesisControllerObserver::KEY_COUNT] = KEYBOARD_MAPPING;

// The observers send the keys pressed on the controllers to USB keyboards.
// Connecting each controller to its own keyboard allows each controller to independently send up to
// 6 keys.
UsbKeyboardGenesisControllerObserver observers[NUMBER_OF_DEVICES] =
{
  UsbKeyboardGenesisControllerObserver(getDevice(0), keyLookup[0]),
  UsbKeyboardGenesisControllerObserver(getDevice(1), keyLookup[1])
};

#endif // USE_KEYBOARD

#ifdef USE_GAMEPAD

UsbGamepadGenesisControllerObserver::ButtonMap buttonLookup[IGenesisControllerObserver::KEY_COUNT] = GAMEPAD_MAPPING;

// The observers send the keys pressed on the controllers to USB controller.
UsbGamepadGenesisControllerObserver observers[NUMBER_OF_DEVICES] =
{
  UsbGamepadGenesisControllerObserver(getDevice(0), buttonLookup),
  UsbGamepadGenesisControllerObserver(getDevice(1), buttonLookup)
};

#endif // USE_GAMEPAD

GenesisController gControllers[NUMBER_OF_DEVICES] =
{
  GenesisController(0,
                    GENESIS_CONTROLLER_1_PIN_1,
                    GENESIS_CONTROLLER_1_PIN_2,
                    GENESIS_CONTROLLER_1_PIN_3,
                    GENESIS_CONTROLLER_1_PIN_4,
                    GENESIS_CONTROLLER_1_PIN_6,
                    GENESIS_CONTROLLER_1_PIN_7,
                    GENESIS_CONTROLLER_1_PIN_9,
                    &observers[0]),
  GenesisController(1,
                    GENESIS_CONTROLLER_2_PIN_1,
                    GENESIS_CONTROLLER_2_PIN_2,
                    GENESIS_CONTROLLER_2_PIN_3,
                    GENESIS_CONTROLLER_2_PIN_4,
                    GENESIS_CONTROLLER_2_PIN_6,
                    GENESIS_CONTROLLER_2_PIN_7,
                    GENESIS_CONTROLLER_2_PIN_9,
                    &observers[1])
};

/*------------- MAIN -------------*/
int main(void)
{
  board_init();

  usb_init();

  GenesisController* pController = gControllers;
  for (uint32_t i = sizeof(gControllers) / sizeof(gControllers[0]); i > 0; --i, ++pController)
  {
    pController->pinInit();
  }

  init_timer(gControllers, sizeof(gControllers) / sizeof(gControllers[0]));

  multicore_launch_core1(timer_process);

  while (1)
  {
    usb_task();
  }

  return 0;
}


