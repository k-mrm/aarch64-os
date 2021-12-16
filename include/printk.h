#ifndef AARCH64_OS_LIB_PRINTK_H
#define AARCH64_OS_LIB_PRINTK_H

int printk(const char *fmt, ...);
void panic(const char *s, ...) __attribute__((noreturn));

void printk_init(void);

#endif
