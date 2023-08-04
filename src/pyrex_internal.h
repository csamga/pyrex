#include "../include/pyrex/pyrex.h"

#define WIN32_LEAN_AND_MEAN

#ifndef UNICODE
#	define UNICODE
#endif // UNICODE
#ifndef _UNICODE
#	define _UNICODE
#endif // _UNICODE

#define _WINVER 0x0A00
#define _WIN32_WINNT 0x0A00

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

struct _PrxInternal {
	HINSTANCE instance;
	ATOM window_class;
};

extern struct _PrxInternal _prx_internal;

struct _PrxWindow {
	HWND handle;
	unsigned int client_width;
	unsigned int client_height;
	bool had_close_request;
	void *user_data;
};

wchar_t *_wide_string_from_utf8(const char *source);

