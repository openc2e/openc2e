#include "debugkit.h"
#include <QApplication>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	DebugKit mykit;
	mykit.show();

	return app.exec();
}

