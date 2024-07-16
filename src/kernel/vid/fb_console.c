#include <stdint.h>
#include <string.h>

#include "video.h"

#define FONTHEIGHT 13
#define TEXTHEIGHT FONTHEIGHT + 1
#define FONTWIDTH 8
#define TABSPACES 4

#define TOGGLECUR() {fb_invertrect(&(rect_t){vcurx, vcury, vcurx + 1, vcury + 12});\
vcurvisible = !vcurvisible;}

int16_t curx = 0;
int16_t cury = 0;
uint8_t curenable = 0;

int16_t vcurx;
int16_t vcury;

uint8_t vcurvisible = 0;
uint8_t is_scrolling = 0;

color_t backcolor = 0;
color_t forecolor = RGB0(255, 255, 255);

extern uint32_t fb_width;
extern uint32_t fb_height;

static void _fb_scroll()
{
    uint32_t row_size = fb_width * sizeof(uint32_t);

    uint32_t* src = fb_pixeladdr(0, 14);
    uint32_t* dest = fb_pixeladdr(0, 0);

    uint32_t copy_lines = fb_height - 14;

    is_scrolling = 1
    memmove(dest, src, copy_lines * row_size);

    uint32_t* clear_start = fb_pixeladdr(0, fb_height - 14);
    memset(clear_start, 0, 14 * row_size);
    is_scrolling = 0;

    //for (uint16_t y = 0; y < fb_height; y++)
    //{
    //    for (uint16_t x = 0; x < fb_width; x++)
    //    {
    //        fb_setpixel(x, y, fb_getpixel(x, y + 14));
    //    }
    //}
}

static void _fb_linefeed()
{
    cury += TEXTHEIGHT;

    if(cury >= fb_height)
    {
        cury -= TEXTHEIGHT;
        vcury -= TEXTHEIGHT;
        _fb_scroll();
    }
}

static void _fb_curwalk()
{
    curx += FONTWIDTH;

    if(curx >= fb_width)
    {
        curx = 0;
        _fb_linefeed();
    }
}

static void _fb_curup()
{
    cury -= TEXTHEIGHT;

    if(cury < 0)
        cury = 0;
}

static void _fb_curback()
{
    curx -= FONTWIDTH;

    if(curx < 0)
    {
        _fb_curup();
        curx = fb_width - fb_width % FONTWIDTH - FONTWIDTH;
    }
}

static void _fb_hortab()
{
    curx = curx - curx % TABSPACES + TABSPACES; //Round to tab spaces

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

void fb_togglecursor()
{
    if (curenable && !is_scrolling)
    {
        TOGGLECUR();
    }
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