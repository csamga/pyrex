#ifndef PRX_TIMER_INCLUDED
#define PRX_TIMER_INCLUDED

#include <stdint.h>

void timer_init(void);
uint64_t timer_get_frequency(void);
uint64_t timer_get_value(void);

#endif // PRX_TIMER_INCLUDED

