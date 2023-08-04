#include "pyrex_internal.h"

struct _PrxInternal _prx_internal = {0};

// PUBLIC API
//
PRXAPI void prx_process_events(void) {
	MSG message = {0};
	while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}
}

// PRIVATE API
//
wchar_t *_wide_string_from_utf8(const char *source)
{
	int count = MultiByteToWideChar(CP_UTF8, 0, source, -1, NULL, 0);

	if (!count) {
		fprintf(stderr,
				"[WIN32 ERROR] MultiByteToWideChar failed (%lu)\n",
				GetLastError());
		return NULL;
	}

	wchar_t *dest = calloc(count, sizeof *dest);
	if (!MultiByteToWideChar(CP_UTF8, 0, source, -1, dest, count)) {
		fprintf(stderr,
				"[WIN32 ERROR] MultiByteToWideChar failed (%lu)\n",
				GetLastError());
		free(dest);
		return NULL;
	}

	return dest;
}

