#include <stdlib.h>
#include <pyrex/pyrex.h>

int main(void)
{
	prx_initialize();

	PrxWindow *window_1 = prx_window_create(600, 200, "window 1");
	PrxWindow *window_2 = prx_window_create(600, 400, "window 2");

	while (1) {
		prx_process_events();

		if (window_1 != NULL && prx_window_is_closed(window_1)) {
			prx_window_destroy(&window_1);
		}
		if (window_2 != NULL && prx_window_is_closed(window_2)) {
			prx_window_destroy(&window_2);
		}

		if (!window_1 && !window_2) {
			break;
		}
	}

	prx_terminate();

	return EXIT_SUCCESS;
}

