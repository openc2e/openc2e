#include "CameraPart.h"

#include "Camera.h"
#include "Engine.h"
#include "World.h"
#include "common/backend/Backend.h"

CameraPart::CameraPart(Agent* p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
	unsigned int _z, unsigned int view_width, unsigned int view_height, unsigned int camera_width, unsigned int camera_height)
	: SpritePart(p, _id, spritefile, fimg, _x, _y, _z),
	  rendertarget(get_backend()->newRenderTarget(view_width, view_height)) {
	viewwidth = view_width;
	viewheight = view_height;
	camerawidth = camera_width;
	cameraheight = camera_height;
	camera = std::shared_ptr<Camera>(new PartCamera(this));
}

void CameraPart::partRender(RenderTarget* renderer, int xoffset, int yoffset) {
	// TODO: hack to stop us rendering cameras inside cameras. but cameras are marked as camera-shy anyways...
	if (renderer == get_backend()->getMainRenderTarget().get()) {
		// make sure we're onscreen before bothering to do any work..
		if (xoffset + x + (int)camerawidth >= 0 && yoffset + y + (int)cameraheight >= 0 &&
			xoffset + x < (int)renderer->getWidth() && yoffset + y < (int)renderer->getHeight()) {
			world.drawWorld(camera.get(), rendertarget.get());
		}
	}

	renderer->blitRenderTarget(rendertarget.get(), Rect2f(xoffset + x, yoffset + y, camerawidth, cameraheight));

	SpritePart::partRender(renderer, xoffset, yoffset);
}

void CameraPart::tick() {
	SpritePart::tick();

	camera->tick();
}