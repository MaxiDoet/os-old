import sys
from PIL import Image, ImageDraw, ImageFont

def rgb2hex(r, g, b):
    return '0x{:02x}{:02x}{:02x}'.format(r, g, b)

# Config
name="image"
out_file="image.h"

im = Image.open(sys.argv[1])
width, height = im.size
pixels = im.load()

data="/* Generated using image converter tool by TheHeroCraft1579 */\n\n"
# C Definition
data+="int %s[%s] = {" % (name, width*height)

if (width, height) is not im.size:
	print("Warning: Image size does not match size settings!")

for y in range(height):
	for x in range(width):
		print("Pixel: %s, %s Color: %s" % (x, y, rgb2hex(*pixels[x, y])))
		if x == width-1 and y == height-1:
			data+="%s" % rgb2hex(*pixels[x, y])
		else:
			data+="%s, " % rgb2hex(*pixels[x, y])
	data+="\n"

# Footer
data+="};\n\n"
data+="/* END */"

with open(out_file, "w") as fp:
	fp.write(data)

