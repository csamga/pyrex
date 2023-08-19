#include <stdlib.h>
#include <pyrex/pyrex.h>

int main(void)
{
	prx_initialize();

	prx_window_configure(PRX_DPI_SCALED, PRX_TRUE);
	PrxWindow *window = prx_window_create(720, 480, "Default Window");

	while (!prx_window_is_closed(window)) {
		prx_process_events();
	}

	prx_terminate();

	return EXIT_SUCCESS;
}

