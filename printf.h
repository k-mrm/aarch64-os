#ifndef MONO_LIB_PRINTF_H
#define MONO_LIB_PRINTF_H

int printf(const char *fmt, ...);
void panic(const char *s) __attribute__((noreturn));

#endif
