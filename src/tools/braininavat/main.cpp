#include "braininavat.h"

#include <QApplication>

#include "../../Catalogue.h"
#include <boost/filesystem.hpp>

#include <stdlib.h>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	srand(time(NULL));

	boost::filesystem::path p(".");
	catalogue.initFrom(p);

	BrainInAVat myvat;
	myvat.show();

	return app.exec();
}

