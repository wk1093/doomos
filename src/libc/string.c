#include "string.h"

#include <stdarg.h>

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

void sprintf(char *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char *p = buf;
    const char *f = fmt;

    while(*f) {
        if(*f == '%') {
            f++;
            if(*f == 'd') {
                int val = va_arg(args, int);
                char numbuf[12];
                char *nump = numbuf + sizeof(numbuf) - 1;
                *nump = '\0';
                int is_negative = 0;
                if(val < 0) {
                    is_negative = 1;
                    val = -val;
                }
                do {
                    *(--nump) = '0' + (val % 10);
                    val /= 10;
                } while(val);
                if(is_negative) {
                    *(--nump) = '-';
                }
                while(*nump) {
                    *p++ = *nump++;
                }
            } else if(*f == 's') {
                const char *sval = va_arg(args, const char *);
                while(*sval) {
                    *p++ = *sval++;
                }
            }
            f++;
        } else {
            *p++ = *f++;
        }
    }
    *p = '\0';
    va_end(args);
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

#define MEMORY_LOCATION 0x1000000

size_t current_pos = 0;

void* malloc(size_t size) {
    void *ptr = (void *)(MEMORY_LOCATION + current_pos);
    current_pos += size;
    return ptr;
}

void printf(const char *fmt, ...) {
    // Dummy implementation: In a real kernel, this would output to console or serial port
    va_list args;
    va_start(args, fmt);
    // For simplicity, we won't implement actual output here
    va_end(args);
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

int abs(int x) {
    return (x < 0) ? -x : x;
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
