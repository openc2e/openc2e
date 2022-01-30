#pragma once

#include "common/Image.h"
#include "fileformats/ImageUtils.h"
#include "fileformats/paletteFile.h"
#include "openc2e-core/creaturesImage.h"

#include <map>
#include <memory>

class PathManager;

class ImageManager {
  public:
	enum ImageType {
		IMAGE_IS_BACKGROUND = 1,
		IMAGE_SPR = 2,
	};

	ImageManager(std::shared_ptr<PathManager>);
	void load_default_palette();
	creaturesImage& get_image(std::string name, ImageType allowed_types);

  private:
	std::shared_ptr<PathManager> m_path_manager;
	std::map<std::string, creaturesImage> m_cache;
	shared_array<Color> m_default_palette;
};