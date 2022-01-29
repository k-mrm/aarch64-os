#ifndef AARCH64_OS_SYSCALL_H
#define AARCH64_OS_SYSCALL_H

#define NSYSCALL    18

#define SYS_getpid  0
#define SYS_write   1
#define SYS_read    2
#define SYS_exit    3
#define SYS_fork    4
#define SYS_wait    5
#define SYS_exec    6
#define SYS_open    7
#define SYS_close   8
#define SYS_fstat   9
#define SYS_uname   10
#define SYS_chdir   11
#define SYS_mkdir   12
#define SYS_mknod   13
#define SYS_dup     14
#define SYS_clone   15
#define SYS_waitpid 16
#define SYS_sbrk    17

#endif
