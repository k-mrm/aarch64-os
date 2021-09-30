#ifndef USER_ULIB_H
#define USER_ULIB_H

typedef unsigned long u64;
typedef signed long i64;
typedef unsigned int u32;
typedef signed int i32;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned char u8;
typedef signed char i8;

#define NULL ((void *)0)

typedef _Bool bool;

#define true 1
#define false 0

u64 strlen(char *s);
int ustrcmp(const char *s1, const char *s2);
int printf(const char *fmt, ...);

#endif
