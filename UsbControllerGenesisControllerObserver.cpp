#include "UsbControllerGenesisControllerObserver.h"

UsbControllerGenesisControllerObserver::UsbControllerGenesisControllerObserver(
      UsbController& usbController) :
    usbController(usbController)
{}

bool UsbControllerGenesisControllerObserver::keyStateChanged(uint8_t id, Key key, bool down)
{
  // ID is unneeded
  (void)id;

  UsbController::button_e button = UsbController::BUTTON_COUNT;
  switch(key)
  {
    case KEY_START:
      button = UsbController::BUTTON_START;
      break;
    case KEY_UP:
      button = UsbController::BUTTON_UP;
      break;
    case KEY_DOWN:
      button = UsbController::BUTTON_DOWN;
      break;
    case KEY_LEFT:
      button = UsbController::BUTTON_LEFT;
      break;
    case KEY_RIGHT:
      button = UsbController::BUTTON_RIGHT;
      break;
    case KEY_A:
      button = UsbController::BUTTON_A;
      break;
    case KEY_B:
      button = UsbController::BUTTON_B;
      break;
    case KEY_C:
      button = UsbController::BUTTON_C;
      break;
    case KEY_X:
      button = UsbController::BUTTON_X;
      break;
    case KEY_Y:
      button = UsbController::BUTTON_Y;
      break;
    case KEY_Z:
      button = UsbController::BUTTON_Z;
      break;
    case KEY_MODE:
      button = UsbController::BUTTON_MODE;
      break;
  }

  if (button != UsbController::BUTTON_COUNT)
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

void UsbControllerGenesisControllerObserver::releaseAllKeys()
{
  usbController.updateAllReleased();
}

void UsbControllerGenesisControllerObserver::controllerConnected(uint8_t id)
{
  releaseAllKeys();
}

void UsbControllerGenesisControllerObserver::controllerDisconnected(uint8_t id)
{
  releaseAllKeys();
}

bool UsbControllerGenesisControllerObserver::sendKeys()
{
  return usbController.sendKeys();
}