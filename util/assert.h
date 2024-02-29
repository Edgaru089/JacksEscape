#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


inline static void __panicf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = vfprintf(stderr, fmt, args);
	va_end(args);
	abort();
}


#define ASSERT(expr)                                                                                          \
	do {                                                                                                      \
		if (!(expr))                                                                                          \
			__panicf("Assertion failed: " __FILE__ ":%d[%s]\n    Expression: %s", __LINE__, __func__, #expr); \
	} while (0)
