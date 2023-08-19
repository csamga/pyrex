#include <stdlib.h>
#include <stdio.h>

#include <pyrex/pyrex.h>

int main(void)
{
	prx_initialize();
	PrxWindow *window = prx_window_create(720, 480, "");

	double prev_time = 0.0;

	while (!prx_window_is_closed(window)) {
		prx_process_events();

		double current_time = prx_get_time();

		if (current_time - 0.001 >= prev_time) {
			prev_time = current_time;
			char title[32];
			sprintf(title, "%.3fms", current_time);
			prx_window_set_title(window, title);
		}
	}

	prx_terminate();

	return EXIT_SUCCESS;
}

