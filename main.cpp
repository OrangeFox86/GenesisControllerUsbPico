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

// Definitions for player 1
#ifdef PLAYER1_USB_KEYBOARD

UsbKeyboard player1UsbDevice(REPORT_ID_DEVICE1);
uint8_t player1KeyMap[IGenesisControllerObserver::KEY_COUNT] = PLAYER1_KEYBOARD_MAPPING;
// The observers send the keys pressed on the controllers to USB keyboards.
// Connecting each controller to its own keyboard allows each controller to independently send up to
// 6 keys.
UsbKeyboardGenesisControllerObserver player1Observer(player1UsbDevice, player1KeyMap);

#endif
#ifdef PLAYER1_USB_GAMEPAD

UsbGamepad player1UsbDevice(REPORT_ID_DEVICE1);
UsbGamepadGenesisControllerObserver::ButtonMap player1ButtonMap[IGenesisControllerObserver::KEY_COUNT] = GAMEPAD_MAPPING;
UsbGamepadGenesisControllerObserver player1Observer(player1UsbDevice, player1ButtonMap);

#endif


// Definitions for player 2
#ifdef PLAYER2_USB_KEYBOARD

UsbKeyboard player2UsbDevice(REPORT_ID_DEVICE2);
uint8_t player2KeyMap[IGenesisControllerObserver::KEY_COUNT] = PLAYER2_KEYBOARD_MAPPING;
UsbKeyboardGenesisControllerObserver player2Observer(player2UsbDevice, player2KeyMap);

#endif
#ifdef PLAYER2_USB_GAMEPAD

UsbGamepad player2UsbDevice(REPORT_ID_DEVICE2);
UsbGamepadGenesisControllerObserver::ButtonMap player2ButtonMap[IGenesisControllerObserver::KEY_COUNT] = GAMEPAD_MAPPING;
UsbGamepadGenesisControllerObserver player2Observer(player2UsbDevice, player2ButtonMap);

#endif

IUsbControllerDevice* pDevices[NUMBER_OF_DEVICES] = {&player1UsbDevice, &player2UsbDevice};

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
                    &player1Observer),
  GenesisController(1,
                    GENESIS_CONTROLLER_2_PIN_1,
                    GENESIS_CONTROLLER_2_PIN_2,
                    GENESIS_CONTROLLER_2_PIN_3,
                    GENESIS_CONTROLLER_2_PIN_4,
                    GENESIS_CONTROLLER_2_PIN_6,
                    GENESIS_CONTROLLER_2_PIN_7,
                    GENESIS_CONTROLLER_2_PIN_9,
                    &player2Observer)
};

/*------------- MAIN -------------*/
int main(void)
{
  board_init();

  set_usb_devices(pDevices, sizeof(pDevices) / sizeof(pDevices[0]));

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


