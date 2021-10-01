#ifndef __USB_GAMEPAD_H__
#define __USB_GAMEPAD_H__

#include <stdint.h>
#include "IUsbControllerDevice.h"

//! This class is designed to work with the setup code in usb_descriptors.c
class UsbGamepad : public IUsbControllerDevice
{
  public:
  enum DpadButtons
  {
    DPAD_UP = 0,
    DPAD_DOWN,
    DPAD_LEFT,
    DPAD_RIGHT,
    DPAD_COUNT
  };

  public:
    //! UsbKeyboard constructor
    //! @param[in] reportId  The report ID to use for this USB keyboard
    UsbGamepad(uint8_t reportId = 0);
    //! @returns true iff any button is currently "pressed"
    bool isButtonPressed() final;

    void setAnalogThumbX(bool isLeft, int8_t x);
    void setAnalogThumbY(bool isLeft, int8_t y);
    void setAnalogTrigger(bool isLeft, int8_t z);
    int8_t getAnalogThumbX(bool isLeft);
    int8_t getAnalogThumbY(bool isLeft);
    int8_t getAnalogTrigger(bool isLeft);
    void setDigitalPad(DpadButtons button, bool isPressed);
    void setButtonMask(uint16_t mask, bool isPressed);
    void setButton(uint8_t button, bool isPressed);

    //! Release all currently pressed keys
    void updateAllReleased() final;
    //! Updates the host with any newly pressed keys
    //! @param[in] force  Set to true to update host regardless if key state has changed since last
    //!                   update
    //! @returns true if data has been successfully sent or if keys didn't need to be updated
    bool send(bool force = false) final;
    //! Gets the report for the currently pressed keys
    //! @param[out] buffer  Where the report is written
    //! @param[in] reqlen  The length of buffer
    void getReport(uint8_t *buffer, uint16_t reqlen) final;
    //! Called only from callbacks to update connected state
    //! @param[in] connected  true iff connected
    void updateConnected(bool connected) final;
    //! @returns the current connected state
    bool isConnected() final;

  protected:
    uint8_t getHatValue();

  private:
    //! The report ID to use when sending keys to host
    const uint8_t reportId;
    //! Current left analog states (x,y,z)
    int8_t currentLeftAnalog[3];
    //! Current right analog states (x,y,z)
    int8_t currentRightAnalog[3];
    //! Current d-pad buttons
    bool currentDpad[DPAD_COUNT];
    //! Current button states
    uint16_t currentButtons;
    //! True when something has been updated since the last successful send
    bool buttonsUpdated;
    //! True when this USB device is connected to a host
    bool mIsConnected;
};

#endif // __USB_CONTROLLER_H__
