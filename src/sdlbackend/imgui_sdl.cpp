#include "imgui_sdl.h"

#include "SDL.h"
#include "SDL_opengl.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"

#include <stdio.h>

typedef void(APIENTRY* GL_EnableClientState_Func)(GLenum cap);
typedef void(APIENTRY* GL_VertexPointer_Func)(GLint size, GLenum type, GLsizei stride, const void* ptr);
typedef void(APIENTRY* GL_ColorPointer_Func)(GLint size, GLenum type, GLsizei stride, const void* ptr);
typedef void(APIENTRY* GL_TexCoordPointer_Func)(GLint size, GLenum type, GLsizei stride, const void* ptr);
typedef void(APIENTRY* GL_DrawElements_Func)(GLenum mode, GLsizei count, GLenum type, const void* indices);
typedef GLenum(APIENTRY* GL_GetError_Func)();
typedef void(APIENTRY* GL_UseProgramObjectARB_Func)(GLhandleARB program);
typedef void(APIENTRY* GL_Enable_Func)(GLenum cap);
typedef void(APIENTRY* GL_Disable_Func)(GLenum cap);
typedef GLboolean(APIENTRY* GL_IsEnabled_Func)(GLenum cap);
typedef void(APIENTRY* GL_GetIntegerv_Func)(GLenum pname, GLint* data);
typedef void(APIENTRY* GL_Scissor_Func)(GLint x, GLint y, GLsizei width, GLsizei height);

static SDL_Renderer* CurrentRenderer = nullptr;

bool ImGuiSDL_Init(SDL_Window* window) {
	SDL_GLContext ctx = SDL_GL_GetCurrentContext();
	if (ctx == nullptr) {
		// not using opengl backend, won't work!
		return false;
	}

	if (!ImGui_ImplSDL2_InitForSDLRenderer(window)) {
		return false;
	}

	SDL_Renderer* renderer = SDL_GetRenderer(window);
	int w = 0, h = 0;
	SDL_GetRendererOutputSize(renderer, &w, &h);

	ImGuiIO& io = ImGui::GetIO();

	// Loads the font texture.
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	static constexpr uint32_t rmask = 0x000000ff, gmask = 0x0000ff00, bmask = 0x00ff0000, amask = 0xff000000;
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixels, width, height, 32, 4 * width, rmask, gmask, bmask, amask);
	io.Fonts->TexID = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	CurrentRenderer = renderer;

	return true;
}

bool ImGuiSDL_ProcessEvent(const SDL_Event* event) {
	return ImGui_ImplSDL2_ProcessEvent(event);
}

void ImGuiSDL_NewFrame(SDL_Window* window) {
	ImGui_ImplSDL2_NewFrame(window);
}

void ImGuiSDL_Shutdown() {
	// Frees up the memory of the font texture.
	ImGuiIO& io = ImGui::GetIO();
	SDL_Texture* texture = static_cast<SDL_Texture*>(io.Fonts->TexID);
	SDL_DestroyTexture(texture);
}

// static void
// GL_SetShader(GL_RenderData * data, GL_Shader shader)
// {
//     if (data->shaders && shader != data->current.shader) {
//         GL_SelectShader(data->shaders, shader);
//         data->current.shader = shader;
//     }
// }

