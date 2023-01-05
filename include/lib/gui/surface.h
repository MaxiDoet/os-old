#ifndef _gui_surface_h
#define _gui_surface_h

typedef struct surface_t {
    uint16_t width;
    uint16_t height;
    uint16_t bpp;

    void *fb;
} surface_t;

#endif