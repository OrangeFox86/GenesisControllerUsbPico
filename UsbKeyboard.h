#ifndef __USB_KEYBOARD_H__
#define __USB_KEYBOARD_H__

#include <stdint.h>

//! This class is designed to work with the setup code in usb_descriptors.c
class UsbKeyboard
{
  public:
    UsbKeyboard(uint8_t reportId = 0);

    bool isKeyPressed();

    bool updatePressed(uint8_t keycode);
    bool updateReleased(uint8_t keycode);
    void updateAllReleased();
    bool sendKeys(bool force = false);

    void getReport(uint8_t *buffer, uint16_t reqlen);

    void updateConnected(bool connected);
    bool isConnected();

    static void init();
    static void task();

  protected:
    static void ledTask();

  private:
    const uint8_t reportId;
    uint8_t currentKeycodes[6];
    bool keycodesUpdated;
    bool keyPressed;
    bool mIsConnected;
};

extern UsbKeyboard gKeyboards[1];

bool is_usb_connected();

#endif // __USB_KEYBOARD_H__
