#include <stdint.h>

#include "video.h"

#define FONTHEIGHT 13

#define TOGGLECUR() {fb_invertrect(&(rect_t){vcurx, vcury, vcurx + 1, vcury + 12});\
vcurvisible = !vcurvisible;}

uint16_t curx = 0;
uint16_t cury = 0;
uint8_t curenable = 0;

uint16_t vcurx;
uint16_t vcury;

uint8_t vcurvisible = 0;

color_t backcolor = 0;
color_t forecolor = RGB0(255, 255, 255);

extern uint32_t fb_width;
extern uint32_t fb_height;

static void _fb_scroll()
{
    for (uint16_t y = 0; y < fb_height; y++)
    {
        for (uint16_t x = 0; x < fb_width; x++)
        {
            fb_setpixel(x, y, fb_getpixel(x, y + 14));
        }
    }
}

static void _fb_linefeed()
{
    cury += FONTHEIGHT + 1;

    if(cury >= fb_height)
    {
        cury -= FONTHEIGHT + 1;
        vcury = vcury - 14;
        _fb_scroll();
    }
}

static void _fb_curwalk()
{
    curx += 8;

    if(curx >= fb_width)
    {
        _fb_linefeed();
        curx = 0;
    }
}

static void _fb_curback()
{
    curx -= 8;

    if(curx < 0)
    {
        curx = 0;
    }
}

static void _fb_hortab()
{
    curx = curx - curx % 4 + 4;

    if(curx >= fb_width)
    {
        _fb_linefeed();
        curx = 0;
    }
}

static void _fb_updatecur()
{
    if(curx == vcurx && cury == vcury || !curenable)
        return;

    if(vcurvisible)
        TOGGLECUR();

    vcurx = curx;
    vcury = cury;

    TOGGLECUR();
}

void fb_setcursor(uint8_t vis)
{
    if(vis != curenable)
        TOGGLECUR();

    curenable = vis;
    _fb_updatecur();
}

void fb_putc(char c)
{
    switch (c)
    {
        case '\a': {
            
        } break;

        case '\b': {
            _fb_curback();
        } break;

        case '\n': {
            _fb_linefeed();
            curx = 0;
        } break;

        case '\r': {
            curx = 0;
        } break;

        case '\t': {
            _fb_hortab();
        } break;
    
        default: {
            fb_drawchar(c, curx, cury, forecolor, backcolor);
            vcurvisible = 0;
            _fb_curwalk();
        }   break;
    }

    _fb_updatecur();

    return;
}