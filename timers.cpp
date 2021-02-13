#include "timers.h"
#include "GenesisController.h"
#include "pico/stdlib.h"

//! The controllers array used by these timers
static GenesisController *gTimerControllers = nullptr;
//! The number of controllers stored in gTimerControllers
static uint32_t gTimerControllersCount = 0;

void init_timer(GenesisController *controllers, uint32_t count)
{
  gTimerControllers = controllers;
  gTimerControllersCount = count;
}

// Note: Pico can't seem to handle reload times of 6 us very well. When attempted, trigger times
//       ranged from 9 us to 12 us. Since this chip has two cores, it's not much of an issue to use
//       an entire core just for timing :)
void timer_process()
{
  absolute_time_t nextTime;
  update_us_since_boot(&nextTime, time_us_64());
  while (1)
  {
    // Wait about 20 ms
    update_us_since_boot(&nextTime, to_us_since_boot(nextTime) + 20000);
    busy_wait_until(nextTime);

    for (uint8_t count = 0; count < GenesisController::NUM_COUNTS; ++count)
    {
      // Wait about 6 us
      update_us_since_boot(&nextTime, to_us_since_boot(nextTime) + 6);
      busy_wait_until(nextTime);
      // Sample all inputs
      uint32_t inputs = gpio_get_all();
      // Process the current cycle state
      GenesisController *controller = gTimerControllers;
      for (uint32_t i = gTimerControllersCount; i > 0; --i, ++controller)
      {
        controller->nextSignal(count, inputs);
      }
    }
    // Commit all signals now
    GenesisController *controller = gTimerControllers;
    for (uint32_t i = gTimerControllersCount; i > 0; --i, ++controller)
    {
      controller->commitSignals();
    }
  }
}
