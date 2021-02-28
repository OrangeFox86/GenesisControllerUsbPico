#ifndef __TIMERS_H__
#define __TIMERS_H__

#include <stdint.h>

// Forward declaration of a Genesis Controller object
class GenesisController;

//! Initializes timer data
//! @param[in] controllers  List of controllers the timers must update
//! @param[in] count  The number of elements in controllers
void init_timer(GenesisController *controllers, uint32_t count);

//! The timer process which must be run on its own core (never returns)
void timer_process();

#endif // __TIMERS_H__
