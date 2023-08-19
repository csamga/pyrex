#include <stdlib.h>
#include <stdio.h>

#include <pyrex/pyrex.h>

struct UserData {
	int x;
	float y;
};

void foo(PrxWindow *window) {
	struct UserData *data = prx_window_get_user_pointer(window);
	printf("%d %f\n", data->x, data->y);
}

int main(void)
{
	prx_initialize();

	PrxWindow *window = prx_window_create(720, 480, "Pyrex");

	struct UserData data = {
		.x = 10,
		.y = 37.9,
	};

	prx_window_set_user_pointer(window, &data);

	while (!prx_window_is_closed(window)) {
		prx_process_events();
	}

	foo(window);

	prx_window_destroy(&window);
	prx_terminate();

	return EXIT_SUCCESS;
}

