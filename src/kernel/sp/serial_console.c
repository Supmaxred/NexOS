#include <stdarg.h>
#include <stdlib.h>

#include "serialport.h"

void sp_putc(char c)
{
    write_serial(c);

    if(c == '\n')
        write_serial('\r');
}

void sp_puts(const char* str)
{  
    while(*str != '\0')
        sp_putc(*str++);
}

void sp_printf(const char* format, ...)
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
                sp_puts(buf);
            }   break;

            case 'u':
            {
                unsigned int value = va_arg(args, unsigned int);
                char buf[12];
                itou(value, buf);
                sp_puts(buf);
            }   break;

            case 'x':
            {
                unsigned int value = va_arg(args, unsigned int);
                char buf[9];
                itox(value, buf);
                sp_puts(buf);
            }   break;

            case 'b':
            {
                unsigned int value = va_arg(args, unsigned int);
                char buf[33];
                itob(value, buf);
                sp_puts(buf);
            }   break;

            case 'c':
            {
                char value = va_arg(args, int);
                sp_putc(value);
            }   break;
            
            case 's':
            {
                const char* value = va_arg(args, char*);
                sp_puts(value);
            }   break;

            case '%':
            {
                sp_putc('%');
            }   break;

            default:
                break;
            }
        }
        else
        {
            sp_putc(*format);
        }

        format++;
    }
    va_end(args);
}