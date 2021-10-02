#ifndef __USB_KEYBOARD_H__
#define __USB_KEYBOARD_H__

#include <stdint.h>
#include "IUsbControllerDevice.h"

//! This class is designed to work with the setup code in usb_descriptors.c
//! @note This class does not allow modifier keys such as shift
class UsbKeyboard : public IUsbControllerDevice
{
  public:
    //! UsbKeyboard constructor
    //! @param[in] reportId  The report ID to use for this USB keyboard
    UsbKeyboard(uint8_t interfaceId, uint8_t reportId = 0);
    //! @returns true iff any key is currently "pressed"
    bool isButtonPressed();
    //! Set a key to be pressed
    //! @param[in] keycode  The keycode to update. This must be a HID_KEY_* value within hid.h.
    bool updatePressed(uint8_t keycode);
    //! Set a key to be released
    //! @param[in] keycode  The keycode to update. This must be a HID_KEY_* value within hid.h.
    bool updateReleased(uint8_t keycode);
    //! Release all currently pressed keys
    void updateAllReleased();
    //! Updates the host with any newly pressed keys
    //! @param[in] force  Set to true to update host regardless if key state has changed since last
    //!                   update
    //! @returns true if data has been successfully sent or if keys didn't need to be updated
    bool send(bool force = false);
    //! Gets the report for the currently pressed keys
    //! @param[out] buffer  Where the report is written
    //! @param[in] reqlen  The length of buffer
    void getReport(uint8_t *buffer, uint16_t reqlen);

  public:
    //! The number of static controllers created
    static const uint8_t NUMBER_OF_KEYBOARDS = 2;
  private:
    const uint8_t interfaceId;
    //! The report ID to use when sending keys to host
    const uint8_t reportId;
    //! List of currently pressed keys
    uint8_t currentKeycodes[6];
    //! True when currentKeycodes has been updated since the last successful send call
    bool keycodesUpdated;
    //! True when at least 1 key is pressed
    bool keyPressed;
};

#endif // __USB_KEYBOARD_H__
