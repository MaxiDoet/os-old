void putpixel(int x, int y, int color);
void draw_horizontal_line(int x, int y, int w, int color);
void draw_vertical_line(int x, int y, int h, int color);
void draw_line(int x0, int y0, int x1, int y1, int color);
void draw_filled_rectangle(int x, int y, int w, int h, int color);
void draw_rectangle(int x, int y, int w, int h, int size, int color);
void draw_rounded_rectangle(int x, int y, int w, int h, int r, int color);
void draw_circle(int x0, int y0, int radius, int color);
void draw_circle_filled(int x0, int y0, int radius, int color);
void draw_monochrome_bitmap(int x, int y, int *bitmap, int color);
void draw_string(int x, int y, int font[][1024], char* str, int color);
void draw_image(int x, int y, int w, int h, int data[]);
void draw_image_transparent(int x, int y, int w, int h, int data[]);

void putpixel(int x, int y, int color)
{
        multiboot_uint16_t *pixel = fb + fb_pitch * y + 2 * x;
        *pixel = color;
}

void draw_horizontal_line(int x, int y, int w, int color)
{
	int i;

	multiboot_uint16_t *pixel = fb + fb_pitch * y + 2 * x;

	for (i=0; i<w; i++) {
		*pixel = color;
		*pixel++;
	}
}

void draw_vertical_line(int x, int y, int h, int color)
{
	int i;

        multiboot_uint16_t *pixel = fb + fb_pitch * y + 2 * x;

        for (i=0; i<h; i++) {
                *pixel = color;
                pixel = fb + fb_pitch * (y+i) + 2 * x;
        }
}

void draw_line(int x0, int y0, int x1, int y1, int color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int D = 2*dy - dx;
	int y = y0;
	int x;

	for (x=x0; x<x1; x++) {
		putpixel(x, y, color);
		if (D > 0) {
			y++;
			D-=2*dx;
		}
		D+=2*dy;
	}
}

void draw_filled_rectangle(int x, int y, int w, int h, int color)
{
	int i;

	for(i=0; i<h; i++) {
		draw_horizontal_line(x, y+i, w, color);
	}
}

void draw_rectangle(int x, int y, int w, int h, int size, int color)
{
	if (size==1) {
		// Top, Bottom
		draw_horizontal_line(x, y, w, color);
		draw_horizontal_line(x, y+h, w, color);
		// Left, Right
		draw_vertical_line(x, y, h, color);
		draw_vertical_line(x+w, y, h, color);
	} else {
		// Top, Bottom
		draw_filled_rectangle(x, y, w, size, color);
		draw_filled_rectangle(x, y+h, w, size, color);
		// Left, Right
		draw_filled_rectangle(x, y, size, h, color);
		draw_filled_rectangle(x+w, y, size, h+size, color);
	}
}

/*
void draw_rounded_rectangle(int x, int y, int w, int h, int radius, int color)
{
	//draw_filled_rectangle(x+radius*2, y+radius*2, w-radius*2, h-radius*2, color);

	// Top left
	draw_circle_filled(x+radius, y+radius, radius, 0xFFF);
	// Top right
	draw_circle_filled(x+w-radius, y+radius, radius, 0xFFF);
	// Bottom left
	draw_circle_filled(x+radius, y+h-radius, radius, 0xFFF);
	// Bottom right
	draw_circle_filled(x+w-radius, y+h-radius, radius, 0xFFF);

	// Left
	draw_filled_rectangle(x, y+radius*2, radius*2, h-radius*4-1, 0x37E0);
	// Right
	draw_filled_rectangle(x+w, y+radius, radius*2, h-radius, 0x37E0);
	// Top
	draw_filled_rectangle(x+radius*2, y, w-radius, radius*2, 0x37E0);
	// Bottom
        draw_filled_rectangle(x, y+h, w, radius, 0x37E0);

	// Debug
	putpixel(x, y, 0xF800);
	putpixel(x+w, y, 0xF800);
	putpixel(x, y+h, 0xF800);
	putpixel(x+w, y+h, 0xF800);

}
*/
/*
void draw_rounded_rectangle(int x, int y, int w, int h, int r, int color)
{
	draw_horizontal_line(x+r  , y    , w-2*r, color); // Top
  	draw_horizontal_line(x+r  , y+h-1, w-2*r, color); // Bottom
  	draw_vertical_line(x    , y+r  , h-2*r, color); // Left
	draw_vertical_line(x+w-1, y+r  , h-2*r, color); // Right

  	draw_circle_filled(x+r    , y+r    , r, color);
  	draw_circle_filled(x+w-r-1, y+r    , r, color);
  	draw_circle_filled(x+w-r-1, y+h-r-1, r, color);
  	draw_circle_filled(x+r    , y+h-r-1, r, color);
}
*/

