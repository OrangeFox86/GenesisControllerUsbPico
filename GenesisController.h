#ifndef __GENESIS_CONTROLLER_H__
#define __GENESIS_CONTROLLER_H__

#include <stdint.h>
#include "configuration.h"
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
    //! All of the available input pins
    enum InputPinSelect
    {
      INPUT_PIN_UP_Z = 0, // Pin 1
      INPUT_PIN_DN_Y,     // Pin 2
      INPUT_PIN_LE_X,     // Pin 3
      INPUT_PIN_RI_MO,    // Pin 4
      INPUT_PIN_A_B,      // Pin 6
      INPUT_PIN_ST_C,     // Pin 9
      INPUT_PIN_COUNT
    };
  public:
    //! GenesisController constructor
    //! @param[in] id  The controller ID for this controller
    //! @param[in] pin1  GPIO index for Genesis pin 1 (in)
    //! @param[in] pin2  GPIO index for Genesis pin 2 (in)
    //! @param[in] pin3  GPIO index for Genesis pin 3 (in)
    //! @param[in] pin4  GPIO index for Genesis pin 4 (in)
    //! @param[in] pin6  GPIO index for Genesis pin 6 (in)
    //! @param[in] pin7  GPIO index for Genesis pin 7 (select out)
    //! @param[in] pin9  GPIO index for Genesis pin 9 (in)
    //! @param[in] pObserver  The observer to update whenever keys are pressed and released
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
    //! Called externally to flush all pressed/released keys to my observer
    void commitSignals();
    //! Called externally to transition signal to the next phase
    //! Note: First call will be 0
    //! @param[in] count  The current count [0,7]
    void nextSignal(uint8_t count, uint32_t inputs);
  protected:
    //! Processes signals saved to savedInputs
    void processSignals();
    //! Determines if the controller is connected based on values in savedInputs
    void checkConnection();
    //! Updates state due to controller just becoming connected
    void controllerConnected();
    //! Updates state due to controller just becoming disconnected
    void controllerDisconnected();
    //! Updates the state of a single key
    //! @param[in] key  The key to update
    //! @param[in] state  The state of the key
    void updateKey(IGenesisControllerObserver::Key key, bool state);
    //! Get the state of a single saved input
    //! @param[in] count  The count (time) value
    //! @param[in] pin  The pin to check
    //! @returns the state of the selected pin at the selected time
    bool getInput(uint8_t count, InputPinSelect pin);
    //! Sets the select (output) pin
    //! @param[in] state  The state to set the pin
    void setSelect(bool state);
  private:
    //! Default constructor not defined or used
    GenesisController();
  public:
    //! The maximum signal count value
    static const uint8_t NUM_COUNTS = 8;
  private:
    //! Set to true to invert inputs
    static const bool INVERT_INPUTS = INVERT_CONTROLLER_INPUTS;
    //! Set to true to invert outputs
    static const bool INVERT_OUTPUTS = INVERT_CONTROLLER_OUTPUTS;
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
