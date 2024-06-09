#include <stdarg.h>
#include <stdlib.h>

#include "serialport.h"

void dbg_putc(char c)
{
    write_serial(c);

    if(c == '\n')
        write_serial('\r');
}

void dbg_puts(const char* str)
{  
    while(*str != '\0')
        dbg_putc(*str++);
}

void dbg_printf(const char* format, ...)
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
                char buf[11];
                itos(value, buf);
                dbg_puts(buf);
            }   break;

            case 'x':
            {
                unsigned int value = va_arg(args, unsigned int);
                char buf[9];
                itox(value, buf);
                dbg_puts(buf);
            }   break;

            case 'c':
            {
                char value = va_arg(args, int);
                dbg_putc(value);
            }   break;
            
            case 's':
            {
                const char* value = va_arg(args, char*);
                dbg_puts(value);
            }   break;

            case '%':
            {
                dbg_putc('%');
            }   break;

            default:
                break;
            }
        }
        else
        {
            dbg_putc(*format);
        }

        format++;
    }
    va_end(args);
}