#include <stdlib.h>
#include <stdio.h>

#include <pyrex/pyrex.h>

static const char *window_title = "Callbacks";

void window_resized_callback(PrxWindow *window, int width, int height) {
	char title[64];
	char event[16] = "Window resized";
	sprintf(title, "%s - %s: (%d, %d)\n", window_title,	event, width, height);
	prx_window_set_title(window, title);
}

void window_moved_callback(PrxWindow *window, int x, int y) {
	char title[64];
	char event[16] = "Window moved";
	sprintf(title, "%s - %s: (%d, %d)\n", window_title, event, x, y);
	prx_window_set_title(window, title);
}

void mouse_moved_callback(PrxWindow *window, int x, int y) {
	char title[64];
	char event[16] = "Mouse moved";
	sprintf(title, "%s - %s: (%d, %d)", window_title, event, x, y);
	prx_window_set_title(window, title);
}

int main(void)
{
	prx_initialize();

	prx_window_configure(PRX_DPI_SCALED, PRX_TRUE);
	// prx_window_configure(PRX_MIN_WIDTH, 500);
	// prx_window_configure(PRX_MIN_HEIGHT, 500);
	PrxWindow *window = prx_window_create(720, 480, window_title);

	prx_window_set_window_resized_callback(window, window_resized_callback);
	prx_window_set_window_moved_callback(window, window_moved_callback);
	prx_window_set_mouse_moved_callback(window, mouse_moved_callback);

	while (!prx_window_is_closed(window)) {
		prx_process_events();
	}

	prx_terminate();

	return EXIT_SUCCESS;
}

