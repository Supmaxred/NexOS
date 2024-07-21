#include "video.h"
#include "ke.h"
#include "res.h"
#include "serialport.h"

uint32_t* fb_addr;
uint32_t fb_pitch;
uint32_t fb_width;
uint32_t fb_height;
uint8_t fb_bpp;

void fb_init()
{
    fb_addr = (uint32_t*)(uint32_t)mb->framebuffer_addr;
    fb_pitch = mb->framebuffer_pitch;
    fb_width = mb->framebuffer_width;
    fb_height = mb->framebuffer_height;
    fb_bpp = mb->framebuffer_bpp;

    sp_printf("fb_addr - %x\n", fb_addr);
    sp_printf("fb_pitch - %i\n", fb_pitch);
    sp_printf("fb_width - %i\n", fb_width);
    sp_printf("fb_height - %i\n", fb_height);
    sp_printf("fb_bpp - %i\n", fb_bpp);
    sp_printf("fb_type - %i\n\n", mb->framebuffer_type);
}

void fb_drawrect(const rect_t* rect, color_t col)
{
    uint16_t x1 = min(rect->x1, rect->x2);
    uint16_t y1 = min(rect->y1, rect->y2);
    uint16_t x2 = max(rect->x1, rect->x2);
    uint16_t y2 = max(rect->y1, rect->y2);

    for (uint16_t y = y1; y < y2; y++) {
        for (uint16_t x = x1; x < x2; x++) {
            fb_setpixel(x, y, col);
        }
    }
}

void fb_invertrect(const rect_t* rect)
{
    uint16_t x1 = min(rect->x1, rect->x2);
    uint16_t y1 = min(rect->y1, rect->y2);
    uint16_t x2 = max(rect->x1, rect->x2);
    uint16_t y2 = max(rect->y1, rect->y2);

    for (uint16_t y = y1; y < y2; y++) {
        for (uint16_t x = x1; x < x2; x++) {
            uint32_t col = fb_getpixel(x, y);
            fb_setpixel(x, y, RGB0(255 - R_PART(col), 255 - G_PART(col), 255 - B_PART(col)));
        }
    }
}

void fb_bitmapblt(
    uint16_t x, 
    uint16_t y,
    uint8_t h,
    uint8_t* bitpattern,
    uint32_t fore_color,
    uint32_t back_color
    )
{
    uint16_t xx;

    uint16_t yy = y;
    for (uint32_t j = 0; j < h; j++) {

        xx = x;
        for (uint32_t i = 128; i > 0; i >>= 1)
        {

            if (i & *bitpattern)
                fb_setpixel(xx, yy, fore_color);
            else 
                fb_setpixel(xx, yy, back_color);

            xx++;
        }

        bitpattern++;
        yy++;
    }
}

void fb_alphablt(
    uint16_t dest_x, 
    uint16_t dest_y,
    uint16_t width, 
    uint16_t height, 
    const uint32_t* src_data, 
    uint8_t alpha
) {
    if (alpha == 0) {
        return;
    }

    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            uint32_t src_pixel = src_data[y * width + x];
            
            uint8_t blended_a = (A_PART(src_pixel) * alpha) / 255;

            uint32_t dest_pixel = fb_getpixel(dest_x + x, dest_y + y);
            
            uint8_t out_r = (R_PART(src_pixel) * blended_a + R_PART(dest_pixel) * (255 - blended_a)) / 255;
            uint8_t out_g = (G_PART(src_pixel) * blended_a + G_PART(dest_pixel) * (255 - blended_a)) / 255;
            uint8_t out_b = (B_PART(src_pixel) * blended_a + B_PART(dest_pixel) * (255 - blended_a)) / 255;
            uint8_t out_a = blended_a + A_PART(dest_pixel) * (255 - blended_a) / 255;
            
            fb_setpixel(dest_x + x, dest_y + y, RGBA(out_r, out_g, out_b, out_a));
        }
    }
}


void fb_drawchar(
    char c,
    uint16_t x, 
    uint16_t y,
    uint32_t fore_color,
    uint32_t back_color
    )
{
    fb_bitmapblt(x, y, 13, &FontData[(uint32_t)c * 13], fore_color, back_color);
}