#ifndef __USB_CONTROLLER_H__
#define __USB_CONTROLLER_H__

#include <stdint.h>

//! This class is designed to work with the setup code in usb_descriptors.c
//! @note this doesn't support analog sticks
class UsbController
{
  public:
    enum button_e
    {
      BUTTON_UP = 0,
      BUTTON_DOWN,
      BUTTON_LEFT,
      BUTTON_RIGHT,
      DPAD_MAX = BUTTON_RIGHT,
      BUTTON_A,
      BUTTON_B,
      BUTTON_C,
      BUTTON_X,
      BUTTON_Y,
      BUTTON_Z,
      BUTTON_START,
      BUTTON_MODE,
      BUTTON_COUNT
    };
  public:
    //! UsbKeyboard constructor
    //! @param[in] reportId  The report ID to use for this USB keyboard
    UsbController(uint8_t reportId = 0);
    //! @returns true iff any key is currently "pressed"
    bool isButtonPressed();
    //! Set a button to be pressed
    bool updatePressed(button_e button);
    //! Set a button to be released
    bool updateReleased(button_e button);
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
    //! @returns a reference to a controller, given an index
    static UsbController& getController(uint8_t controllerIndex);

  protected:
    //! Controls the state of the LED based on connected and button states
    static void ledTask();

  public:
    //! The number of static controllers created
    static const uint8_t NUMBER_OF_CONTROLLERS = 2;
  private:
    //! The report ID to use when sending keys to host
    const uint8_t reportId;
    //! Current d-pad state
    uint8_t currentDpad;
    //! Current button states
    uint32_t currentButtons;
    //! True when currentDpad or currentButtons have been updated since the last successful send
    bool buttonsUpdated;
    //! True when this USB device is connected to a host
    bool mIsConnected;
};

#endif // __USB_CONTROLLER_H__
