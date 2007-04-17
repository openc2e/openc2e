#include "braininavat.h"

class BrainView : public QWidget {
	Q_OBJECT

private:
	BrainInAVat *parent;

public:
	BrainView(BrainInAVat *p);
	QSize minimumSize();
	void paintEvent(QPaintEvent *event);
};

