#include <QtGui>

class BrainView : public QWidget {
	Q_OBJECT

private:
	class c2eCreature *creature;

public:
	unsigned int neuron_var;
	unsigned int dendrite_var;
	float threshold;

	BrainView();
	QSize minimumSize();
	void paintEvent(QPaintEvent *event);
	void setCreature(class c2eCreature *);
};

