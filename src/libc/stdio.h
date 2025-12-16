#include <stdarg.h>

void printf(const char *fmt, ...);

void fprintf(int fd, const char *fmt, ...);

void vprintf(const char *fmt, va_list args);

void printf_dbg(int x, int y, const char *fmt, ...);