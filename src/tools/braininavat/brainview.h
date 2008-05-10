#include <QtGui>

class BrainView : public QWidget {
	Q_OBJECT

private:
	class Creature *creature;

	void drawLobeBoundaries(QPainter &painter, unsigned int x, unsigned int y, unsigned int width, unsigned int height, std::string text);

public:
	unsigned int neuron_var;
	unsigned int dendrite_var;
	float threshold;

	BrainView();
	void setCreature(class Creature *);

	QSize minimumSize();
	
	void paintEvent(QPaintEvent *event);

	void drawOldBrain(class oldBrain *b);
	void drawC2eBrain(class c2eBrain *b);
};

