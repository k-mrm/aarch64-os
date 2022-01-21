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

u64 strlen(const char *s);
int strcmp(const char *s1, const char *s2);
char *strchr(const char *s, int c);
char *strtok(char *s1, const char *s2);
char *strcpy(char *dst, const char *src);
int printf(const char *fmt, ...);
int puts(char *s);
void *memset(void *dst, int c, u64 n);
void *memcpy(void *dst, const void *src, u64 n);
void *memmove(void *dst, const void *src, u64 n);
void *malloc(u64 nbytes);
void free(void *ptr);

#endif
