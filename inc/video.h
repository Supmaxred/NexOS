#include <stdint.h>

#define RGB0(r, g, b)       ( (uint32_t)(((r) << 16) | ((g) << 8) | (b)) )
#define RGBA(r, g, b, a)    ( (uint32_t)(((r) << 16) | ((g) << 8) | (b) | (a << 24)) )

#define R_PART(c) ((c) >> 16 & 0xff)
#define G_PART(c) ((c) >> 8 & 0xff)
#define B_PART(c) ((c) & 0xff)
#define A_PART(c) ((c) >> 24 & 0xff)

#define PIXELADDR(x, y, w) ((y) * (w) + (x))

typedef uint32_t color_t;

typedef struct rect {
  uint16_t x1;
  uint16_t y1;
  uint16_t x2;
  uint16_t y2;
} rect_t;

extern uint32_t* fb_addr;
extern uint32_t fb_pitch;
extern uint32_t fb_width;
extern uint32_t fb_height;
extern uint8_t fb_bpp;

void fb_init();

static inline uint32_t* fb_pixeladdr(uint32_t x, uint32_t y) 
{
    return fb_addr + PIXELADDR(x, y, fb_width);
}

static inline void fb_setpixel(uint16_t x, uint16_t y, color_t col)
{
    if(x >= fb_width || y >= fb_height)
        return;

    *((uint32_t*)fb_pixeladdr(x, y)) = col;
}

static inline color_t fb_getpixel(uint16_t x, uint16_t y)
{
    if(x >= fb_width || y >= fb_height)
        return 0;

    return *((uint32_t*)fb_pixeladdr(x, y));
}

void fb_drawrect(const rect_t* rect, color_t col);

void fb_invertrect(const rect_t* rect);

void fb_bitmapblt(
    uint16_t x, 
    uint16_t y,
    uint8_t h,
    uint8_t* bitpattern,
    uint32_t fore_color,
    uint32_t back_color
    );

void fb_alphablt(
    uint16_t dest_x, 
    uint16_t dest_y,
    uint16_t width, 
    uint16_t height, 
    const uint32_t* src_data, 
    uint8_t alpha
);

void fb_drawchar(
    char c,
    uint16_t x, 
    uint16_t y,
    uint32_t fore_color,
    uint32_t back_color
    );

//fb_stdio.c
void fb_setcursor(uint8_t vis);

void fb_putc(char c);