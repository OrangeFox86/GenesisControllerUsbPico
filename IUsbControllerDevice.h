#ifndef __I_USB_CONTROLLER_DEVICE_H__
#define __I_USB_CONTROLLER_DEVICE_H__

#include <stdint.h>

//! Interface for defining a USB controller device
class IUsbControllerDevice
{
  public:
    inline IUsbControllerDevice() {}
    inline virtual ~IUsbControllerDevice() {}
    //! @returns true iff there is at least 1 button pressed on the device
    virtual bool isButtonPressed() = 0;
    //! Release all currently pressed buttons
    virtual void updateAllReleased() = 0;
    //! Updates the host with pressed buttons
    //! @param[in] force  Set to true to update host regardless if key state has changed since last
    //!                   update
    //! @returns true if data has been successfully sent or if buttons didn't need to be updated
    virtual bool send(bool force = false) = 0;
    //! Gets the report for the currently pressed buttons
    //! @param[out] buffer  Where the report is written
    //! @param[in] reqlen  The length of buffer
    virtual void getReport(uint8_t *buffer, uint16_t reqlen) = 0;
    //! Called only from callbacks to update connected state
    //! @param[in] connected  true iff connected
    virtual void updateConnected(bool connected) = 0;
    //! @returns the current connected state
    virtual bool isConnected() = 0;
};

#endif // __I_USB_CONTROLLER_DEVICE_H__
