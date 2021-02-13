#include "UsbKeyboardGenesisControllerObserver.h"

UsbKeyboardGenesisControllerObserver::UsbKeyboardGenesisControllerObserver(
      UsbKeyboard& usbKeyboard,
      const uint8_t *keyLookup) :
    usbKeyboard(usbKeyboard),
    keyLookup(keyLookup)
{}

bool UsbKeyboardGenesisControllerObserver::keyStateChanged(uint8_t id, Key key, bool down)
{
  // Key is unneeded
  (void)key;

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
  const uint8_t *keys = keyLookup;
  for (uint32_t i = 0; i < IGenesisControllerObserver::KEY_COUNT; ++i, ++keys)
  {
    usbKeyboard.updateReleased(*keys);
  }
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