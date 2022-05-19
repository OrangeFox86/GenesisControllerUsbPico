#ifndef __USB_CONTROLLER_GENESIS_CONTROLLER_OBSERVER_H__
#define __USB_CONTROLLER_GENESIS_CONTROLLER_OBSERVER_H__

#include "IGenesisControllerObserver.h"
#include "UsbGamepad.h"

//! Yes, I know this name is ridiculous, but at least it's descriptive!
class UsbGamepadGenesisControllerObserver : public IGenesisControllerObserver
{
  public:
    //! Enumerates all available options to map a button to a USB gamepad
    enum ButtonMap
    {
      MAP_NONE = 0,
      MAP_LANALOG_UP,
      MAP_LANALOG_DOWN,
      MAP_LANALOG_LEFT,
      MAP_LANALOG_RIGHT,
      MAP_RANALOG_UP,
      MAP_RANALOG_DOWN,
      MAP_RANALOG_LEFT,
      MAP_RANALOG_RIGHT,
      MAP_LTRIGGER,
      MAP_RTRIGGER,
      MAP_DPAD_UP,
      MAP_DPAD_DOWN,
      MAP_DPAD_LEFT,
      MAP_DPAD_RIGHT,
      MAP_BUTTON0,
      MAP_BUTTON1,
      MAP_BUTTON2,
      MAP_BUTTON3,
      MAP_BUTTON4,
      MAP_BUTTON5,
      MAP_BUTTON6,
      MAP_BUTTON7,
      MAP_BUTTON8,
      MAP_BUTTON9,
      MAP_BUTTON10,
      MAP_BUTTON11,
      MAP_BUTTON12,
      MAP_BUTTON13,
      MAP_BUTTON14,
      MAP_BUTTON15,
      MAP_COUNT
    };
  public:
    //! Constructor for UsbKeyboardGenesisControllerObserver
    //! @param[in] usbController  The USB controller to update when keys are pressed or released
    UsbGamepadGenesisControllerObserver(UsbGamepad& usbController,
                                        const ButtonMap *buttonLookup);
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
    //! The USB controller I update
    UsbGamepad& usbController;
    //! My mapping of genesis buttons to USB gamepad buttons
    const ButtonMap* const buttonLookup;
};

#endif // __USB_CONTROLLER_GENESIS_CONTROLLER_OBSERVER_H__
