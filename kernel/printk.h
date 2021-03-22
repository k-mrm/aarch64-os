#ifndef MONO_LIB_PRINTK_H
#define MONO_LIB_PRINTK_H

int printk(const char *fmt, ...);
void panic(const char *s) __attribute__((noreturn));

#endif
