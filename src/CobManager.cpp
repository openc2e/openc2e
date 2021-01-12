#include "CobManager.h"

#include "Engine.h"
#include "PathResolver.h"
#include "World.h"
#include "creaturesException.h"
#include "fileformats/c1cobfile.h"
#include "fileformats/c2cobfile.h"
#include "utils/ascii_tolower.h"
#include "utils/find_if.h"

#include <algorithm>
#include <fmt/format.h>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

void CobManager::update() {
	objects.clear();

	std::string directory = "";
	if (engine.version == 2) {
		directory = "Objects";
	}
	for (auto cob : world.findFiles(directory, "*.cob")) {
		if (engine.version == 1) {
			c1cobfile cobfile = read_c1cobfile(cob);
			objects.emplace_back(cobfile.name, cob);
			if (world.findFile(fs::path(cob).stem().string() + ".rcb").size()) {
				objects.back().is_removable = true;
			}
		} else if (engine.version == 2) {
			c2cobfile cobfile(cob);

			for (auto b : cobfile.blocks) {
				if (b->type != "agnt")
					continue;
				cobAgentBlock a(b);
				objects.emplace_back(a.name, cob);
				if (a.removescript.size() > 0) {
					objects.back().is_removable = true;
				}
			}
		}
	}

	std::sort(objects.begin(), objects.end(), [](auto& a, auto& b) { return a.name < b.name; });
}

Image CobManager::getPicture(const CobFileInfo& info) {
	if (engine.version == 1) {
		c1cobfile cobfile = read_c1cobfile(info.filename);

		if (cobfile.picture.width > 0 && cobfile.picture.height > 0) {
			// TODO: don't require casting
			return cobfile.picture;
		}
	} else if (engine.version == 2) {
		c2cobfile cobfile(info.filename);

		for (auto b : cobfile.blocks) {
			if (b->type != "agnt")
				continue;
			cobAgentBlock a(b);
			if (a.name != info.name)
				continue;
			return a.thumbnail;
		}
	}
	return {};
}

void CobManager::inject(const CobFileInfo& info) {
	std::string idata;
	if (engine.version == 1) {
		c1cobfile cobfile = read_c1cobfile(info.filename);
		for (auto s : cobfile.object_scripts) {
			idata += s + "\n";
		}
		for (auto s : cobfile.install_scripts) {
			idata += "iscr," + s + "\n";
		}
	} else if (engine.version == 2) {
		c2cobfile cobfile(info.filename);
		auto block = find_if(cobfile.blocks, [&](auto& b) {
			return b->type == "agnt" && cobAgentBlock(b).name == info.name;
		});
		if (!block) {
			throw creaturesException(fmt::format("Couldn't find agent {}", info.name));
		}
		cobAgentBlock a(*block);

		// dependencies
		assert(a.deptypes.size() == a.depnames.size());
		for (size_t i = 0; i < a.deptypes.size(); i++) {
			int deptype = a.deptypes[i];
			std::string depname = a.depnames[i];

			fs::path resourcedir;
			switch (deptype) {
				case 0: resourcedir = "Images/"; break;
				case 1: resourcedir = "Sounds/"; break;
				default:
					throw creaturesException("Unknown dependency type " + std::to_string(deptype));
			}

			if (world.findFile(resourcedir / depname).size())
				continue; // TODO: update file if necessary?

			auto depBlock = find_if(cobfile.blocks, [&](auto& b) {
				return b->type == "file" && cobFileBlock(b).filetype == deptype && cobFileBlock(b).filename == depname;
			});
			if (!depBlock) {
				throw creaturesException(fmt::format("Couldn't find dependency {} (type {})", depname, deptype));
			}
			cobFileBlock f(*depBlock);

			fs::path dir = fs::path(world.getUserDataDir()) / resourcedir;
			if (!fs::exists(dir)) {
				fs::create_directory(dir);
			}
			assert(fs::exists(dir) && fs::is_directory(dir)); // TODO: error handling

			std::string outputfile = dir / depname;
			assert(!fs::exists(outputfile));

			std::ofstream output(outputfile, std::ios::binary);
			output.write((char*)f.getFileContents(), f.filesize);
		}

		for (auto s : a.scripts) {
			idata += s + "\n";
		}
		idata += "iscr," + a.installscript + "\n";
	}

	// idata += "rscr\n";
	std::string result = engine.executeNetwork(idata);
	if (result.size()) {
		fmt::print("warning: injection returned data (error?): {}\n", result);
	}
}

void CobManager::remove(const CobFileInfo& info) {
	std::string rdata;
	if (engine.version == 1) {
		std::string rcbpath = world.findFile(fs::path(info.filename).stem().string() + ".rcb");
		c1cobfile cobfile = read_c1cobfile(rcbpath);
		for (auto s : cobfile.install_scripts) {
			rdata += s + "\n";
		}

	} else if (engine.version == 2) {
		c2cobfile cobfile(info.filename);
		auto block = find_if(cobfile.blocks, [&](auto& b) {
			return b->type == "agnt" && cobAgentBlock(b).name == info.name;
		});
		if (!block) {
			throw creaturesException(fmt::format("Couldn't find agent {}", info.name));
		}
		cobAgentBlock a(*block);

		rdata = a.removescript;
	}

	rdata += "\nrscr\n";
	std::string result = engine.executeNetwork(rdata);
	if (result.size()) {
		fmt::print("warning: removal returned data (error?): {}\n", result);
	}
}