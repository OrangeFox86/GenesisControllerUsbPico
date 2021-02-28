#include "UsbKeyboardGenesisControllerObserver.h"

UsbKeyboardGenesisControllerObserver::UsbKeyboardGenesisControllerObserver(
      UsbKeyboard& usbKeyboard,
      const uint8_t *keyLookup) :
    usbKeyboard(usbKeyboard),
    keyLookup(keyLookup)
{}

bool UsbKeyboardGenesisControllerObserver::keyStateChanged(uint8_t id, Key key, bool down)
{
  // ID is unneeded
  (void)id;

  uint8_t keycode = keyLookup[key];
  if (down)
  {
    return usbKeyboard.updatePressed(keycode);
  }
  else
  {
    return usbKeyboard.updateReleased(keycode);
  }
}

void UsbKeyboardGenesisControllerObserver::releaseAllKeys()
{
  usbKeyboard.updateAllReleased();
}

void UsbKeyboardGenesisControllerObserver::controllerConnected(uint8_t id)
{
  releaseAllKeys();
}

void UsbKeyboardGenesisControllerObserver::controllerDisconnected(uint8_t id)
{
  releaseAllKeys();
}

bool UsbKeyboardGenesisControllerObserver::sendKeys()
{
  return usbKeyboard.sendKeys();
}