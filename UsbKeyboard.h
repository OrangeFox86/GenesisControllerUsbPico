#ifndef __USB_KEYBOARD_H__
#define __USB_KEYBOARD_H__

#include <stdint.h>

//! This class is designed to work with the setup code in usb_descriptors.c
//! @note This class does not allow modifier keys such as shift
class UsbKeyboard
{
  public:
    //! UsbKeyboard constructor
    //! @param[in] reportId  The report ID to use for this USB keyboard
    UsbKeyboard(uint8_t reportId = 0);
    //! @returns true iff any key is currently "pressed"
    bool isKeyPressed();
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
    bool sendKeys(bool force = false);
    //! Gets the report for the currently pressed keys
    //! @param[out] buffer  Where the report is written
    //! @param[in] reqlen  The length of buffer
    void getReport(uint8_t *buffer, uint16_t reqlen);
    //! Called only from callbacks to update connected state
    //! @param[in] connected  true iff connected
    void updateConnected(bool connected);
    //! @returns the current connected state
    bool isConnected();
    //! Initializes USB
    static void init();
    //! The USB task which must be periodically called by main()
    static void task();

  protected:
    //! Controls the state of the LED based on connected and button states
    static void ledTask();

  private:
    //! The report ID to use when sending keys to host
    const uint8_t reportId;
    //! List of currently pressed keys
    uint8_t currentKeycodes[6];
    //! True when currentKeycodes has been updated since the last successful sendKeys call
    bool keycodesUpdated;
    //! True when at least 1 key is pressed
    bool keyPressed;
    //! True when this USB device is connected to a host
    bool mIsConnected;
};

//! All of the available keyboards to use
extern UsbKeyboard gKeyboards[2];

#endif // __USB_KEYBOARD_H__
