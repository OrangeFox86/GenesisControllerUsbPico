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

void UsbKeyboardGenesisControllerObserver::controllerConnected(uint8_t id)
{
  usbKeyboard.updateControllerConnected(true);
}

void UsbKeyboardGenesisControllerObserver::controllerDisconnected(uint8_t id)
{
  usbKeyboard.updateControllerConnected(false);
}

bool UsbKeyboardGenesisControllerObserver::send()
{
  return usbKeyboard.send();
}