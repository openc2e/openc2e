#include "Engine.h"
#include "World.h"
#include "Backend.h"

int Backend::run(int argc, char **argv) {
	resize(800, 600);
	
	// do a first-pass draw of the world. TODO: correct?
	world.drawWorld();

	while (!engine.done) {
		if (!engine.tick()) // if the engine didn't need an update..
			delay(10); // .. delay for a short while
	} // main loop

	return 0;
}
