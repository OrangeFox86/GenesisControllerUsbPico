#include "GenesisController.h"
#include "IGenesisControllerObserver.h"
#include "pico/stdlib.h"
#include <string.h>

GenesisController::GenesisController(uint8_t id,
                                     uint8_t pin1,
                                     uint8_t pin2,
                                     uint8_t pin3,
                                     uint8_t pin4,
                                     uint8_t pin6,
                                     uint8_t pin7,
                                     uint8_t pin9,
                                     IGenesisControllerObserver *pObserver) :
  id(id),
  inputPins{pin1, pin2, pin3, pin4, pin6, pin9},
  inputMasks{1UL << pin1, 1UL << pin2, 1UL << pin3, 1UL << pin4, 1UL << pin6, 1UL << pin9},
  allInputsMask(1UL << pin1 | 1UL << pin2 | 1UL << pin3 | 1UL << pin4 | 1UL << pin6 | 1UL << pin9),
  selectPin(pin7),
  allOutputsMask(1 << pin7),
  controllerState(STATE_DISCONNECTED),
  pObserver(pObserver),
  keyStatus(),
  controllerSensed(false),
  connectingCount(0)
{}

void GenesisController::pinInit()
{
  // Initialize all GPIO
  uint32_t allGpioMask = allInputsMask | allOutputsMask;
  gpio_init_mask(allGpioMask);
  gpio_set_dir_masked(allGpioMask, allOutputsMask);
  // Set all inputs as pull down
  const uint8_t* pPin = inputPins;
  for (uint32_t i = INPUT_PIN_COUNT; i > 0; --i, ++pPin)
  {
    // Assume external pull resistors
    gpio_disable_pulls(*pPin);
  }
  setSelect(true); // Initial select is HIGH
}

void GenesisController::nextSignal(uint8_t count, uint32_t inputs)
{
  if (controllerState == STATE_CONNECT_INIT || controllerState == STATE_CONNECTED)
  {
    // LOW at count 0 and even number counts
    // HIGH on odd number counts
    setSelect((count & 1) != 0);
  }
  else
  {
    // Keep select HIGH until controller is sensed
    setSelect(true);
  }
  // Saved input is for the previously set select state
  // Inputs for count 0 means the state while idle HIGH
  savedInputs[count] = inputs;
  if (count == NUM_COUNTS - 1)
  {
    // Reached the max count; time to process all of the saved inputs
    processSignals();
  }
}

void GenesisController::commitSignals()
{
  if (pObserver != nullptr)
  {
    // Time to commit all key changes
    pObserver->send();
  }
}

void GenesisController::checkConnection()
{
  bool lastState = controllerSensed;
  // Sensed when anything is HIGH when not inverted (assumes pull-down is used)
  // Or when anything is LOW when inverted (assumes pull-up is used)
  controllerSensed = (savedInputs[0] & allInputsMask) != (INVERT_INPUTS ? allInputsMask : 0);
  if (lastState != controllerSensed)
  {
    if (controllerSensed)
    {
      controllerConnected();
    }
    else
    {
      controllerDisconnected();
    }
  }
}

void GenesisController::controllerConnected()
{
  if (pObserver != nullptr)
  {
    pObserver->controllerConnected(id);
  }
  controllerState = STATE_CONNECTING;
  memset(keyStatus, 0, sizeof(keyStatus));
  isSixButton = false;
  connectingCount = 0;
}

void GenesisController::controllerDisconnected()
{
  if (pObserver != nullptr)
  {
    pObserver->controllerDisconnected(id);
  }
  controllerState = STATE_DISCONNECTED;
  memset(keyStatus, 0, sizeof(keyStatus));
  isSixButton = false;
  connectingCount = 0;
}

void GenesisController::updateKey(IGenesisControllerObserver::Key key, bool state)
{
  if (INVERT_INPUTS)
  {
    state = !state;
  }
  if (keyStatus[key] != state)
  {
    keyStatus[key] = state;
    if (pObserver != nullptr)
    {
      pObserver->keyStateChanged(id, key, state);
    }
  }
}

void GenesisController::processSignals()
{
  checkConnection();

  switch (controllerState)
  {
    default: // Fall through
    case STATE_DISCONNECTED:
      // Nothing further to do
      break;

    case STATE_CONNECTING:
    {
      // Stay at connecting state until MAX_CONNECTING_COUNT has been reached */
      if (++connectingCount >= MAX_CONNECTING_COUNT)
      {
        connectingCount = 0;
        controllerState = STATE_CONNECT_INIT;
      }
    }
    break;

    case STATE_CONNECT_INIT:
    {
			// Purpose of this state is to figure out what kind of controller we have (3 or 6 button).
      isSixButton = getInput(7, INPUT_PIN_RI_MO);
      controllerState = STATE_CONNECTED;
    }
    break;

    case STATE_CONNECTED:
    {
      // Get next button state
      updateKey(IGenesisControllerObserver::KEY_A, !getInput(3, INPUT_PIN_A_B));
      updateKey(IGenesisControllerObserver::KEY_START, !getInput(3, INPUT_PIN_ST_C));
      updateKey(IGenesisControllerObserver::KEY_UP, !getInput(2, INPUT_PIN_UP_Z));
      updateKey(IGenesisControllerObserver::KEY_DOWN, !getInput(2, INPUT_PIN_DN_Y));
      updateKey(IGenesisControllerObserver::KEY_LEFT, !getInput(2, INPUT_PIN_LE_X));
      updateKey(IGenesisControllerObserver::KEY_RIGHT, !getInput(2, INPUT_PIN_RI_MO));
      updateKey(IGenesisControllerObserver::KEY_B, !getInput(2, INPUT_PIN_A_B));
      updateKey(IGenesisControllerObserver::KEY_C, !getInput(2, INPUT_PIN_ST_C));
      if (isSixButton)
      {
        updateKey(IGenesisControllerObserver::KEY_Z, !getInput(6, INPUT_PIN_UP_Z));
        updateKey(IGenesisControllerObserver::KEY_Y, !getInput(6, INPUT_PIN_DN_Y));
        updateKey(IGenesisControllerObserver::KEY_X, !getInput(6, INPUT_PIN_LE_X));
        updateKey(IGenesisControllerObserver::KEY_MODE, !getInput(6, INPUT_PIN_RI_MO));
      }
    }
    break;
  }
}

bool GenesisController::getInput(uint8_t count, InputPinSelect pin)
{
  bool input = ((inputMasks[pin] & savedInputs[count]) != 0);
  if (INVERT_INPUTS)
  {
    input = !input;
  }
  return input;
}

void GenesisController::setSelect(bool state)
{
  gpio_put(selectPin, INVERT_OUTPUTS ? !state : state);
}
