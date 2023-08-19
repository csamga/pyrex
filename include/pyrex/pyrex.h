#ifndef PYREX_H_INCLUDED
#define PYREX_H_INCLUDED

#define PRX_FALSE 	0
#define PRX_TRUE 	1

#define PRX_UNSPECIFIED -1

#define PRX_POS_X 		0x00001000
#define PRX_POS_Y 		0x00001001
#define PRX_MAXIMISED 	0x00001002
#define PRX_RESIZABLE 	0x00001003
#define PRX_VISIBLE   	0x00001004
#define PRX_FOCUSED   	0x00001005
#define PRX_DECORATED 	0x00001006
#define PRX_DPI_SCALED 	0x00001007

typedef struct PrxWindow PrxWindow;

typedef void (*PrxWindowDestroyedCb)(PrxWindow *);
typedef void (*PrxWindowMovedCb)(PrxWindow *, int, int);
typedef void (*PrxWindowResizedCb)(PrxWindow *, int, int);
typedef void (*PrxWindowClosedCb)(PrxWindow *);
typedef void (*PrxMouseMovedCb)(PrxWindow *, int, int);

int prx_initialize(void);
void prx_terminate(void);
void prx_process_events(void);

PrxWindow *prx_window_create(int width, int height, const char *title);
void prx_window_destroy(PrxWindow **window);

void prx_window_configure(int setting, int value);

void prx_window_get_size(PrxWindow *, int *width, int *height);
void prx_window_get_client_size(PrxWindow *, int *width, int *height);

typedef struct HWND__ *HWND;
HWND prx_window_get_win32_handle(PrxWindow *window);

void prx_window_set_user_pointer(PrxWindow *window, void *data);
void *prx_window_get_user_pointer(PrxWindow *window);

void prx_window_set_title(PrxWindow *window, const char *title);
char *prx_window_get_title(PrxWindow *window);

void prx_window_set_size_limits(
	PrxWindow *window,
	int min_width, int min_height,
	int max_width, int max_height
);

int prx_window_is_closed(PrxWindow *window);

void prx_window_set_window_destroyed_callback(
	PrxWindow *window,
	PrxWindowDestroyedCb callback
);
void prx_window_set_window_moved_callback(
	PrxWindow *window,
	PrxWindowMovedCb callback
);
void prx_window_set_window_resized_callback(
	PrxWindow *window,
	PrxWindowResizedCb callback
);
void prx_window_set_window_closed_callback(
	PrxWindow *window,
	PrxWindowClosedCb callback
);
void prx_window_set_mouse_moved_callback(
	PrxWindow *window,
	PrxMouseMovedCb callback
);

double prx_get_time(void);

#endif // PYREX_H_INCLUDED

