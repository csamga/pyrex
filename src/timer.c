#include "internal.h"

static uint64_t frequency;

void timer_init(void) {
	QueryPerformanceFrequency((LARGE_INTEGER *)&frequency);
}

uint64_t timer_get_frequency(void) {
	return frequency;
}

uint64_t timer_get_value(void) {
	uint64_t time;
	QueryPerformanceCounter((LARGE_INTEGER *)&time);
	return time;
}

