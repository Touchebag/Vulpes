#pragma once

#include <cstdio>

/*
 * LOG_LEVEL
 * Every level includes all previous levels
 *
 * 0 - Nothing
 * 1 - Error
 * 2 - Warning
 * 3 - Debug
 * 4 - Verbose
 */

// ---- Main log ----

#if LOG_LEVEL > 0
#define LOG(level, ...) fprintf(stderr, level " : " __VA_ARGS__); fprintf(stderr, "\n"); fflush(stderr);
#else
#define LOG(...) ((void)0);
#endif

// ---- Levels ----

#if LOG_LEVEL > 0
#define LOGE(...) LOG("ERROR", __VA_ARGS__)
#else
#define LOGE(...) ((void)0);
#endif

#if LOG_LEVEL > 1
#define LOGW(...) LOG("WARNING", __VA_ARGS__)
#else
#define LOGW(...) ((void)0);
#endif

#if LOG_LEVEL > 2
#define LOGD(...) LOG("DEBUG", __VA_ARGS__)
#else
#define LOGD(...) ((void)0);
#endif

#if LOG_LEVEL > 3
#define LOGV(...) LOG("VERBOSE", __VA_ARGS__)
#else
#define LOGV(...) ((void)0);
#endif

