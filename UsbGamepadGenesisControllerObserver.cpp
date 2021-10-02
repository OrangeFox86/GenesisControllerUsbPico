#include "UsbGamepadGenesisControllerObserver.h"

UsbGamepadGenesisControllerObserver::UsbGamepadGenesisControllerObserver(
      UsbGamepad& usbController,
      const ButtonMap *buttonLookup) :
    usbController(usbController),
    buttonLookup(buttonLookup)
{}

bool UsbGamepadGenesisControllerObserver::keyStateChanged(uint8_t id, Key key, bool down)
{
  // ID is unneeded
  (void)id;

  ButtonMap button = buttonLookup[key];

  switch (button)
  {
    case MAP_LANALOG_UP:
      if (usbController.getAnalogThumbY(true) >= 0)
      {
        usbController.setAnalogThumbY(true, down ? 127 : 0);
      }
      break;
    case MAP_LANALOG_DOWN:
      if (usbController.getAnalogThumbY(true) <= 0)
      {
        usbController.setAnalogThumbY(true, down ? -128 : 0);
      }
      break;
    case MAP_LANALOG_LEFT:
      if (usbController.getAnalogThumbX(true) <= 0)
      {
        usbController.setAnalogThumbX(true, down ? -128 : 0);
      }
      break;
    case MAP_LANALOG_RIGHT:
      if (usbController.getAnalogThumbX(true) >= 0)
      {
        usbController.setAnalogThumbX(true, down ? 127 : 0);
      }
      break;
    case MAP_RANALOG_UP:
      if (usbController.getAnalogThumbY(false) >= 0)
      {
        usbController.setAnalogThumbY(false, down ? 127 : 0);
      }
      break;
    case MAP_RANALOG_DOWN:
      if (usbController.getAnalogThumbY(false) <= 0)
      {
        usbController.setAnalogThumbY(false, down ? -128 : 0);
      }
      break;
    case MAP_RANALOG_LEFT:
      if (usbController.getAnalogThumbX(false) <= 0)
      {
        usbController.setAnalogThumbX(false, down ? -128 : 0);
      }
      break;
    case MAP_RANALOG_RIGHT:
      if (usbController.getAnalogThumbX(false) >= 0)
      {
        usbController.setAnalogThumbX(false, down ? 127 : 0);
      }
      break;
    case MAP_LTRIGGER_UP:
      if (usbController.getAnalogTrigger(true) >= 0)
      {
        usbController.setAnalogTrigger(true, down ? 127 : 0);
      }
      break;
    case MAP_LTRIGGER_DOWN:
      if (usbController.getAnalogTrigger(true) <= 0)
      {
        usbController.setAnalogTrigger(true, down ? -128 : 0);
      }
      break;
    case MAP_RTRIGGER_UP:
      if (usbController.getAnalogTrigger(false) >= 0)
      {
        usbController.setAnalogTrigger(false, down ? 127 : 0);
      }
      break;
    case MAP_RTRIGGER_DOWN:
      if (usbController.getAnalogTrigger(false) <= 0)
      {
        usbController.setAnalogTrigger(false, down ? -128 : 0);
      }
      break;
    case MAP_DPAD_UP:
      usbController.setDigitalPad(UsbGamepad::DPAD_UP, down);
      break;
    case MAP_DPAD_DOWN:
      usbController.setDigitalPad(UsbGamepad::DPAD_DOWN, down);
      break;
    case MAP_DPAD_LEFT:
      usbController.setDigitalPad(UsbGamepad::DPAD_LEFT, down);
      break;
    case MAP_DPAD_RIGHT:
      usbController.setDigitalPad(UsbGamepad::DPAD_RIGHT, down);
      break;
    case MAP_BUTTON0:
    case MAP_BUTTON1:
    case MAP_BUTTON2:
    case MAP_BUTTON3:
    case MAP_BUTTON4:
    case MAP_BUTTON5:
    case MAP_BUTTON6:
    case MAP_BUTTON7:
    case MAP_BUTTON8:
    case MAP_BUTTON9:
    case MAP_BUTTON10:
    case MAP_BUTTON11:
    case MAP_BUTTON12:
    case MAP_BUTTON13:
    case MAP_BUTTON14:
    case MAP_BUTTON15:
      usbController.setButton(button - MAP_BUTTON0, down);
      break;
    case MAP_NONE:
      // Do nothing and return true
      break;
    default:
      return false;
  }
  return true;
}

void UsbGamepadGenesisControllerObserver::controllerConnected(uint8_t id)
{
  usbController.updateControllerConnected(true);
}

void UsbGamepadGenesisControllerObserver::controllerDisconnected(uint8_t id)
{
  usbController.updateControllerConnected(false);
}

bool UsbGamepadGenesisControllerObserver::send()
{
  return usbController.send();
}