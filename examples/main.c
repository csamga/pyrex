#include <stdlib.h>
#include <stdio.h>

#include <pyrex/pyrex.h>

struct UserData {
	int x;
	float y;
};

void foo(PrxWindow *window)
{
	struct UserData *data = prx_window_get_user_data(window);
	printf("%d %f\n", data->x, data->y);
}

int main(void)
{
	PrxWindow *window = prx_window_create(720, 480, "Pyrex");

	struct UserData data = {
		.x = 10,
		.y = 37.9,
	};

	prx_window_set_user_data(window, &data);

	while (!prx_window_had_close_request(window)) {
		prx_process_events();
	}

	foo(window);

	prx_window_destroy(&window);

	return EXIT_SUCCESS;
}

