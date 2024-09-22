#include <ctype.h>

void itos(int num, char* str)
{
    int i = 0;
    int isNegative = 0;
    
    if (num < 0) {
        isNegative = 1;
        num = -num;
    }
    
    do {
        str[i++] = num % 10 + '0';
        num /= 10;
    } while (num != 0);
    
    if (isNegative)
        str[i++] = '-';
    
    str[i] = '\0';
    
    int j = 0;
    char temp;
    for (j = 0; j < i / 2; j++) {
        temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

void itox(unsigned int value, char* buf)
{
    static char hex_digits[] = "0123456789ABCDEF";
    for (int i = 7; i >= 0; --i)
    {
        buf[i] = hex_digits[value & 0xF];
        value >>= 4;
    }
    buf[8] = '\0';
}

int stoi(char* str) {
    int result = 0;
    int sign = 1;
    
    if (*str == '-') {
        sign = -1;
        str++;
    }
    
    while (*str) {
        if (*str >= '0' && *str <= '9')
            result = result * 10 + (*str - '0');
        else 
            return 0;
        str++;
    }
    
    return result *= sign;
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