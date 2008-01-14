#include "imagepreview.h"
#include "c1cobfile.h"

#include "../Engine.h"

#include <QPainter>

ImagePreview::ImagePreview(QWidget *parent): QWidget(parent) {
}

void ImagePreview::onSelect(QListWidgetItem *current, QListWidgetItem *prev) {
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
	} else
		previewimg = QImage();

	update();
}

void ImagePreview::paintEvent(QPaintEvent *event) {
	if (previewimg.width() > 0) {
		QPainter painter(this);
		painter.drawImage(QPoint(width()/2 - previewimg.width()/2,height()/2 - previewimg.height()/2), previewimg);
	}
}
