#include "internal.h"

#include <windowsx.h>
#include <math.h>

struct PrxWindow {
	struct PrxWindowConfig config;

	int width, height;
	int min_width, min_height;
	int max_width, max_height;
	bool closed;

	void *user_data;
	PrxWindow *next;
	HWND handle;

	struct {
		PrxWindowDestroyedCb on_window_destroyed;
		PrxWindowMovedCb on_window_moved;
		PrxWindowResizedCb on_window_resized;
		PrxWindowClosedCb on_window_closed;
		PrxMouseMovedCb on_mouse_moved;
	} callbacks;
};

static LRESULT window_procedure(
	HWND window_handle,
	UINT message,
	WPARAM wparam,
	LPARAM lparam
);

static void window_print_win32_style(DWORD style);
static void window_print_win32_ext_style(DWORD ext_style);
static DWORD prx_window_get_win32_style(struct PrxWindowConfig *config);

// Creation/destruction
//
PrxWindow *prx_window_create(
	int width,
	int height,
	const char *title)
{
	PrxWindow *window = calloc(1, sizeof *window);

	if (!window) {
		fprintf(stderr, "[ERROR] malloc for window failed\n");
		exit(EXIT_FAILURE);
	}

	window->width = width;
	window->height = height;
	window->min_width = PRX_UNSPECIFIED;
	window->min_height = PRX_UNSPECIFIED;
	window->max_width = PRX_UNSPECIFIED;
	window->max_height = PRX_UNSPECIFIED;

	window->config = window_init_config;

	// head_window is the last created window
	window->next = state.head_window;
	state.head_window = window;

	if (!state.instance) {
		state.instance = GetModuleHandleW(NULL);
	}

	if (!state.window_class) {
		WNDCLASSEXW wc = {
			.cbSize = sizeof wc,
			.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
			.lpfnWndProc = window_procedure,
			.cbClsExtra = 0,
			.cbWndExtra = 0,
			.hInstance = state.instance,
			.hIcon = LoadIconW(state.instance, IDI_APPLICATION),
			.hCursor = LoadCursorW(NULL, IDC_ARROW),
			// .hbrBackground = (HBRUSH)(COLOR_ACTIVECAPTION + 1),
			.hbrBackground = NULL,
			.lpszMenuName = NULL,
			.lpszClassName = L"PyrexWindowClass",
			.hIconSm = NULL,
		};

		state.window_class = RegisterClassExW(&wc);

		if (!state.window_class) {
			fprintf(
				stderr,
				"[WIN32 ERROR] RegisterClassEx failed: %lu\n",
				GetLastError()
			);
			exit(EXIT_FAILURE);
		}
	}

	DWORD style = prx_window_get_win32_style(&window->config);
	DWORD ext_style = 0;

	wchar_t *wide_title = wide_string_from_utf8(title);

	int pos_x = (window_init_config.pos_x == PRX_UNSPECIFIED) ?
		CW_USEDEFAULT :
		window_init_config.pos_x;
	int pos_y = (window_init_config.pos_y == PRX_UNSPECIFIED) ?
		CW_USEDEFAULT :
		window_init_config.pos_y;

	window->handle = CreateWindowExW(
		ext_style,
		MAKEINTATOM(state.window_class),
		wide_title,
		style,
		pos_x,
		pos_y,
		0,
		0,
		NULL,
		NULL,
		state.instance,
		(LPVOID)window
	);

	if (!window->handle) {
		fprintf(
			stderr,
			"[WIN32 ERROR] CreateWindowExFailed: %lu\n",
			GetLastError()
		);
		exit(EXIT_FAILURE);
	}

	free(wide_title);

	DWORD gwl_style = GetWindowLongPtrW(window->handle, GWL_STYLE);
	DWORD gwl_ext_style = GetWindowLongPtrW(window->handle, GWL_EXSTYLE);

	printf(
		"style: %.8x\next style: %.8x\n",
		(unsigned int)gwl_style,
		(unsigned int)gwl_ext_style
	);

	window_print_win32_style(gwl_style);
	window_print_win32_ext_style(gwl_ext_style);

	if (window_init_config.dpi_scaled) {
		unsigned int dpi = GetDpiForWindow(window->handle);
		float scaling_factor = (float)dpi / USER_DEFAULT_SCREEN_DPI;
		
		RECT client_rect = {
			.left = 0,
			.top = 0,
			.right = width * scaling_factor,
			.bottom = height * scaling_factor,
		};
		AdjustWindowRectExForDpi(&client_rect, style, PRX_FALSE, ext_style, dpi);

		int adjusted_width = client_rect.right - client_rect.left;
		int adjusted_height = client_rect.bottom - client_rect.top;

		SetWindowPos(
			window->handle,
			NULL,
			0,
			0,
			adjusted_width,
			adjusted_height,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE
		);
	} else {
		RECT client_rect = {
			.left = 0,
			.top = 0,
			.right = width,
			.bottom = height,
		};
		AdjustWindowRectEx(&client_rect, style, PRX_FALSE, ext_style);

		int adjusted_width = client_rect.right - client_rect.left;
		int adjusted_height = client_rect.bottom - client_rect.top;

		SetWindowPos(
			window->handle,
			NULL,
			100,
			100,
			adjusted_width,
			adjusted_height,
			0
		);
	}

	ShowWindow(window->handle, SW_SHOW);

	return (PrxWindow *)window;
}

