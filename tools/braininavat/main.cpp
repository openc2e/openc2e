#include "braininavat.h"

#include <QApplication>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	BrainInAVat myvat;
	myvat.show();

	return app.exec();
}

