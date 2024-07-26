#include <stdarg.h>
#include <stdlib.h>

#include "video.h"

void putch(char c)
{
    fb_putc(c);
}

void puts(const char* str)
{
    while(*str != '\0')
        fb_putc(*str++);
}

void itou(unsigned int value, char* buf)
{
    int i = 0;
    if (value == 0)
    {
        buf[i++] = '0';
    }
    else
    {
        while (value > 0)
        {
            buf[i++] = (value % 10) + '0';
            value /= 10;
        }
    }
    buf[i] = '\0';

    for (int j = 0; j < i / 2; j++)
    {
        char temp = buf[j];
        buf[j] = buf[i - j - 1];
        buf[i - j - 1] = temp;
    }
}

void itob(unsigned int value, char* buf)
{
    int len = 32;

    for (int i = len - 1; i >= 0; i--)
    {
        buf[i] = (value & 1) + '0';
        value >>= 1;
    }
    buf[len] = '\0';
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

            case 'u':
            {
                unsigned int value = va_arg(args, unsigned int);
                char buf[12];
                itou(value, buf);
                puts(buf);
            }   break;

            case 'x':
            {
                unsigned int value = va_arg(args, unsigned int);
                char buf[9];
                itox(value, buf);
                puts(buf);
            }   break;

            case 'b':
            {
                unsigned int value = va_arg(args, unsigned int);
                char buf[33];
                itob(value, buf);
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