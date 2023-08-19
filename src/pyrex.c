#include "internal.h"

struct PrxState state = {0};
struct PrxWindowConfig window_init_config = {0};

int prx_initialize(void) {
	if (state.initialized) {
		return PRX_TRUE;
	}

	init_window_config();

	timer_init();
	state.timer_offset = timer_get_value();

	if (!SetProcessDpiAwarenessContext(
		DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
	{
		fprintf(
			stderr,
			"[WIN32 ERROR] SetProcessDpiAwarenessContext failed: %ld\n",
			GetLastError()
		);
		exit(EXIT_FAILURE);
	}

	state.initialized = PRX_TRUE;

	return PRX_TRUE;
}

void prx_terminate(void) {
	while (state.head_window) {
		prx_window_destroy(&state.head_window);
	}
	
	if (!UnregisterClassW(MAKEINTATOM(state.window_class), NULL)) {
		fprintf(
			stderr,
			"[WIN32 ERROR] UnregisterClassW failed: %lu\n",
			GetLastError()
		);
		exit(EXIT_FAILURE);
	}
}

void prx_process_events(void) {
	MSG message = {0};
	while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}
}

double prx_get_time(void) {
	return (double)(timer_get_value() - state.timer_offset) /
		timer_get_frequency();
}

void init_window_config(void) {
	window_init_config = (struct PrxWindowConfig){
		.pos_x = PRX_UNSPECIFIED,
		.pos_y = PRX_UNSPECIFIED,
		.resizable = PRX_TRUE,
		.maximised = PRX_FALSE,
		.visible = PRX_TRUE,
		.focused = PRX_TRUE,
		.decorated = PRX_TRUE,
		.dpi_scaled = PRX_FALSE,
	};
}

wchar_t *wide_string_from_utf8(const char *source) {
	int count = MultiByteToWideChar(CP_UTF8, 0, source, -1, NULL, 0);

	if (!count) {
		fprintf(
			stderr,
			"[WIN32 ERROR] MultiByteToWideChar failed (%lu)\n",
			GetLastError()
		);
		return NULL;
	}

	wchar_t *dest = calloc(count, sizeof *dest);
	if (!MultiByteToWideChar(CP_UTF8, 0, source, -1, dest, count)) {
		fprintf(
			stderr,
			"[WIN32 ERROR] MultiByteToWideChar failed (%lu)\n",
			GetLastError()
		);
		free(dest);
		return NULL;
	}

	return dest;
}

