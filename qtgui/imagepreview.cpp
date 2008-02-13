#include "imagepreview.h"
#include "c1cobfile.h"

#include "Engine.h"
#include "cobFile.h"

#include <QPainter>

ImagePreview::ImagePreview(QWidget *parent): QWidget(parent) {
	imgdata = 0;
}

ImagePreview::~ImagePreview() {
	if (imgdata) delete imgdata;
}

void ImagePreview::onSelect(QListWidgetItem *current, QListWidgetItem *prev) {
	previewimg = QImage();

	if (engine.version == 1) {
		QString filename = current->toolTip();
		std::ifstream cobstream(filename.toAscii(), std::ios::binary);
		if (cobstream.fail()) {
			return; // TODO: throw some kind of warning or something
		}

		c1cobfile cobfile(cobstream);

		if (cobfile.imagewidth > 0 && cobfile.imageheight > 0) {
			previewimg = QImage((uchar*)cobfile.imagedata.get(), cobfile.imagewidth, cobfile.imageheight, QImage::Format_Indexed8).mirrored();
			previewimg.setNumColors(256);
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

			for (unsigned int i = 0; i < b->thumbnailwidth * b->thumbnailheight; i++) {
				unsigned int v = oldimgdata[i];
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
