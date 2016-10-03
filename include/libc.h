/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>

int write(int fd, char *buffer, int size);

void itoa(int a, char *b);

void perror();

int strlen(char *a);

int gettime();

int getpid();

int fork();

void exit();

extern int errno;

#endif  /* __LIBC_H__ */
