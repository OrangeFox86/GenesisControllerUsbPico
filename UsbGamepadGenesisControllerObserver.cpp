#include "UsbGamepadGenesisControllerObserver.h"

UsbGamepadGenesisControllerObserver::UsbGamepadGenesisControllerObserver(
      UsbGamepad& usbController) :
    usbController(usbController)
{}

bool UsbGamepadGenesisControllerObserver::keyStateChanged(uint8_t id, Key key, bool down)
{
  // ID is unneeded
  (void)id;

  UsbGamepad::button_e button = UsbGamepad::BUTTON_COUNT;
  switch(key)
  {
    case KEY_START:
      button = UsbGamepad::BUTTON_START;
      break;
    case KEY_UP:
      button = UsbGamepad::BUTTON_UP;
      break;
    case KEY_DOWN:
      button = UsbGamepad::BUTTON_DOWN;
      break;
    case KEY_LEFT:
      button = UsbGamepad::BUTTON_LEFT;
      break;
    case KEY_RIGHT:
      button = UsbGamepad::BUTTON_RIGHT;
      break;
    case KEY_A:
      button = UsbGamepad::BUTTON_A;
      break;
    case KEY_B:
      button = UsbGamepad::BUTTON_B;
      break;
    case KEY_C:
      button = UsbGamepad::BUTTON_C;
      break;
    case KEY_X:
      button = UsbGamepad::BUTTON_X;
      break;
    case KEY_Y:
      button = UsbGamepad::BUTTON_Y;
      break;
    case KEY_Z:
      button = UsbGamepad::BUTTON_Z;
      break;
    case KEY_MODE:
      button = UsbGamepad::BUTTON_MODE;
      break;
  }

  if (button != UsbGamepad::BUTTON_COUNT)
  {
    if (down)
    {
      return usbController.updatePressed(button);
    }
    else
    {
      return usbController.updateReleased(button);
    }
  }
  else
  {
    return false;
  }
}

void UsbGamepadGenesisControllerObserver::releaseAllKeys()
{
  usbController.updateAllReleased();
}

void UsbGamepadGenesisControllerObserver::controllerConnected(uint8_t id)
{
  releaseAllKeys();
}

void UsbGamepadGenesisControllerObserver::controllerDisconnected(uint8_t id)
{
  releaseAllKeys();
}

bool UsbGamepadGenesisControllerObserver::sendKeys()
{
  return usbController.sendKeys();
}