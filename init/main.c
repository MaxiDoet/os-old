#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/drivers/vgacon.h"
#include "../include/drivers/vga-color.h"
#include "../include/lib/print.h"
#include "../bin/shell/shell.h"
#include "../include/kernel/gdt.h"
#include "../include/kernel/idt.h"
#include "../include/kernel/multiboot.h"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

void kernel_main(unsigned long addr)
{
	multiboot_info_t *mbi;
	mbi = (multiboot_info_t *) addr;

	if (CHECK_FLAG (mbi->flags, 12))
    	{
      		multiboot_uint32_t color;
      unsigned i;
      void *fb = (void *) (unsigned long) mbi->framebuffer_addr;

      switch (mbi->framebuffer_type)
        {
        case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
          {
            unsigned best_distance, distance;
            struct multiboot_color *palette;
            
            palette = (struct multiboot_color *) mbi->framebuffer_palette_addr;

            color = 0;
            best_distance = 4*256*256;
            
            for (i = 0; i < mbi->framebuffer_palette_num_colors; i++)
              {
                distance = (0xff - palette[i].blue) * (0xff - palette[i].blue)
                  + palette[i].red * palette[i].red
                  + palette[i].green * palette[i].green;
                if (distance < best_distance)
                  {
                    color = i;
                    best_distance = distance;
                  }
              }
          }
          break;

        case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
          color = ((1 << mbi->framebuffer_blue_mask_size) - 1) 
            << mbi->framebuffer_blue_field_position;
          break;

        case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
          color = '\\' | 0x0100;
          break;

        default:
          color = 0xffffffff;
          break;
        }

      	for (i = 0; i < mbi->framebuffer_width
             && i < mbi->framebuffer_height; i++)
        {
          switch (mbi->framebuffer_bpp)
            {
            case 8:
              {
                multiboot_uint8_t *pixel = fb + mbi->framebuffer_pitch * i + i;
                *pixel = color;
              }
              break;
            case 15:
            case 16:
              {
                multiboot_uint16_t *pixel
                  = fb + mbi->framebuffer_pitch * i + 2 * i;
                *pixel = color;
              }
              break;
            case 24:
              {
                multiboot_uint32_t *pixel
                  = fb + mbi->framebuffer_pitch * i + 3 * i;
                *pixel = (color & 0xffffff) | (*pixel & 0xff000000);
              }
              break;

            case 32:
              {
                multiboot_uint32_t *pixel
                  = fb + mbi->framebuffer_pitch * i + 4 * i;
                *pixel = color;
              }
              break;
            }
        }
    }

	//unsigned char *fb = (unsigned char*)(addr + 88);

	/*
	vgacon_init();

	vgacon_clear(VGA_COLOR_BLACK);

	vgacon_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

	prints("Welcome!\n\n");

	prints("gdt init\n");
	gdt_setup();
	prints("idt init\n");
	idt_setup();

	shell_main();
	*/

	//putpixel(fb, 1, 1, 0x7800);
}

void keyboard_irq_handler()
{
	prints("Keyboard IRQ");
}
