import sys
from PIL import Image, ImageDraw, ImageFont

# Config
name="tm"
size=20
width=16
height=32
chars="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
out_file="font.h"

# Colors
COLOR_BLACK=(0, 0, 0)
COLOR_WHITE=(255, 255, 255)

data="/* Generated using font converter tool by TheHeroCraft1579 */\n\n"

for i in range(len(chars)):

    # C Definition
    data+="int %s_char_%s[%s] = {" % (name, chars[i], width*height)

    print("Char: %s" % chars[i])

    # Write char on image
    char=chars[i]
    font = ImageFont.truetype(sys.argv[1], size)

    im = Image.new("RGB",(width,height), color=(0, 0, 0))
    draw = ImageDraw.Draw(im)
    w, h = draw.textsize(char, font=font)
    draw.fontmode = "1"
    draw.text(((width-w)/2,(height-h)/2), char, fill="white", font=font)

    #im.save("char_%s.png" % char, "PNG")

    pixels = im.load()

    # Convert pixels
    for i in range(height):
        for j in range(width):
            if (pixels[j, i] == COLOR_BLACK):
                data+="0, "
            else:
                data+="1, "

        data+="\n"

    data = data[:-3]

    # Footer
    data+="};\n"



data+="/* END */"

with open(out_file, "w") as fp:
    fp.write(data)

