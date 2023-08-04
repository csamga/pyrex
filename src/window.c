#include "pyrex_internal.h"

#include <math.h>

static LRESULT window_procedure(HWND window_handle,
								UINT message,
								WPARAM wparam,
								LPARAM lparam);

PRXAPI PrxWindow *prx_window_create(unsigned int width,
									unsigned int height,
									const char *title)
{
	struct _PrxWindow *window = malloc(sizeof *window);

	if (!window) {
		fprintf(stderr, "[ERROR] malloc for *window failed\n");
		exit(EXIT_FAILURE);
	}

	window->client_width = width;
	window->client_height = height;
	window->had_close_request = PRX_FALSE;

	if (!_prx_internal.instance) {
		_prx_internal.instance = GetModuleHandleW(NULL);
	}

	if (!_prx_internal.window_class) {
		WNDCLASSEXW wc = {
			.cbSize = sizeof wc,
			.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
			.lpfnWndProc = window_procedure,
			.cbClsExtra = 0,
			.cbWndExtra = 0,
			.hInstance = _prx_internal.instance,
			.hIcon = LoadIconW(_prx_internal.instance, IDI_APPLICATION),
			.hCursor = LoadCursorW(NULL, IDC_ARROW),
			.hbrBackground = (HBRUSH)(COLOR_ACTIVECAPTION + 1),
			.lpszMenuName = NULL,
			.lpszClassName = L"PyrexWindowClass",
			.hIconSm = NULL,
		};

		_prx_internal.window_class = RegisterClassExW(&wc);

		if (!_prx_internal.window_class) {
			fprintf(stderr,
					"[ERROR] RegisterClassEx failed: %lu\n",
					GetLastError());
			exit(EXIT_FAILURE);
		}
	}

	if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE)) {
		fprintf(stderr,
				"[ERROR] SetProcessDpiAwarenessContext failed: %ld\n",
				GetLastError());
		exit(EXIT_FAILURE);
	}

	DWORD ext_style = 0;
	DWORD style = WS_OVERLAPPEDWINDOW;

	wchar_t *wide_title = _wide_string_from_utf8(title);

	window->handle = CreateWindowExW(ext_style,
									 MAKEINTATOM(_prx_internal.window_class),
									 wide_title,
									 style,
									 CW_USEDEFAULT,
									 CW_USEDEFAULT,
									 0,
									 0,
									 NULL,
									 NULL,
									 _prx_internal.instance,
									 (LPVOID)window);

	if (!window->handle) {
		fprintf(stderr, "[ERROR] CreateWindowExFailed: %lu\n", GetLastError());
		exit(EXIT_FAILURE);
	}

	free(wide_title);

	float dpi = GetDpiForWindow(window->handle);

	SetWindowPos(window->handle,
				 NULL,
				 0,
				 0,
				 (int)ceilf(width * dpi / USER_DEFAULT_SCREEN_DPI),
				 (int)ceilf(height * dpi / USER_DEFAULT_SCREEN_DPI),
				 SWP_NOMOVE);

	ShowWindow(window->handle, SW_SHOW);

	return (PrxWindow *)window;
}

PRXAPI void prx_window_destroy(PrxWindow **handle)
{
	struct _PrxWindow **window = (struct _PrxWindow **)handle;

	DestroyWindow((*window)->handle);

	if (!UnregisterClassW(MAKEINTATOM(_prx_internal.window_class), NULL)) {
		fprintf(stderr,
				"[ERROR] UnregisterClassW failed: %lu\n",
				GetLastError());
		exit(EXIT_FAILURE);
	}

	free(*window);
	window = NULL;	
}

PRXAPI void prx_window_set_user_data(PrxWindow *window, void *data)
{
	struct _PrxWindow *win = (struct _PrxWindow *)window;
	win->user_data = data;
}

PRXAPI void *prx_window_get_user_data(PrxWindow *window)
{
	struct _PrxWindow *win = (struct _PrxWindow *)window;
	return win->user_data;
}

PRXAPI bool prx_window_had_close_request(PrxWindow *handle) {
	struct _PrxWindow *window = (struct _PrxWindow *)handle;
	return window->had_close_request;
}

static LRESULT window_procedure(HWND window_handle,
								UINT message,
								WPARAM wparam,
								LPARAM lparam)
{
	struct _PrxWindow *window = NULL;

	if (message == WM_CREATE) {
		LPCREATESTRUCT create_struct = (LPCREATESTRUCT)lparam;
		window = (struct _PrxWindow *)create_struct->lpCreateParams;
		SetWindowLongPtrW(window_handle, GWLP_USERDATA, (LONG_PTR)window);
		return 0;
	} else {
		LONG_PTR user_data = GetWindowLongPtrW(window_handle, GWLP_USERDATA);
		window = (struct _PrxWindow *)user_data;
	}

	switch (message) {
	// 	case WM_SIZE: {
	// 		unsigned int width = LOWORD(lparam);
	// 		unsigned int height = HIWORD(lparam);
	// 	}
	// 	break;			
	// case WM_DISPLAYCHANGE:
	// 	break;
	// case WM_PAINT:
	// 	break;
	// case WM_KEYDOWN:
	// 	break;
	// case WM_KEYUP:
	// 	break;
	case WM_CLOSE:
		window->had_close_request = PRX_TRUE;
		break;
	case WM_DESTROY:
		PostQuitMessage(EXIT_SUCCESS);
		break;
	default:
		return DefWindowProcW(window_handle, message, wparam, lparam);
	}

	return 0;
}

