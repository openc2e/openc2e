#pragma once

#include "common/EntityPool.h"
#include "openc2e-core/creaturesImage.h"

#include <fmt/core.h>
#include <string>
#include <vector>

class Renderable {
  public:
	int x = 0;
	int y = 0;
	int z = 0;
	int object_sprite_base = 0;
	int part_sprite_base = 0;
	int sprite_index = 0;
	creaturesImage sprite;
	bool has_animation = false;
	unsigned int animation_frame = 0; // only if has_animation is true
	std::string animation_string; // only if has_animation is true

	int32_t frame() const {
		return object_sprite_base + part_sprite_base + sprite_index;
	}

	int32_t height() const {
		return sprite.height(frame());
	}

	void clear_animation() {
		has_animation = false;
		animation_frame = 0;
		animation_string = {};
	}
};

class RenderableManager {
  private:
	bool m_needs_sorting;
	EntityPool<Renderable> m_pool;

  public:
	using Handle = decltype(m_pool)::Id;

  public:
	Handle add(Renderable e) {
		m_needs_sorting = true;
		return m_pool.add(std::move(e));
	}

	Renderable* try_get(Handle handle) {
		return m_pool.try_get(handle);
	}

	void tick() {
		for (auto& r : m_pool) {
			if (!r.has_animation) {
				continue;
			}
			if (r.animation_frame >= r.animation_string.size()) {
				// already done
				// TODO: are we on the correct frame already?
				// TODO: clear animation?
				r.has_animation = false;
				r.animation_string = {};
				r.animation_frame = 0;
				continue;
			}
			r.animation_frame += 1;
			if (r.animation_frame >= r.animation_string.size()) {
				// done!
				continue;
			}
			if (r.animation_string[r.animation_frame] == 'R') {
				r.animation_frame = 0;
			}

			// TODO: assert isdigit
			r.sprite_index = r.animation_string[r.animation_frame] - '0';
		}
	}

	auto iter_zorder() {
		if (m_needs_sorting) {
			fmt::print("* [RenderableManager] Resorting entities by z-order\n");
			m_pool.stable_sort([](const Renderable& left, const Renderable& right) { return left.z < right.z; });
			m_needs_sorting = false;
		}
		return m_pool;
	}

	bool contains(Handle handle) const {
		return m_pool.contains(handle);
	}
};

using RenderableHandle = RenderableManager::Handle;