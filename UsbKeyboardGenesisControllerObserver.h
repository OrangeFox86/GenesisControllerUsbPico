#ifndef __USB_KEYBOARD_GENESIS_CONTROLLER_OBSERVER_H__
#define __USB_KEYBOARD_GENESIS_CONTROLLER_OBSERVER_H__

#include "IGenesisControllerObserver.h"
#include "UsbKeyboard.h"

//! Yes, I know this name is ridiculous, but at least it's descriptive!
class UsbKeyboardGenesisControllerObserver : public IGenesisControllerObserver
{
  public:
    //! Constructor for UsbKeyboardGenesisControllerObserver
    //! @param[in] usbKeyboard  The USB keyboard to update when keys are pressed or released
    //! @param[in] keyLookup  The mapping of genesis keys to USB keyboard keys
    UsbKeyboardGenesisControllerObserver(UsbKeyboard& usbKeyboard,
                                         const uint8_t *keyLookup);
    //! Called when key pressed/released
    //! @param[in] id  The controller ID where this originated
    //! @param[in] key  The key that has been pressed or released
    //! @param[in] down  Set to true when down or false when up
    //! @returns true if key was successfully set
    //! @returns false if key could not be set
    bool keyStateChanged(uint8_t id, Key key, bool down);
    //! Called when controller connected
    //! @param[in] id  The controller ID where this originated
    void controllerConnected(uint8_t id);
    //! Called when controller disconnected
    //! @param[in] id  The controller ID where this originated
    void controllerDisconnected(uint8_t id);
    //! Called periodically to send the key states
    //! @returns true if keys sent or false if there was an eror
    virtual bool send();
  private:
    //! The USB keyboard I update
    UsbKeyboard& usbKeyboard;
    //! My mapping of genesis keys to USB keyboard keys
    const uint8_t* const keyLookup;
};

#endif // __USB_KEYBOARD_GENESIS_CONTROLLER_OBSERVER_H__
