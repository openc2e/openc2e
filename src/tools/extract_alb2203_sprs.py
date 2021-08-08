import glob
import os
import struct

import PIL.Image


def read_palette_dta(f):
    # in a format usable by PIL
    palette = []
    for _ in range(256 * 3):
        palette.append(f.read(1)[0] << 2)
    return palette


def read_old_spr(f):
    numimages = struct.unpack("<H", f.read(2))[0]
    offsets = []
    widths = []
    heights = []
    for _ in range(numimages):
        offsets.append(struct.unpack("<H", f.read(2))[0])
        widths.append(struct.unpack("B", f.read(1))[0])
        heights.append(struct.unpack("B", f.read(1))[0])

    images = []
    for i in range(numimages):
        assert offsets[i] == f.tell()
        data = f.read(widths[i] * heights[i])
        img = PIL.Image.frombytes("P", size=(widths[i], heights[i]), data=data)
        images.append(img)

    return images


def is_background_piece(images):
    return (len(images) == 16 or len(images) == 128) and all(
        i.width == 80 and i.height == 40 for i in images
    )


def stitch_old_spr(images):
    assert len(images) == 16 or len(images) == 128
    for i in images:
        assert i.width == images[0].width
        assert i.height == images[0].height

    sprwidth = images[0].width
    sprheight = images[0].height
    heightinsprites = 4
    widthinsprites = len(images) // heightinsprites
    totalwidth = sprwidth * widthinsprites
    totalheight = sprheight * heightinsprites

    newimage = PIL.Image.new("P", size=(totalwidth, totalheight))
    pixels = newimage.load()

    for i in range(heightinsprites):
        for j in range(widthinsprites):
            whereweare = j + i * widthinsprites
            newimage.paste(
                images[whereweare],
                (j * sprwidth, i * sprheight, (j + 1) * sprwidth, (i + 1) * sprheight),
            )
    return newimage


def stitch_to_sheet(images):
    SHEET_PADDING = 5
    max_width = 800
    total_width = 0
    total_height = 0

    # find good row size
    current_row_width = SHEET_PADDING
    current_row_height = 0
    for i in images:
        if current_row_width + i.width + SHEET_PADDING > max_width:
            total_height += current_row_height + SHEET_PADDING
            current_row_width = SHEET_PADDING
            current_row_height = 0
        current_row_width += i.width + SHEET_PADDING
        current_row_height = max(current_row_height, i.height)
        total_width = max(total_width, current_row_width)
    total_height += SHEET_PADDING + current_row_height + SHEET_PADDING

    # set up storage, set to default background color
    newimage = PIL.Image.new("P", size=(total_width, total_height))
    # 0x1C,0x29,0x3B in C1 palette.dta -> rgb(112, 164, 236)
    newimage.paste(100, (0, 0, newimage.width, newimage.height))

    # copy sprites over
    current_x = SHEET_PADDING
    current_y = SHEET_PADDING
    current_row_height = 0
    for i in images:
        # check row
        if current_x + i.width + SHEET_PADDING > total_width:
            current_y += current_row_height + SHEET_PADDING
            current_x = SHEET_PADDING
            current_row_height = 0
        # copy
        for y in range(i.height):
            newimage.paste(
                i, (current_x, current_y, current_x + i.width, current_y + i.height)
            )
        # auto rowstart = ((uint8_t*)i.data.data()) + y * i.width * depth;
        # auto rowend = rowstart + i.width * depth;
        # auto insertpos = &data[(current_y + y) * total_width * depth + current_x * depth];
        # std::copy(rowstart, rowend, insertpos);
        # }
        current_x += i.width + SHEET_PADDING
        current_row_height = max(current_row_height, i.height)

    return newimage


palettes = {}
with open("PALETTE.DTA", "rb") as f:
    palettes["default"] = read_palette_dta(f)
for i in [0, 1, 2, 3, 4]:
    with open("{}.PAL".format(i), "rb") as f:
        palettes[i] = read_palette_dta(f)

for k in palettes:
    try:
        os.makedirs("palette-{}".format(k))
    except FileExistsError:
        pass

background_sprites = []
for fname in sorted(glob.glob("*.SPR")):
    with open(fname, "rb") as f:
        images = read_old_spr(f)
        if is_background_piece(images):
            background_sprites.append(stitch_old_spr(images))
        else:
            newimage = stitch_to_sheet(images)
            for k, v in palettes.items():
                newimage.putpalette(v)
                newimage.save("palette-{}/{}.png".format(k, fname))

assert len(background_sprites) == 128
background = stitch_old_spr(background_sprites)

for k, v in palettes.items():
    background.putpalette(v)
    background.save("palette-{}/background-{}.png".format(k, k))

# with open("BKGND.DTA", "rb") as f:
#     for i in range(256):
#         line = []
#         line.append(struct.unpack("<H", f.read(2))[0])
#         line.append(struct.unpack("<H", f.read(2))[0])
#         line.append(struct.unpack("<H", f.read(2))[0])
#         line.append(struct.unpack("<H", f.read(2))[0])
#         line.append(struct.unpack("<H", f.read(2))[0])
#         line.append(struct.unpack("<H", f.read(2))[0])
#         line.append(struct.unpack("<H", f.read(2))[0])
#         line.append(struct.unpack("<H", f.read(2))[0])
#         print("{}: {}".format(i, line))