void prx_window_destroy(PrxWindow **window) {
	DestroyWindow((*window)->handle);

	PrxWindow **prev = &state.head_window;

	while (*prev != *window) {
		prev = &(*prev)->next;
	}

	*prev = (*window)->next;

	free(*window);
	*window = NULL;
}

// Configuration
//
void prx_window_configure(int setting, int value) {
	switch (setting) {
	case PRX_POS_X:
		window_init_config.pos_x = value;
		break;
	case PRX_POS_Y:
		window_init_config.pos_y = value;
		break;
	case PRX_MAXIMISED:
		window_init_config.maximised = value ? PRX_TRUE : PRX_FALSE;
		break;
	case PRX_RESIZABLE:
		window_init_config.resizable = value ? PRX_TRUE : PRX_FALSE;
		break;
	case PRX_VISIBLE:
		window_init_config.visible = value ? PRX_TRUE : PRX_FALSE;
		break;
	case PRX_FOCUSED:
		window_init_config.focused = value ? PRX_TRUE : PRX_FALSE;
		break;
	case PRX_DECORATED:
		window_init_config.decorated = value ? PRX_TRUE : PRX_FALSE;
		break;
	case PRX_DPI_SCALED:
		window_init_config.dpi_scaled = value ? PRX_TRUE : PRX_FALSE;
		break;
	}
}

void prx_window_get_size(PrxWindow *window, int *width, int *height) {
	RECT window_rect = {0};
	GetWindowRect(window->handle, &window_rect);
	*width = window_rect.right - window_rect.left;
	*height = window_rect.bottom - window_rect.top;
}

void prx_window_get_client_size(PrxWindow *window, int *width, int *height) {
	RECT client_rect = {0};
	GetClientRect(window->handle, &client_rect);
	*width = client_rect.right - client_rect.left;
	*height = client_rect.bottom - client_rect.top;
}

HWND prx_window_get_win32_handle(PrxWindow *window) {
	return window->handle;
}

void prx_window_set_user_pointer(PrxWindow *window, void *data) {
	window->user_data = data;
}

void *prx_window_get_user_pointer(PrxWindow *window) {
	return window->user_data;
}

void prx_window_set_title(PrxWindow *window, const char *title) {
	wchar_t *wide_title = wide_string_from_utf8(title);

	if (!SetWindowTextW(window->handle, wide_title)) {
		fprintf(
			stderr,
			"[WIN32 ERROR] SetWindowTextW failed (%ld)\n",
			GetLastError()
		);
		exit(EXIT_FAILURE);
	}

	free(wide_title);
}

char *prx_window_get_title(PrxWindow *window) {
	int title_length = 1 + GetWindowTextLengthW(window->handle);
	char *title = malloc(title_length * sizeof *title);

	if (!GetWindowTextA(window->handle, title, title_length)) {
		fprintf(
			stderr,
			"[WIN32 ERROR] GetWindowTextW failed (%ld)\n",
			GetLastError()
		);
		exit(EXIT_FAILURE);
	}

	return title;
}

void prx_window_set_size_limits(
	PrxWindow *window,
	int min_width, int min_height,
	int max_width, int max_height)
{
	float scale = 1.0f;
	
	if (window->config.dpi_scaled) {
		float dpi = GetDpiForWindow(window->handle);
		scale = dpi / USER_DEFAULT_SCREEN_DPI;
	}

	if (min_width != PRX_UNSPECIFIED) {
		window->min_width = min_width * scale;
	}
	if (min_height != PRX_UNSPECIFIED) {
		window->min_height = min_height * scale; 
	}
	if (max_width != PRX_UNSPECIFIED) {
		window->max_width = max_width * scale; 
	}
	if (max_width != PRX_UNSPECIFIED) {
		window->max_height = max_height * scale; 
	}
}

// Callbacks
//
void prx_window_set_window_destroyed_callback(
	PrxWindow *window,
	PrxWindowDestroyedCb callback)
{
	window->callbacks.on_window_destroyed = callback;
}

void prx_window_set_window_moved_callback(
	PrxWindow *window,
	PrxWindowMovedCb callback)
{
	window->callbacks.on_window_moved = callback;
}

