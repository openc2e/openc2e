#include "braininavat.h"

class BrainView : public QWidget {
	Q_OBJECT

private:
	BrainInAVat *parent;

public:
	unsigned int neuron_var;
	unsigned int dendrite_var;
	float threshold;

	BrainView(BrainInAVat *p);
	QSize minimumSize();
	void paintEvent(QPaintEvent *event);
};

