#include <stdlib.h>
#include <stdio.h>

#include <pyrex/pyrex.h>

void resized_callback(PrxWindow *window, int width, int height) {
	int window_w, window_h, client_w, client_h;
	prx_window_get_size(window, &window_w, &window_h);
	prx_window_get_client_size(window, &client_w, &client_h);

	char title[128];
	sprintf(
		title,
		"callback: %d - %d, window: %d - %d, client: %d - %d",
		width, height,
		window_w, window_h,
		client_w, client_h
	);
	prx_window_set_title(window, title);
}

void mouse_moved_callback(PrxWindow *window, int x, int y) {
	char title[32];
	sprintf(title, "%d - %d", x, y);
	prx_window_set_title(window, title);
}

int main(void)
{
	prx_initialize();

	// prx_window_configure(PRX_DPI_SCALED, PRX_TRUE);
	PrxWindow *window = prx_window_create(500, 500, "DPI Scaling");

	prx_window_set_window_resized_callback(window, resized_callback);
	prx_window_set_mouse_moved_callback(window, mouse_moved_callback);
	
	int window_w, window_h, client_w, client_h;
	prx_window_get_size(window, &window_w, &window_h);
	prx_window_get_client_size(window, &client_w, &client_h);

	char title[128];
	sprintf(
		title,
		"window: %d - %d, client: %d - %d",
		window_w, window_h,
		client_w, client_h
	);
	prx_window_set_title(window, title);

	// prx_window_set_size_limits(
	// 	window,
	// 	500, 500,
	// 	PRX_UNSPECIFIED, PRX_UNSPECIFIED
	// );

	while (!prx_window_is_closed(window)) {
		prx_process_events();
	}

	prx_terminate();

	return EXIT_SUCCESS;
}

