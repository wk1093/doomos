#include "stdio.h"
#include <stdarg.h>

#include <framebuffer.h>

void sprintf(char *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char *p = buf;
    const char *f = fmt;

    while(*f) {
        if(*f == '%') {
            f++;
            if(*f == 'd' || *f == 'i') {
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
            } else if (*f == 'c') {
                char cval = (char)va_arg(args, int);
                *p++ = cval;
            } else if (*f == '%') {
                *p++ = '%';
            } else if (*f == 'x') {
                unsigned int val = va_arg(args, unsigned int);
                char numbuf[9];
                char *nump = numbuf + sizeof(numbuf) - 1;
                *nump = '\0';
                do {
                    unsigned int digit = val & 0xF;
                    *(--nump) = (digit < 10) ? ('0' + digit) : ('a' + (digit - 10));
                    val >>= 4;
                } while(val);
                while(*nump) {
                    *p++ = *nump++;
                }
            } else if (*f == 'u') {
                unsigned int val = va_arg(args, unsigned int);
                char numbuf[11];
                char *nump = numbuf + sizeof(numbuf) - 1;
                *nump = '\0';
                do {
                    *(--nump) = '0' + (val % 10);
                    val /= 10;
                } while(val);
                while(*nump) {
                    *p++ = *nump++;
                }
            } else if (*f == 'p') {
                uintptr_t val = (uintptr_t)va_arg(args, void *);
                char numbuf[19];
                char *nump = numbuf + sizeof(numbuf) - 1;
                *nump = '\0';
                do {
                    unsigned int digit = val & 0xF;
                    *(--nump) = (digit < 10) ? ('0' + digit) : ('a' + (digit - 10));
                    val >>= 4;
                } while(val);
                *(--nump) = 'x';
                *(--nump) = '0';
                while(*nump) {
                    *p++ = *nump++;
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

static int line_pos = 0;
static int col_pos = 0;

static const int col_offset = 640; // offset from left edge

void printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    const char *f = fmt;
    char buffer[1024];
    char *p = buffer;
    while(*f) {
        if(*f == '%') {
            f++;
            if(*f == 'd' || *f == 'i') {
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
            } else if (*f == 'c') {
                char cval = (char)va_arg(args, int);
                *p++ = cval;
            } else if (*f == '%') {
                *p++ = '%';
            } else if (*f == 'x') {
                unsigned int val = va_arg(args, unsigned int);
                char numbuf[9];
                char *nump = numbuf + sizeof(numbuf) - 1;
                *nump = '\0';
                do {
                    unsigned int digit = val & 0xF;
                    *(--nump) = (digit < 10) ? ('0' + digit) : ('a' + (digit - 10));
                    val >>= 4;
                } while(val);
                while(*nump) {
                    *p++ = *nump++;
                }
            } else if (*f == 'u') {
                unsigned int val = va_arg(args, unsigned int);
                char numbuf[11];
                char *nump = numbuf + sizeof(numbuf) - 1;
                *nump = '\0';
                do {
                    *(--nump) = '0' + (val % 10);
                    val /= 10;
                } while(val);
                while(*nump) {
                    *p++ = *nump++;
                }
            } else if (*f == 'p') {
                uintptr_t val = (uintptr_t)va_arg(args, void *);
                char numbuf[19];
                char *nump = numbuf + sizeof(numbuf) - 1;
                *nump = '\0';
                do {
                    unsigned int digit = val & 0xF;
                    *(--nump) = (digit < 10) ? ('0' + digit) : ('a' + (digit - 10));
                    val >>= 4;
                } while(val);
                *(--nump) = 'x';
                *(--nump) = '0';
                while(*nump) {
                    *p++ = *nump++;
                }
            }
            f++;
        } else {
            *p++ = *f++;
        }
    }
    *p = '\0';

    // fb_draw_string(10, line_pos*10, buffer, 0x00FFFFFF);
    // line_pos += 1;

    // if (line_pos * 10 >= FB_HEIGHT) {
    //     line_pos = 0;
    //     fb_clear(0x00000000);
    // }

    // implement \n handling
    char *bp = buffer;
    while (*bp) {
        if (*bp == '\n') {
            line_pos += 1;
            col_pos = 0;
        } else if (*bp == '\r') {
            col_pos = 0;
        } else {
            fb_draw_char(col_offset + col_pos * 8, line_pos * 10, *bp, 0x00FFFFFF);
            col_pos += 1;
        }
        bp++;
        if (col_pos * 8 + col_offset >= FB_WIDTH) {
            col_pos = 0;
            line_pos += 1;
        }
        if (line_pos * 10 >= FB_HEIGHT) {
            line_pos = 0;
            col_pos = 0;
            fb_clear(0x00000000);
        }
    }
    va_end(args);
    fb_swap();
}

void vprintf(const char *fmt, va_list args) {
    char buffer[1024];
    char *p = buffer;
    const char *f = fmt;

    while(*f) {
        if(*f == '%') {
            f++;
            if(*f == 'd' || *f == 'i') {
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
            } // Add other format specifiers as needed
            f++;
        } else {
            *p++ = *f++;
        }
    }
    *p = '\0';

    // For simplicity, just call printf here
    printf("%s", buffer);
}