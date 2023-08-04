#ifndef PYREX_H_INCLUDED
#define PYREX_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#if !defined (_WIN32) && (defined (__WIN32__) || defined (WIN32) || defined (MINGW32))
#	define _WIN32
#endif // _WIN32

#if defined (_WIN32) && defined (PRX_BUILD_DLL)
#	define PRXAPI __declspec(dllexport)
#elif defined (_WIN32) && defined (PRX_USE_DLL)
#	define PRXAPI __declspec(dllimport)
#else
#	define PRXAPI
#endif // PRXAPI

// PYREX PUBLIC MACRO CONSTANTS
//
#define PRX_TRUE 1
#define PRX_FALSE 0

// PYREX PUBLIC TYPES
//
typedef struct PrxWindow PrxWindow;

#include <stdbool.h>

// PYREX PUBLIC DECLARATIONS
//
PRXAPI PrxWindow *prx_window_create(unsigned int width,
									unsigned int height,
									const char *title);

PRXAPI void prx_window_destroy(PrxWindow **window);

PRXAPI void prx_window_set_user_data(PrxWindow *window, void *data);
PRXAPI void *prx_window_get_user_data(PrxWindow *window);

PRXAPI bool prx_window_had_close_request(PrxWindow *window);

PRXAPI void prx_process_events(void);

#ifdef __cplusplus
}
#endif

#endif // PYREX_H_INCLUDED