void ImGuiSDL_RenderDrawData(ImDrawData* drawData) {
	float xscale = 1.0;
	float yscale = 1.0;
	SDL_RenderGetScale(CurrentRenderer, &xscale, &yscale);

	// TODO: reset after?
	SDL_BlendMode blendMode;
	SDL_GetRenderDrawBlendMode(CurrentRenderer, &blendMode);
	SDL_SetRenderDrawBlendMode(CurrentRenderer, SDL_BLENDMODE_BLEND);

	// get SHADER_RGBA
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = 1;
	rect.h = 1;
	SDL_RenderCopy(CurrentRenderer, static_cast<SDL_Texture*>(ImGui::GetIO().Fonts->TexID), nullptr, &rect);
	SDL_RenderFlush(CurrentRenderer);

	((GL_EnableClientState_Func)SDL_GL_GetProcAddress("glEnableClientState"))(GL_VERTEX_ARRAY);
	((GL_EnableClientState_Func)SDL_GL_GetProcAddress("glEnableClientState"))(GL_COLOR_ARRAY);
	((GL_EnableClientState_Func)SDL_GL_GetProcAddress("glEnableClientState"))(GL_TEXTURE_COORD_ARRAY);

	for (int n = 0; n < drawData->CmdListsCount; n++) {
		auto commandList = drawData->CmdLists[n];
		auto vertexBuffer = commandList->VtxBuffer;
		auto indexBuffer = commandList->IdxBuffer.Data;

		for (auto& v : vertexBuffer) {
			v.pos.x *= xscale;
			v.pos.y *= yscale;
		}

		// GL_ActivateRenderer(CurrentRenderer);
		// TODO: use textures

		// ((GL_UseProgramObjectARB_FUNC)SDL_GL_GetProcAddress("glUseProgramObjectARB")(SHADER_SOLID));

		// GL_SetShader(data, SHADER_SOLID);
		// GL_SetBlendMode(data, renderer->blendMode);

		// glEnableClientState(GL_VERTEX_ARRAY);

		((GL_VertexPointer_Func)SDL_GL_GetProcAddress("glVertexPointer"))(2, GL_FLOAT, sizeof(ImDrawVert), &vertexBuffer[0].pos);
		((GL_ColorPointer_Func)SDL_GL_GetProcAddress("glColorPointer"))(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), &vertexBuffer[0].col);
		((GL_TexCoordPointer_Func)SDL_GL_GetProcAddress("glTexCoordPointer"))(2, GL_FLOAT, sizeof(ImDrawVert), &vertexBuffer[0].uv);

		GLenum err;
		while ((err = ((GL_GetError_Func)SDL_GL_GetProcAddress("glGetError"))()) != GL_NO_ERROR) {
			printf("gl error %i\n", err);
		}

		for (int cmd_i = 0; cmd_i < commandList->CmdBuffer.Size; cmd_i++) {
			const ImDrawCmd* drawCommand = &commandList->CmdBuffer[cmd_i];

			// have texture now
			SDL_GL_BindTexture((SDL_Texture*)drawCommand->TextureId, NULL, NULL);
			GLenum err;
			while ((err = ((GL_GetError_Func)SDL_GL_GetProcAddress("glGetError"))()) != GL_NO_ERROR) {
				printf("gl error %i\n", err);
			}
			// TODO: texture modmode
			// TODO: texture blend mode
			// TODO: yuv12 stuff
			// TODO: set shader according to texture type
			// something about !GL_shaders_textureSize_supported

			int framebuffer_height;
			SDL_GetRendererOutputSize(CurrentRenderer, nullptr, &framebuffer_height);

			int old_scissor[4];
			bool scissor_was_enabled = ((GL_IsEnabled_Func)SDL_GL_GetProcAddress("glIsEnabled"))(GL_SCISSOR_TEST);
			((GL_Enable_Func)SDL_GL_GetProcAddress("glEnable"))(GL_SCISSOR_TEST);
			((GL_GetIntegerv_Func)SDL_GL_GetProcAddress("glGetIntegerv"))(GL_SCISSOR_BOX, old_scissor);
			((GL_Scissor_Func)SDL_GL_GetProcAddress("glScissor"))(
				drawCommand->ClipRect.x * xscale,
				framebuffer_height - drawCommand->ClipRect.w * yscale,
				(drawCommand->ClipRect.z - drawCommand->ClipRect.x) * xscale,
				(drawCommand->ClipRect.w - drawCommand->ClipRect.y) * yscale);

			if (drawCommand->UserCallback) {
				drawCommand->UserCallback(commandList, drawCommand);
			} else {
				((GL_DrawElements_Func)SDL_GL_GetProcAddress("glDrawElements"))(GL_TRIANGLES, drawCommand->ElemCount, GL_UNSIGNED_SHORT, indexBuffer);
				GLenum err;
				while ((err = ((GL_GetError_Func)SDL_GL_GetProcAddress("glGetError"))()) != GL_NO_ERROR) {
					printf("gl error %i\n", err);
				}
			}

			indexBuffer += drawCommand->ElemCount;
			SDL_GL_UnbindTexture((SDL_Texture*)drawCommand->TextureId);
			if (!scissor_was_enabled) {
				((GL_Disable_Func)SDL_GL_GetProcAddress("glDisable"))(GL_SCISSOR_TEST);
			}
			((GL_Scissor_Func)SDL_GL_GetProcAddress("glScissor"))(
				old_scissor[0],
				old_scissor[1],
				old_scissor[2],
				old_scissor[3]);
		}
	}
}
