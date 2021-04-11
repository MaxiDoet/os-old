import sys
from PIL import Image, ImageDraw, ImageFont

def rgb2hex(r, g, b, a):
    return '0x{:02x}{:02x}{:02x}'.format(r, g, b)

# Config
name="image"
out_file="image.h"

im = Image.open(sys.argv[1])
width, height = im.size
im = im.convert("RGBA")
pixels = im.load()

data="/* Generated using image converter tool by TheHeroCraft1579 */\n\n"
# C Definition
data+="int %s[%s] = {" % (name, width*height)

print(im.format)

for y in range(height):
	for x in range(width):
		print(pixels[x,y])
		print("Pixel: %s, %s Color: %s" % (x, y, rgb2hex(*pixels[x, y])))
		if x == width-1 and y == height-1:
			if pixels[x, y][3] == 0:
				data+="0x20304" # Secret transparent code :-)
			else:
				data+="%s" % rgb2hex(*pixels[x, y])
		else:
			if pixels[x, y][3] == 0:
				data+="0x20304, "
			else:
				data+="%s, " % rgb2hex(*pixels[x, y])

	data+="\n"

# Footer
data+="};\n\n"
data+="/* END */"

with open(out_file, "w") as fp:
	fp.write(data)

