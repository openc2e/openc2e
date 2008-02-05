#ifndef BRAINVIEWER_H
#define BRAINVIEWER_H 1

#include <QtGui>

class BrainViewer : public QDialog {
	Q_OBJECT

	public:
		BrainViewer(QWidget *parent = 0);
		~BrainViewer();

	private slots:
		void onTick();
	
	private:
		class BrainView *brainview;
		class QScrollArea *scrollArea;
};

#endif /* BRAINVIEWER_H */

