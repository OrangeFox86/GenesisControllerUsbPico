#ifndef __I_GENESIS_CONTROLLER_OBSERVER_H__
#define __I_GENESIS_CONTROLLER_OBSERVER_H__

#include <stdint.h>

class IGenesisControllerObserver
{
  public:
    //! Definition for each key on a genesis controller
    enum Key
    {
      KEY_START = 0,
      KEY_UP,
      KEY_DOWN,
      KEY_LEFT,
      KEY_RIGHT,
      KEY_A,
      KEY_B,
      KEY_C,
      KEY_X,
      KEY_Y,
      KEY_Z,
      KEY_MODE,
      KEY_COUNT
    };
    //! Called when key pressed/released
    //! @param[in] id  The controller ID where this originated
    //! @param[in] key  The key that has been pressed or released
    //! @param[in] down  Set to true when down or false when up
    //! @returns true if key was successfully set
    //! @returns false if key could not be set
    virtual bool keyStateChanged(uint8_t id, Key key, bool down) = 0;
    //! Called when controller connected
    //! @param[in] id  The controller ID where this originated
    virtual void controllerConnected(uint8_t id) = 0;
    //! Called when controller disconnected
    //! @param[in] id  The controller ID where this originated
    virtual void controllerDisconnected(uint8_t id) = 0;
    //! Called periodically to send the key states
    //! @returns true if keys sent or false if there was an eror
    virtual bool send() = 0;
};

#endif // __I_GENESIS_CONTROLLER_OBSERVER_H__
