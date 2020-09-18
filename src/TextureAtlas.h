#pragma once

#include <memory>

class TextureAtlas {
public:
  virtual ~TextureAtlas() = default;
protected:
  TextureAtlas() = default;
};
using TextureAtlasHandle = std::shared_ptr<TextureAtlas>;