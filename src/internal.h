#include "../include/pyrex/pyrex.h"

#ifndef UNICODE
#	define UNICODE
#endif // UNICODE
#ifndef _UNICODE
#	define _UNICODE
#endif // _UNICODE

#define WIN32_LEAN_AND_MEAN
#define _WINVER 0x0A00
#define _WIN32_WINNT 0x0A00
#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "timer.h"

extern struct PrxState {
	PrxWindow *head_window;
	HINSTANCE instance;
	ATOM window_class;
	uint64_t timer_offset;
	bool initialized;
} state;

extern struct PrxWindowConfig {
	int pos_x, pos_y;
	bool resizable;
	bool maximised;
	bool visible;
	bool focused;
	bool decorated;
	bool dpi_scaled;
} window_init_config;

void init_window_config(void);
wchar_t *wide_string_from_utf8(const char *source);

