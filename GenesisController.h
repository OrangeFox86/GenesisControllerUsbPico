#ifndef __GENESIS_CONTROLLER_H__
#define __GENESIS_CONTROLLER_H__

#include <stdint.h>
#include "IGenesisControllerObserver.h"

class GenesisController
{
  public:
    //! States for the controller state machine
    enum State
    {
      STATE_DISCONNECTED = 0,
      STATE_CONNECTING,
      STATE_CONNECT_INIT,
      STATE_CONNECTED
    };
  protected:
    //!
    enum InputPinSelect
    {
      INPUT_PIN_UP_Z = 0,
      INPUT_PIN_DN_Y,
      INPUT_PIN_LE_X,
      INPUT_PIN_RI_MO,
      INPUT_PIN_A_B,
      INPUT_PIN_ST_C,
      INPUT_PIN_COUNT
    };
  public:
    //! GenesisController constructor
    //! @param[in] id  The controller ID for this controller
    //! @param[in] pin1  GPIO index for Genesis pin 1
    //! @param[in] pin2  GPIO index for Genesis pin 2
    //! @param[in] pin3  GPIO index for Genesis pin 3
    //! @param[in] pin4  GPIO index for Genesis pin 4
    //! @param[in] pin6  GPIO index for Genesis pin 6
    //! @param[in] pin7  GPIO index for Genesis pin 7
    //! @param[in] pin9  GPIO index for Genesis pin 9
    GenesisController(uint8_t id,
                      uint8_t pin1,
                      uint8_t pin2,
                      uint8_t pin3,
                      uint8_t pin4,
                      uint8_t pin6,
                      uint8_t pin7,
                      uint8_t pin9,
                      IGenesisControllerObserver *pObserver);
    //! Initializes all of my pins
    void pinInit();
    //! Called externally to flush all pressed keys to my observer
    void commitSignals();
    //! Called externally to transition signal to the next phase
    //! Note: First call will be 0
    //! @param[in] count  The current count [0,7]
    void nextSignal(uint8_t count, uint32_t inputs);
  protected:
    void processSignals();
    void checkConnection();
    void controllerConnected();
    void controllerDisconnected();
    void updateKey(IGenesisControllerObserver::Key key, bool state);
    bool getInput(uint8_t count, InputPinSelect pin);
    void setSelect(bool state);
  private:
    //! Default constructor not defined or used
    GenesisController();
  public:
    //! The maximum signal count value
    static const uint8_t NUM_COUNTS = 8;
  private:
    //! Set to true to invert inputs
    //! @note This assumes external pull-down resistor used when false or pull-up when true
    static const bool INVERT_INPUTS = false;
    //! Set to true to invert outputs
    static const bool INVERT_OUTPUTS = true; // An NPN is used which inverts the output
    //! Number of connecting cycles to make before becomming connected
    static const uint8_t MAX_CONNECTING_COUNT = 25;
    //! The ID for this controller
    const uint8_t id;
    //! GPIO indicies for input pins
    const uint8_t inputPins[INPUT_PIN_COUNT];
    //! GPIO masks for input pins
    const uint32_t inputMasks[INPUT_PIN_COUNT];
    //! Mask which represents all inputs
    const uint32_t allInputsMask;
    //! GPIO index for select pin (7)
    const uint8_t selectPin;
    //! Mask which represents all outputs
    const uint32_t allOutputsMask;
    //! The current controller state
    State controllerState;
    //! The observer which is called on state changes
    IGenesisControllerObserver *pObserver;
    //! Current status of all keys (true when pressed)
    bool keyStatus[IGenesisControllerObserver::KEY_COUNT];
    //! Set to true if controller is sensed; false otherwise
    bool controllerSensed;
    //! Set to true if controller is six button; false otherwise
    bool isSixButton;
    //! The count kept while connecting
    uint8_t connectingCount;
    //! All the saved input signals
    uint32_t savedInputs[NUM_COUNTS];
};

#endif // __GENESIS_CONTROLLER_H__
