import sys
from PIL import Image, ImageDraw, ImageFont

# Config
name="font"
size=20
width=32
height=32
chars="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-/*#+ß!§$%&"
count=200
out_file="font.h"

# Colors
COLOR_BLACK=(0, 0, 0)
COLOR_WHITE=(255, 255, 255)

data="/* Generated using font converter tool by TheHeroCraft1579 */\n\n"
# C Definition
data+="int %s[][%s] = {" % (name, width*height)

font = ImageFont.truetype(sys.argv[1], size)

for a in range(count):
	for b in range(len(chars)):
		print("a: %s, b: %s, char: %s" % (a, b, chars[b]))
		if(ord(chars[b]) == a):
			char = chars[b]
			print("%s, ASCI: %s, Index: %s" % (char, ord(char), a))
    			# Write char on image
			im = Image.new("RGB",(width,height), color=(0, 0, 0))
			draw = ImageDraw.Draw(im)
			w, h = draw.textsize(char, font=font)
			draw.fontmode = "1"
			draw.text(((width-w)/2,(height-h)/2), char, fill="white", font=font)

    			#im.save("char_%s.png" % char, "PNG")

			pixels = im.load()

			data+="\n"
			data+="{"

    			# Convert pixel
			for c in range(height):
				for d in range(width):
					if (pixels[d, c] == COLOR_BLACK):
						if d == width-1 and c == height-1:
							data+="0"
						else:
							data+="0, "
					else:
						if d == width-1 and c == height-1:
							data+="1"
						else:
							data+="1, "

			data+="}, // ASCI: %s\n" % (ord(char))
			break

	else:

		data+="\n"
		data+="{"
		for e in range(width*height-1):
			data+="0, "

		if a == count-1:
			data+="0}  // None\n"
		else:
			data+="0}, // None\n"

# Footer
data+="};\n\n"
data+="/* END */"

with open(out_file, "w") as fp:
	fp.write(data)

