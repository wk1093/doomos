#include "string.h"
#include <stdarg.h>
#include <stdio.h>

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;
    while(n--) *d++ = *s++;
    return dst;
}

void *memset(void *dst, int value, size_t n) {
    uint8_t *d = dst;
    while(n--) *d++ = (uint8_t)value;
    return dst;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *a = s1, *b = s2;
    while(n--) {
        if(*a != *b) return (*a < *b) ? -1 : 1;
        a++; b++;
    }
    return 0;
}

size_t strlen(const char *s) {
    size_t len = 0;
    while(*s++) len++;
    return len;
}

int atoi(const char *str) {
    int result = 0;
    int sign = 1;

    // Skip whitespace
    while(*str == ' ' || *str == '\t') str++;

    // Handle optional sign
    if(*str == '-') {
        sign = -1;
        str++;
    } else if(*str == '+') {
        str++;
    }

    // Convert digits to integer
    while(*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result * sign;
}

void strcpy(char *dst, const char *src) {
    while((*dst++ = *src++));
    *dst = '\0';
}
// kernel is at 0x00100000, so this assumes the kernel is less than 16MB in size (which it is 13MB right now)
// We are also in 32 bit, so we need to make sure we don't exceed 4GB (0xFFFFFFFF)
#define MEMORY_LOCATION 0x1000000

size_t current_pos = 0;

void* malloc(size_t size) {
    if (current_pos + size > 0xFFFFFFFF - MEMORY_LOCATION) {
        printf("malloc: Out of memory!\n");
        for(;;);
        // Out of memory
        return NULL;
    }
    void *ptr = (void *)(MEMORY_LOCATION + current_pos);
    current_pos += size;
    return ptr;
}

void fprintf(int fd, const char *fmt, ...) {
    // Dummy implementation: In a real kernel, this would output to a file descriptor
    va_list args;
    va_start(args, fmt);
    // For simplicity, we won't implement actual output here
    va_end(args);
}

char toupper(char c) {
    if(c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');
    }
    return c;
}

char tolower(char c) {
    if(c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }
    return c;
}

int isalpha(int c) {
    return ( (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') );
}

int isdigit(int c) {
    return (c >= '0' && c <= '9');
}

int isspace(int c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v');
}

void strcat(char *dst, const char *src) {
    while(*dst) dst++;
    while((*dst++ = *src++));
}

int strcmp(const char *s1, const char *s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

int strcasecmp(const char *s1, const char *s2) {
    while(*s1 && *s2) {
        char c1 = tolower(*s1);
        char c2 = tolower(*s2);
        if(c1 != c2) {
            return (unsigned char)c1 - (unsigned char)c2;
        }
        s1++;
        s2++;
    }
    return (unsigned char)tolower(*s1) - (unsigned char)tolower(*s2);
}

void strncpy(char *dst, const char *src, size_t n) {
    while(n && (*dst++ = *src++)) n--;
    while(n--) *dst++ = '\0';
}

void* alloca(size_t size) {
    // I don't care that it wants stack
    return malloc(size);
}

int strncasecmp(const char *s1, const char *s2, size_t n) {
    while(n && *s1 && *s2) {
        char c1 = tolower(*s1);
        char c2 = tolower(*s2);
        if(c1 != c2) {
            return (unsigned char)c1 - (unsigned char)c2;
        }
        s1++;
        s2++;
        n--;
    }
    if(n == 0) return 0;
    return (unsigned char)tolower(*s1) - (unsigned char)tolower(*s2);
}

void* realloc(void *ptr, size_t size) {
    // Simple implementation: allocate new block and copy old data
    void *new_ptr = malloc(size);
    if(ptr) {
        // In a real implementation, we would need to know the old size
        // Here we just assume the old size is less than or equal to new size
        memcpy(new_ptr, ptr, size);
    }
    return new_ptr;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while(n && *s1 && *s2) {
        if(*s1 != *s2) {
            return (unsigned char)*s1 - (unsigned char)*s2;
        }
        s1++;
        s2++;
        n--;
    }
    if(n == 0) return 0;
    return (unsigned char)*s1 - (unsigned char)*s2;
}
