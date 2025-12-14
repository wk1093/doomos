#pragma once
#include <stddef.h>
#include <stdint.h>

#define stderr 0
#define stdout 0

void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *dst, int value, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
size_t strlen(const char *s);

void sprintf(char *buf, const char *fmt, ...);

int atoi(const char *str);

void strcpy(char *dst, const char *src);

void* malloc(size_t size);

void printf(const char *fmt, ...);

void fprintf(int fd, const char *fmt, ...);

char toupper(char c);
char tolower(char c);
int isalpha(int c);
int isdigit(int c);
int isspace(int c);

void strcat(char *dst, const char *src);

int strcmp(const char *s1, const char *s2);

int strcasecmp(const char *s1, const char *s2);

#define MININT (-2147483647-1)
#define MAXINT 2147483647

int abs(int x);

void strncpy(char *dst, const char *src, size_t n);

void panic(const char *msg);

void* alloca(size_t size);

int strncasecmp(const char *s1, const char *s2, size_t n);

void* realloc(void *ptr, size_t size);

int strncmp(const char *s1, const char *s2, size_t n);