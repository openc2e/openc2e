#include "CameraPart.h"
#include "Backend.h"
#include "Camera.h"
#include "Engine.h"
#include "World.h"

CameraPart::CameraPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
		unsigned int _z, unsigned int view_width, unsigned int view_height, unsigned int camera_width, unsigned int camera_height)
		: SpritePart(p, _id, spritefile, fimg, _x, _y, _z),
		  rendertarget(engine.backend->newRenderTarget(view_width, view_height), [](RenderTarget *t) { engine.backend->freeRenderTarget(t); }) {
	viewwidth = view_width;
	viewheight = view_height;
	camerawidth = camera_width;
	cameraheight = camera_height;
	camera = shared_ptr<Camera>(new PartCamera(this));
}

void CameraPart::partRender(RenderTarget *renderer, int xoffset, int yoffset) {
	// TODO: hack to stop us rendering cameras inside cameras. but cameras are marked as camera-shy anyways...
	if (renderer == engine.backend->getMainRenderTarget()) {
		// make sure we're onscreen before bothering to do any work..
		if (xoffset + x + (int)camerawidth >= 0 && yoffset + y + (int)cameraheight >= 0 &&
			xoffset + x < (int)renderer->getWidth() && yoffset + y < (int)renderer->getHeight()) {
			world.drawWorld(camera.get(), rendertarget.get());
		}
	}
	
	renderer->blitRenderTarget(rendertarget.get(), xoffset + x, yoffset + y, camerawidth, cameraheight);
	
	SpritePart::partRender(renderer, xoffset, yoffset);
}

void CameraPart::tick() {
	SpritePart::tick();

	camera->tick();
}