void e(short x0, short y0, short r, int cornername, int delta, int color)
{
	short f     = 1 - r;
  	short ddF_x = 1;
  	short ddF_y = -2 * r;
  	short x     = 0;
  	short y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      draw_vertical_line(x0+x, y0-y, 2*y+1+delta, color);
      draw_vertical_line(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      draw_vertical_line(x0-x, y0-y, 2*y+1+delta, color);
      draw_vertical_line(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

void draw_rounded_rectangle(int x, int y, int w, int h, int r, int color) {
  draw_filled_rectangle(x+r, y, w-2*r, h, color);

  // draw four corners
  e(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  e(x+r    , y+r, r, 2, h-2*r-1, color);
}

void draw_circle(int x0, int y0, int radius, int color)
{
	/* Source: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm */

	int f = 1-radius;
	int ddF_x = 0;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	putpixel(x0, y0 + radius, color);
	putpixel(x0, y0 - radius, color);
	putpixel(x0 + radius, y0, color);
	putpixel(x0 - radius, y0, color);

	while(x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x + 1;

		putpixel(x0 + x, y0 + y, color);
		putpixel(x0 - x, y0 + y, color);
		putpixel(x0 + x, y0 - y, color);
		putpixel(x0 - x, y0 - y, color);
		putpixel(x0 + y, y0 + x, color);
		putpixel(x0 - y, y0 + x, color);
		putpixel(x0 + y, y0 - x, color);
		putpixel(x0 - y, y0 - x, color);
	}
}

void draw_circle_filled(int x0, int y0, int radius, int color)
{
	/* Source: https://stackoverflow.com/a/14976268/13378037 */

	int x = radius;
	int y = 0;
	int xChange = 1 - (radius << 1);
	int yChange = 0;
	int radiusError = 0;

	while (x >= y) {
		for(int i = x0 - x; i <= x0 + x; i++) {
			putpixel(i, y0 + y, color);
			putpixel(i, y0 - y, color);
		}
		for(int i = x0 - y; i <= x0 + y; i++) {
			putpixel(i, y0 + x, color);
			putpixel(i, y0 - x, color);
		}

		y++;
		radiusError += yChange;
		yChange += 2;
		if(((radiusError << 1) + xChange) > 0) {
			x--;
			radiusError += xChange;
			xChange += 2;
		}
	}
}

void draw_monochrome_bitmap(int x, int y, int *bitmap, int color)
{
	for(int i=0; i < 32; i++) {
		for(int j=0; j < 32; j++) {
			if(bitmap[j+i*32] == 1) {
				putpixel(x+j, y+i, color);
			} else {
				continue;
			}
		}
	}
}

void draw_string(int x, int y, int font[][1024], char* str, int color)
{
	int j=0;
	for(int i=0; str[i] != '\0'; i++) {
		draw_monochrome_bitmap(x+j, y, font[str[i]], color);
		j+=10;
	}
}

void draw_image(int x, int y, int w, int h, int data[])
{
        int i,j;

        for (i=0; i<h; i++) {
                for (j=0; j<w; j++) {
                        int color24 = data[i*w+j];
                        int color16;
                        color16 = (((color24&0xf80000)>>8) + ((color24&0xfc00)>>5) + ((color24&0xf8)>>3));
                        putpixel(x+j, y+i, color16);
                }
        }
}

void draw_image_transparent(int x, int y, int w, int h, int data[])
{
        int i,j;

        for (i=0; i<h; i++) {
                for (j=0; j<w; j++) {
			if (data[i*w+j] == 0x20304) continue;

                        int color24 = data[i*w+j];
                        int color16;
                        color16 = (((color24&0xf80000)>>8) + ((color24&0xfc00)>>5) + ((color24&0xf8)>>3));
                        putpixel(x+j, y+i, color16);
                }
        }
}