void prx_window_set_window_resized_callback(
	PrxWindow *window,
	PrxWindowResizedCb callback)
{
	window->callbacks.on_window_resized = callback;
}

void prx_window_set_window_closed_callback(
	PrxWindow *window,
	PrxWindowClosedCb callback)
{
	window->callbacks.on_window_closed = callback;
}

void prx_window_set_mouse_moved_callback(
	PrxWindow *window,
	PrxMouseMovedCb callback)
{
	window->callbacks.on_mouse_moved = callback;
}

// Events
//
int prx_window_is_closed(PrxWindow *window) {
	return window->closed;
}

static LRESULT window_procedure(
	HWND handle,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	LRESULT result = 0;
	PrxWindow *window = NULL;

	if (message == WM_NCCREATE) {
		LPCREATESTRUCT create_struct = (LPCREATESTRUCT)lparam;
		window = (PrxWindow *)create_struct->lpCreateParams;
		SetWindowLongPtrW(handle, GWLP_USERDATA, (LONG_PTR)window);
		return DefWindowProcW(handle, message, wparam, lparam);
	} else {
		LONG_PTR user_data = GetWindowLongPtrW(handle, GWLP_USERDATA);
		window = (PrxWindow *)user_data;
	}

	switch (message) {
	case WM_DESTROY:
		{
			if (window->callbacks.on_window_destroyed) {
				window->callbacks.on_window_destroyed(window);
			}

			PostQuitMessage(EXIT_SUCCESS);
		}
		break;
	case WM_MOVE:
		{
			int x = GET_X_LPARAM(lparam);
			int y = GET_Y_LPARAM(lparam);

			if (window->callbacks.on_window_moved) {
				window->callbacks.on_window_moved(window, x, y);
			}
		}
		break;
	case WM_SIZE:
		{
			int width = LOWORD(lparam);
			int height = HIWORD(lparam);

			if (window->callbacks.on_window_resized) {
				window->callbacks.on_window_resized(window,	width, height);
			}
		}
		break;
	case WM_CLOSE:
		{
			if (window->callbacks.on_window_closed) {
				window->callbacks.on_window_closed(window);
			}

			window->closed = PRX_TRUE;
		}
		break;
	case WM_MOUSEMOVE:
		{
			int x = LOWORD(lparam);
			int y = HIWORD(lparam);

			if (window->callbacks.on_mouse_moved) {
				window->callbacks.on_mouse_moved(window, x, y);
			}
		}
		break;
	case WM_GETMINMAXINFO:
		{
			if (!window) {
				return DefWindowProcW(handle, message, wparam, lparam);
			}

			RECT decoration = {0};
			AdjustWindowRectExForDpi(
				&decoration,
				0,
				FALSE,
				WS_OVERLAPPED,
				GetDpiForWindow(handle)
			);

			MINMAXINFO *mmi = (MINMAXINFO *)lparam;

			if (window->min_width != PRX_UNSPECIFIED ||
				window->min_height != PRX_UNSPECIFIED)
			{
				mmi->ptMinTrackSize.x =	window->min_width +
					decoration.right - decoration.left;
				mmi->ptMinTrackSize.y =	window->min_height +
					decoration.bottom - decoration.top;
			}

			if (window->max_width != PRX_UNSPECIFIED ||
				window->max_height != PRX_UNSPECIFIED)
			{
				mmi->ptMaxTrackSize.x =	window->max_width +
					decoration.right - decoration.left;
				mmi->ptMaxTrackSize.y =	window->max_height +
					decoration.bottom - decoration.top;
			}
		}
		break;
	// case WM_DISPLAYCHANGE:
	// 	break;
	// case WM_KEYDOWN:
	// 	break;
	// case WM_KEYUP:
	// 	break;
	default:
		result = DefWindowProcW(handle, message, wparam, lparam);
	}

	return result;
}

static DWORD prx_window_get_win32_style(struct PrxWindowConfig *config) {
	DWORD style =
		WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS |
		WS_SYSMENU 		|
		WS_MINIMIZEBOX;

	if (config->decorated) {
		style |= WS_CAPTION;

		if (config->resizable) {
			style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
		}
	} else {
		style |= WS_POPUP;
	}

	return style;
}

