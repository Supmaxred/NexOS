#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "ke.h"
#include "x86.h"

int cmd_echo(int argc, char** argv)
{
    if(argc < 2)
        return -1;

    for (int i = 1; i < argc; i++)
    {
        printf("%s ", argv[i]);
    }
    printf("\n");
}

int cmd_sysinfo(int argc, char** argv)
{
    if(argc > 1)
        return -1;

    printf("PRETTY_NAME:\"NexOS 0.0.1\"\n");
    printf("NAME:\"NexOS\"\n");
    printf("VERSION:\"0.0.1\"\n");
}

int cmd_calc(int argc, char** argv)
{
    if(argc != 4)
        return -1;

    int a = stoi(argv[1]);
    int b = stoi(argv[3]);
    int res = 0;

    switch (argv[2][0])
    {
    case '+':
        res = a + b;
        break;

    case '-':
        res = a - b;
        break;

    case '*':
        res = a * b;
        break;

    case '/':
        res = a / b;
        break;
    
    default:
        printf("Not valid operator!\n");
        return -2;
        break;
    }

    printf("Result:%i\n", res);
}

int cmd_reboot(int argc, char** argv)
{
    uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
    printf("Failed to safe-reboot, trying to fire up CPU.\n");
    idt_setentry(13, (void*)0x0, 0x0, 0x8E);
    idt_setentry(8, (void*)0x0, 0x0, 0x8E);
    idt_setentry(6, (void*)0x0, 0x0, 0x8E);
    idt_setentry(5, (void*)0x0, 0x0, 0x8E);
    idt_setentry(0, (void*)0x0, 0x0, 0x8E);
    void (*rf)();
    rf = (void (*)())0x0;
    rf();
}

int cmd_sleep(int argc, char** argv)
{
    if(argc > 2)
        return -1;

    uint32_t ms = 1000;
    if(argc > 1)
        ms = stoi(argv[1]);
    
    sleep(ms);
}

int cmd_uptime(int argc, char** argv)
{
    int full = 0;
    for (int i = 0; i < argc; i++)
    {
        if(argv[i][0] == '-' && argv[i][1] != '\0')
        {
            switch (argv[i][1])
            {
            case 'f':
                full = 1;
                break;
            
            case 'h':
                printf("Usage:\n\tuptime [options]\n\nOptions:\n\t-h\tshow full uptime with milliseconds");
                break;
            
            default:
                printf("Unknown argument %c\n", argv[i][1]);
                return -1;
                break;
            }
        }
    }

    uint32_t ms = ke_clock;

    uint32_t s = ms / 1000;
    ms = ms % 1000;
    
    uint32_t m = s / 60;
    s = s % 60;

    uint32_t h = m / 60;
    m = m % 60;

    printf("%i:%i:%i", h, m, s);
    if(full)
        printf(":%i", ms);
    
    printf("\n");
}

int cmd_date(int argc, char** argv)
{
    time_t curtim = time(NULL);
    struct tm* curdat = localtime(&curtim);
    printf("%s", asctime(curdat));
}

int cmd_test(int argc, char** argv)
{
    int val = stoi(argv[1]);
    int arg = stoi(argv[2]);

    switch(val)
    {
        case 0:
        {
            for (uint32_t i = 0; i < arg; i++)
            {
                putch('\n');
            }
            
        }   break;
    }

    return 0;
}