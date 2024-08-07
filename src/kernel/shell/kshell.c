#include <stdio.h>
#include <string.h>

#include "res.h"
#include "pckbd.h"
#include "video.h"

#define CMD_MAXLEN 2000

#define CMDDEF(name) int cmd_##name(int argc, char** argv)
#define CMD_ENTRY(name) {#name, cmd_##name}

char cmd_buffer[CMD_MAXLEN + 1]; //Max command length + null terminator
uint16_t cmd_bufferlen = 0;

typedef int (*cmdf_t)(int, char**);

typedef struct cmd
{
    char* name;
    cmdf_t func;
} cmd_t;

CMDDEF(echo);
CMDDEF(sysinfo);
CMDDEF(calc);
CMDDEF(reboot);
CMDDEF(sleep);
CMDDEF(uptime);
CMDDEF(date);
CMDDEF(test);

cmd_t cmds[] = {
    CMD_ENTRY(echo),
    CMD_ENTRY(sysinfo),
    CMD_ENTRY(calc),
    CMD_ENTRY(reboot),
    CMD_ENTRY(sleep),
    CMD_ENTRY(uptime),
    CMD_ENTRY(date),
    CMD_ENTRY(test),
};

void kshell_drawlogo()
{
    fb_alphablt(0, 0, 64, 64, nexlogo, 255);
    puts("\n\n\n\n");
}

void kshell_printsol()
{
    printf("$ ");
}

void kshell_parsecmd(char** _argv, char* _argc)
{
    char** argv = _argv;
    char argc = *_argc;

    char* ptr = cmd_buffer;
    char isPrevDiv = 1;
    while (*ptr)
    {
        if (*ptr == '"')
        {
            ptr++;

            argv[argc] = ptr;
            argc++;

            while (*ptr != '"' && *ptr != 0)
                ptr++;
            
            if (*ptr == '"')
            {
                *ptr = '\0';
                ptr++;
            }
        }
        else
        if (*ptr != ' ' && isPrevDiv)
        {
            argv[argc] = ptr;
            argc++;
        }

        isPrevDiv = *ptr == ' ';

        if (*ptr == ' ' && *(ptr - 1) != ' ')
        {
            *ptr = '\0';
        }

        ptr++;
    }

    *_argc = argc;
}

void kshell_execute()
{
    char *argv[CMD_MAXLEN / 2 + 1];
    char argc = 0;

    kshell_parsecmd(argv, &argc);
    if(argc > 0)
    {
        cmd_t* find = 0;

        uint32_t cmds_len = sizeof(cmds) / sizeof(cmds[0]);
        for (uint32_t i = 0; i < cmds_len; i++)
        {
            if(strequ(argv[0], cmds[i].name))
            {
                find = &cmds[i];
                break;
            }
        }

        if(find)
        {
            find->func(argc, argv);
        }
        else
        {
            printf("Command '%s' not found.\n", argv[0]);
        }
    }
}

void kshell_procceschar(char ch)
{
    if(ch == '\b' && cmd_bufferlen > 0)
    {
        puts("\b \b");
        cmd_buffer[--cmd_bufferlen] = '\0';
        return;
    }

	if(ch == '\n') 
    {
        cmd_buffer[cmd_bufferlen++] = 0;
        putch('\n');
		kshell_execute();
        cmd_bufferlen = 0;
        //pckbd_flush();
        kshell_printsol();
		return;
	}

    if(cmd_bufferlen >= CMD_MAXLEN)
        return;

    if(ch < ' ' || ch > '~')
        return;

    //for (int i = 0; i < 10; i++)
    {
        cmd_buffer[cmd_bufferlen++] = ch;
        putch(ch);
    }
}

void kshell_main()
{
    kshell_drawlogo();
    fb_setcursor(1);
    printf("--Welcome to kshell of NexOS!--\n");
    pckbd_enable();

    int ch = 0;
    kshell_printsol();
    while(1)
    {
        while((ch = pckbd_getch()) == -1);

        kshell_procceschar(ch);
    }
}