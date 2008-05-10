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

#ifndef IMAGEPREVIEW_H
#define IMAGEPREVIEW_H 1

#include <QWidget>
#include <QListWidgetItem>

class ImagePreview : public QWidget {
	Q_OBJECT

	public:
		ImagePreview(QWidget *parent = 0);
		~ImagePreview();
		void paintEvent(QPaintEvent *event);

	public slots:
		void onSelect(QListWidgetItem *current, QListWidgetItem *prev);

	private:
		QImage previewimg; // TODO: change to a boost::scoped_ptr or similar?
		unsigned int *imgdata;
};

#endif /* IMAGEPREVIEW_H */
