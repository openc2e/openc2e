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
