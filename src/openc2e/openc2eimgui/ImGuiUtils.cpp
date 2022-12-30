#include "ImGuiUtils.h"

#include "Engine.h"
#include "PathResolver.h"
#include "World.h"
#include "common/backend/Backend.h"
#include "fileformats/ImageUtils.h"
#include "fileformats/peFile.h"

#include <fmt/format.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <unordered_map>

namespace ImGuiUtils {

void Image(Texture texture) {
	ImGui::Image(texture.as<ImTextureID>(), ImVec2(texture.width, texture.height));
}

Texture GetTextureFromExeFile(uint32_t resource) {
	return get_backend()->createTexture(engine.getExeFile()->getBitmap(resource));
}

Texture GetTextureFromExeFileWithTransparentTopLeft(uint32_t resource) {
	auto image = engine.getExeFile()->getBitmap(resource);
	// TODO: don't make all pixels of this color transparent, only pixels of this
	// color that are connected to the edges of the image
	return get_backend()->createTextureWithTransparentColor(image, ImageUtils::GetPixelColor(image, 0, 0));
}

std::vector<Texture> LoadImageWithTransparentTopLeft(const std::string& name) {
	std::string path = findMainDirectoryFile(name);
	if (path.empty()) {
		return {};
	}

	MultiImage images = ImageUtils::ReadImage(path);
	std::vector<Texture> textures(images.size());
	for (size_t i = 0; i < images.size(); ++i) {
		textures[i] = get_backend()->createTextureWithTransparentColor(images[i], ImageUtils::GetPixelColor(images[i], 0, 0));
	}
	return textures;
}

void DrawTexture(Texture texture, ImVec2 p, float transparency) {
	ImDrawList* drawlist = ImGui::GetWindowDrawList();
	drawlist->AddImage(
		texture.as<ImTextureID>(),
		p,
		p + ImVec2(texture.width, texture.height),
		ImVec2(0, 0),
		ImVec2(1, 1),
		IM_COL32(255, 255, 255, transparency * 255));
}

void DisabledButton(const char* text) {
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	ImGui::Button(text);
	ImGui::PopItemFlag();
	ImGui::PopStyleVar();
}

bool ImageButton(Texture tex, bool enabled) {
	return ImageButton(TextureRect{tex, 0, 0, tex.width, tex.height}, enabled);
}

bool ImageButton(TextureRect tex, bool enabled) {
	ImVec4 tint(1, 1, 1, 1);
	if (!enabled) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		tint = ImVec4(1, 1, 1, 0.8);
	}

	// ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
	// ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
	ImGui::PushID(fmt::format("{}_{}_{}_{}_{}", (uintptr_t)tex.parent.as<void*>(), tex.x, tex.y, tex.w, tex.h).c_str());
	bool ret = ImGui::ImageButton(
		tex.parent.as<ImTextureID>(),
		ImVec2(tex.w, tex.h),
		ImVec2(tex.x * 1.0 / tex.parent.width, tex.y * 1.0 / tex.parent.height),
		ImVec2((tex.x + tex.w) * 1.0 / tex.parent.width, (tex.y + tex.h) * 1.0 / tex.parent.height),
		-1, // padding
		ImVec4(0, 0, 0, 0), // bgcolor
		tint);
	ImGui::PopID();
	// ImGui::PopStyleColor();
	// ImGui::PopStyleColor();

	if (!enabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	return ret;
}

bool BeginWindow(const char* title, bool* is_open, ImGuiWindowFlags flags) {
	if (!*is_open) {
		return false;
	}
	return ImGui::Begin(title, is_open, flags | ImGuiWindowFlags_NoCollapse);
}

void EndWindow() {
	return ImGui::End();
}

} // namespace ImGuiUtils