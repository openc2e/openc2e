#include "../World.h" // TODO: not needed?
#include "qtopenc2e.h"
#include "../Engine.h"

#include <QApplication>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	if (!engine.parseCommandLine(argc, argv)) return 1;

	QtOpenc2e myvat;	
	myvat.show();

	world.drawWorld();

	return app.exec();
}

