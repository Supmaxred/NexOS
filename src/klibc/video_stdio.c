#include <stdarg.h>
#include <stdlib.h>

#include "video.h"

void putc(char c)
{
    fb_putc(c);
}

void puts(const char* str)
{
    while(*str != '\0')
        fb_putc(*str++);
}

void printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    while(*format != '\0')
    {
        if(*format == '%')
        {
            format++;

            switch (*format)
            {
            case 'd':
            case 'i':
            {
                int value = va_arg(args, int);
                char buf[12];
                itos(value, buf);
                puts(buf);
            }   break;

            case 'x':
            {
                unsigned int value = va_arg(args, unsigned int);
                char buf[9];
                itox(value, buf);
                puts(buf);
            }   break;

            case 'c':
            {
                char value = va_arg(args, int);
                fb_putc(value);
            }   break;
            
            case 's':
            {
                const char* value = va_arg(args, char*);
                puts(value);
            }   break;

            case '%':
            {
                fb_putc('%');
            }   break;

            default:
                break;
            }
        }
        else
        {
            fb_putc(*format);
        }

        format++;
    }
    va_end(args);
}