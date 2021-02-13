#ifndef __TIMERS_H__
#define __TIMERS_H__

#include <stdint.h>

class GenesisController;

void init_timer(GenesisController *controllers, uint32_t count);

void timer_process();

#endif // __TIMERS_H__
