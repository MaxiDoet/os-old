void putpixel(int x, int y, int color);
void draw_horizontal_line(int x, int y, int w, int color);
void draw_vertical_line(int x, int y, int h, int color);
void draw_line(int x0, int y0, int x1, int y1, int color);
void draw_filled_rectangle(int x, int y, int w, int h, int color);
void draw_rectangle(int x, int y, int w, int h, int size, int color);
void draw_rounded_rectangle(int x, int y, int w, int h, int color);
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

void draw_rounded_rectangle(int x, int y, int w, int h, int color)
{
	draw_filled_rectangle(x, y, w, h, color);

	draw_circle_filled(x+2, y+2, 2, color);

	draw_horizontal_line(x, y, 3, 0xFFF);
	draw_vertical_line(x, y, 3, 0xFFF);
	draw_line(x, y, x+3, y+3, 0xFFF);
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