static void window_print_win32_style(DWORD style) {
	static struct {
		char str[16];
		unsigned long bits;
	} styles[24] = {
		[ 0] = {"WS_BORDER" 		, 0x00800000L},
		[ 1] = {"WS_CAPTION" 		, 0x00C00000L},
		[ 2] = {"WS_CHILD" 			, 0x40000000L},
		[ 3] = {"WS_CHILDWINDOW" 	, 0x40000000L},
		[ 4] = {"WS_CLIPCHILDREN" 	, 0x02000000L},
		[ 5] = {"WS_CLIPSIBLINGS" 	, 0x04000000L},
		[ 6] = {"WS_DISABLED" 		, 0x08000000L},
		[ 7] = {"WS_DLGFRAME" 		, 0x00400000L},
		[ 8] = {"WS_GROUP" 			, 0x00020000L},
		[ 9] = {"WS_HSCROLL" 		, 0x00100000L},
		[10] = {"WS_ICONIC" 		, 0x20000000L},
		[11] = {"WS_MAXIMIZE" 		, 0x01000000L},
		[12] = {"WS_MAXIMIZEBOX" 	, 0x00010000L},
		[13] = {"WS_MINIMIZE" 		, 0x20000000L},
		[14] = {"WS_MINIMIZEBOX" 	, 0x00020000L},
		[15] = {"WS_OVERLAPPED" 	, 0x00000000L},
		[16] = {"WS_POPUP" 			, 0x80000000L},
		[17] = {"WS_SIZEBOX" 		, 0x00040000L},
		[18] = {"WS_SYSMENU" 		, 0x00080000L},
		[19] = {"WS_TABSTOP" 		, 0x00010000L},
		[20] = {"WS_THICKFRAME" 	, 0x00040000L},
		[21] = {"WS_TILED" 			, 0x00000000L},
		[22] = {"WS_VISIBLE" 		, 0x10000000L},
		[23] = {"WS_VSCROLL" 		, 0x00200000L},
	};

	char style_str[256];
	char *current_char = style_str;

	for (size_t i = 0; i < _countof(styles); i++) {
		unsigned long mask = styles[i].bits;
		unsigned long common = style & mask;
		if ((common ^ mask) == 0) {
			current_char += sprintf(current_char, "%s ", styles[i].str);
		}
	}

	*(--current_char) = '\0';
	printf("%s\n", style_str);
}

static void window_print_win32_ext_style(DWORD ext_style) {
	static struct {
		char str[32];
		unsigned long bits;
	} ext_styles[25] = {
		[ 0] = {"WS_EX_ACCEPTFILES" 		, 0x00000010L},
		[ 1] = {"WS_EX_APPWINDOW" 			, 0x00040000L},
		[ 2] = {"WS_EX_CLIENTEDGE" 			, 0x00000200L},
		[ 3] = {"WS_EX_COMPOSITED" 			, 0x02000000L},
		[ 4] = {"WS_EX_CONTEXTHELP" 		, 0x00000400L},
		[ 5] = {"WS_EX_CONTROLPARENT" 		, 0x00010000L},
		[ 6] = {"WS_EX_DLGMODALFRAME" 		, 0x00000001L},
		[ 7] = {"WS_EX_LAYERED" 			, 0x00080000L},
		[ 8] = {"WS_EX_LAYOUTRTL" 			, 0x00400000L},
		[ 9] = {"WS_EX_LEFT" 				, 0x00000000L},
		[10] = {"WS_EX_LEFTSCROLLBAR" 		, 0x00004000L},
		[11] = {"WS_EX_LTRREADING" 			, 0x00000000L},
		[12] = {"WS_EX_MDICHILD" 			, 0x00000040L},
		[13] = {"WS_EX_NOACTIVATE" 			, 0x08000000L},
		[14] = {"WS_EX_NOINHERITLAYOUT" 	, 0x00100000L},
		[15] = {"WS_EX_NOPARENTNOTIFY" 		, 0x00000004L},
		[16] = {"WS_EX_NOREDIRECTIONBITMAP" , 0x00200000L},
		[17] = {"WS_EX_RIGHT" 				, 0x00001000L},
		[18] = {"WS_EX_RIGHTSCROLLBAR" 		, 0x00000000L},
		[19] = {"WS_EX_RTLREADING" 			, 0x00002000L},
		[20] = {"WS_EX_STATICEDGE" 			, 0x00020000L},
		[21] = {"WS_EX_TOOLWINDOW" 			, 0x00000080L},
		[22] = {"WS_EX_TOPMOST" 			, 0x00000008L},
		[23] = {"WS_EX_TRANSPARENT" 		, 0x00000020L},
		[24] = {"WS_EX_WINDOWEDGE" 			, 0x00000100L},
	};

	char ext_style_str[256];
	char *current_char = ext_style_str;

	for (size_t i = 0; i < _countof(ext_styles); i++) {
		unsigned long mask = ext_styles[i].bits;
		unsigned long common = ext_style & mask;
		if ((common ^ mask) == 0) {
			current_char += sprintf(current_char, "%s ", ext_styles[i].str);
		}
	}

	*(--current_char) = '\0';
	printf("%s\n", ext_style_str);
}

