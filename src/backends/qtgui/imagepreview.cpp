/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "imagepreview.h"
#include "cobfile/c1cobfile.h"
#include "cobfile/cobFile.h"
#include "endianlove.h"
#include "Engine.h"

#include <QPainter>
#include <cassert>

ImagePreview::ImagePreview(QWidget *parent): QWidget(parent) {
	imgdata = 0;
}

ImagePreview::~ImagePreview() {
	if (imgdata) delete imgdata;
}

void ImagePreview::onSelect(QListWidgetItem *current, QListWidgetItem *prev) {
	previewimg = QImage();

	if (!current) {
		update();
		return;
	}

	if (engine.version == 1) {
		QString filename = current->toolTip();
		std::ifstream cobstream(filename.toLatin1(), std::ios::binary);
		if (cobstream.fail()) {
			return; // TODO: throw some kind of warning or something
		}

		c1cobfile cobfile = read_c1cobfile(cobstream);

		if (cobfile.picture_width > 0 && cobfile.picture_height > 0) {
			previewimg = QImage((uchar*)cobfile.picture_data.data(), cobfile.picture_width, cobfile.picture_height, QImage::Format_Indexed8).mirrored();
			previewimg.setColorCount(256);
			unsigned char *palette = engine.getPalette();
			if (palette) {
				for (unsigned int i = 0; i < 256; i++) {
					previewimg.setColor(i, QColor(palette[i*3],palette[i*3+1],palette[i*3+2]).rgb());
				}
			}
		}
	} else if (engine.version == 2) {
		cobAgentBlock *b = (cobAgentBlock *)current->data(Qt::UserRole).value<void *>();
		assert(b);
		if (b->thumbnailwidth > 0 && b->thumbnailheight > 0) {
			unsigned short *oldimgdata = b->thumbnail;
			assert(b->thumbnail);
			if (imgdata) delete imgdata;
			imgdata = new unsigned int[b->thumbnailwidth * b->thumbnailheight];

			for (unsigned int i = 0; i < (unsigned int)b->thumbnailwidth * (unsigned int)b->thumbnailheight; i++) {
				unsigned int v = oldimgdata[i];
				v = swapEndianShort(v);
				unsigned int red = ((v & 0xf800) >> 8) & 0xFF;
				unsigned int green = ((v & 0x07e0) >> 3) & 0xFF;
				unsigned int blue = ((v & 0x001f) << 3) & 0xFF;
				imgdata[i] = (red << 16) + (green << 8) + blue;
			}

			previewimg = QImage((uchar*)imgdata, b->thumbnailwidth, b->thumbnailheight, QImage::Format_RGB32);
		}
	}

	update();
}

void ImagePreview::paintEvent(QPaintEvent *event) {
	if (previewimg.width() > 0) {
		QPainter painter(this);
		painter.drawImage(QPoint(width()/2 - previewimg.width()/2,height()/2 - previewimg.height()/2), previewimg);
	}
}
