import colorama
import argparse
import os
import sys
import cv2
from PIL import Image, ImageDraw, ImageFont

def rgb2hex(r, g, b):
    return '0x{:02x}{:02x}{:02x}'.format(r, g, b)


def info(text):
	print(colorama.Fore.BLUE, colorama.Back.WHITE, "INFO ", colorama.Style.RESET_ALL, text)

def warn(text):
	print(colorama.Fore.RED, colorama.Back.WHITE, "WARN ", colorama.Style.RESET_ALL, text)

parser = argparse.ArgumentParser()
parser.add_argument("video", help="a video file", type=str)
args = parser.parse_args()

colorama.init()

# Config
name="video"
out_file="video.h"

# Get number of frames
cap = cv2.VideoCapture(str(args.video))
nof = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
fps = cap.get(cv2.CAP_PROP_FPS)

info("Frames: %s" % str(nof))
info("Width: %s" % str(width))
info("Height: %s" % str(height))
info("FPS: %s" % str(fps))

# Extract all frames using ffmpeg
info("Extracting frames")
os.system("ffmpeg -hide_banner -loglevel error -i %s -vf scale=%s:%s '%%d.jpg'" % (str(args.video), width, height))


info("Converting")
data="/* Generated using video converter tool by TheHeroCraft1579 */\n\n"
# C Definition
data+="int %s[%s] = {" % (name, nof)

for k in range(nof):

	im = Image.open("%s.jpg" % str(k+1))
	width, height = im.size
	pixels = im.load()

	if (width, height) is not im.size:
		warn("Warning: Image size does not match size settings!")

	for y in range(height):
		for x in range(width):
			info("Frame: %s x:%s y:%s %s" % (k+1, x, y, rgb2hex(*pixels[x, y])))
			if x == width-1 and y == height-1:
				data+="%s" % rgb2hex(*pixels[x, y])
			else:
				data+="%s, " % rgb2hex(*pixels[x, y])
		data+="\n"

# Footer
data+="};\n\n"
data+="/* END */"

info("Writing to file")
with open(out_file, "w") as fp:
	fp.write(data)

info("Done")
os.system("rm *.jpg*")
