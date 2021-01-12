#include "BrainViewer.h"

#include <imgui.h>
#include <fmt/core.h>
#include "ImGuiUtils.h"
#include "World.h"

#include "creatures/CreatureAgent.h"
#include "creatures/c2eCreature.h"
#include "creatures/oldCreature.h"
#include "creatures/c2eBrain.h"
#include "creatures/oldBrain.h"

namespace Openc2eImgui {

static bool s_brain_viewer_open = false;

void SetBrainViewerOpen(bool value) {
  s_brain_viewer_open = value;
}

static const int neuron_var = 0;
static const int dendrite_var = 0;
static const float threshold = -1000.0f;

static ImVec2 GetBrainViewSize(Creature *creature) {
	
	int neededwidth = 2, neededheight = 2;
	
	if (c2eCreature *c = dynamic_cast<c2eCreature *>(creature)) {
		c2eBrain *b = c->getBrain();
		assert(b);

		for (auto & i : b->lobes) {
			c2eBrainLobeGene *lobe = i.second->getGene();
			int this_x = lobe->x + lobe->width;
			int this_y = lobe->y + lobe->height;
			if (this_x > neededwidth)
				neededwidth = this_x;
			if (this_y > neededheight)
				neededheight = this_y;
		}
	} else if (oldCreature *oc = dynamic_cast<oldCreature *>(creature)) {
		oldBrain *b = oc->getBrain();
		assert(b);
		
		for (auto & i : b->lobes) {
			oldBrainLobeGene *lobe = i->getGene();
			int this_x = lobe->x + i->getWidth();
			int this_y = lobe->y + i->getHeight();
			if (this_x > neededwidth)
				neededwidth = this_x;
			if (this_y > neededheight)
				neededheight = this_y;
		}
	} else {
    return ImVec2(200, 400);
  }

	return ImVec2((neededwidth + 6) * 20, (neededheight + 5) * 20);
}

static void drawLobeBoundaries(unsigned int x, unsigned int y, unsigned int width, unsigned int height, std::string text) {
  ImDrawList* drawlist = ImGui::GetWindowDrawList();
  ImVec2 cur = ImGui::GetCursorScreenPos();
  
  ImVec2 p((x + 4) * 20, (y + 4) * 20);
  
  // TODO: color?
  drawlist->AddRect(cur + p, cur + p + ImVec2(width * 20, height * 20), IM_COL32_WHITE);
  drawlist->AddText(cur + p - ImVec2(0, 20), IM_COL32_WHITE, text.c_str());
}

static std::string niceNameForOldLobe(unsigned int id) {
	switch (id) {
		case 0: return "percept";
		case 1: return "drive";
		case 2: return "stim source";
		case 3: return "verb";
		case 4: return "noun";
		case 5: return "sensory";
		case 6: return "decision";
		case 7: return "attention";
		case 8: return "concept";
	}
	return fmt::format("lobe {}", id);
}

static void drawOldBrain(oldBrain *b) {
  assert(b);
  
  ImDrawList* drawlist = ImGui::GetWindowDrawList();
  ImVec2 cur = ImGui::GetCursorScreenPos();
  
  std::map<oldNeuron *, std::pair<unsigned int, unsigned int> > neuroncoords;

  unsigned int id = 0;
  for (auto & i : b->lobes) {
    oldBrainLobeGene *lobe = i->getGene();
    drawLobeBoundaries(lobe->x, lobe->y, i->getWidth(), i->getHeight(), niceNameForOldLobe(id));
    id++;

    for (unsigned int y = 0; y < i->getHeight(); y++) {
      for (unsigned int x = 0; x < i->getWidth(); x++) {
        unsigned int neuronid = x + (y * i->getWidth());
        oldNeuron *neuron = i->getNeuron(neuronid);
        
        int lobex = (lobe->x + 4) * 20;
        int lobey = (lobe->y + 4) * 20;
  
        // store the centre coordinate for drawing dendrites
        assert(neuroncoords.find(neuron) == neuroncoords.end());
        neuroncoords[neuron] = std::pair<unsigned int, unsigned int>(lobex + (x * 20) + 10, lobey + (y * 20) + 10);
        
        // if below threshold, don't draw
        float var = neuron->output / 255.0f; // TODO: allow choosing of state too?
        // TODO: muh
        if (threshold == 0.0f) {
          if (var == threshold) continue;
        } else {
          if (var <= threshold) continue;
        }

        // calculate appropriate colour
        float multiplier = 0.5 + (var / 2.2);
        ImU32 color = IM_COL32(255 * multiplier, 255 * multiplier, 255 * multiplier, 255);
    
        // draw neuron
        ImVec2 p(lobex + (x * 20) + 6, lobey + (y * 20) + 6);
        drawlist->AddRect(cur + p, cur + p + ImVec2(8, 8), color);
      }
    }	
  }

  for (auto & lobe : b->lobes) {
    for (unsigned int j = 0; j < lobe->getNoNeurons(); j++) {
      oldNeuron *dest = lobe->getNeuron(j);
      for (auto & dendrite : dest->dendrites) {
        for (std::vector<oldDendrite>::iterator d = dendrite.begin();
          d != dendrite.end(); d++) {
          oldNeuron *src = d->src;

          float var = d->strength / 255.0;
          if (threshold == 0.0f) {
            if (var == threshold) continue;
          } else {
            if (var <= threshold) continue;
          }

          float multiplier = 0.5 + (var / 2.2);
          ImU32 color = IM_COL32(multiplier * 255, multiplier * 255, multiplier * 255, 255);

          assert(neuroncoords.find(src) != neuroncoords.end());
          assert(neuroncoords.find(dest) != neuroncoords.end());
          drawlist->AddLine(
            cur + ImVec2(neuroncoords[src].first, neuroncoords[src].second),
            cur + ImVec2(neuroncoords[dest].first, neuroncoords[dest].second),
            color
          );
        }
      }
    }
  }
}

static void drawC2eBrain(c2eBrain *b) {
	assert(b);

	ImDrawList* drawlist = ImGui::GetWindowDrawList();
  ImVec2 cur = ImGui::GetCursorScreenPos();

	std::map<c2eNeuron *, std::pair<unsigned int, unsigned int> > neuroncoords;

	// draw lobes/neurons
	for (auto & i : b->lobes) {
		c2eBrainLobeGene *lobe = i.second->getGene();
		ImU32 color = IM_COL32(lobe->red, lobe->green, lobe->blue, 255);
    // TODO: set color
		drawLobeBoundaries(lobe->x, lobe->y, lobe->width, lobe->height, i.first);

		for (unsigned int y = 0; y < lobe->height; y++) {
			for (unsigned int x = 0; x < lobe->width; x++) {
				unsigned int neuronid = x + (y * lobe->width);
				c2eNeuron *neuron = i.second->getNeuron(neuronid);

				int lobex = (lobe->x + 4) * 20;
				int lobey = (lobe->y + 4) * 20;
	
				// store the centre coordinate for drawing dendrites
				assert(neuroncoords.find(neuron) == neuroncoords.end());
				neuroncoords[neuron] = std::pair<unsigned int, unsigned int>(lobex + (x * 20) + 10, lobey + (y * 20) + 10);
				
				// always highlight spare neuron
				if (i.second->getSpareNeuron() == neuronid) {
					// TODO: don't hardcode these names?
					if (i.second->getId() == "attn" || i.second->getId() == "decn" || i.second->getId() == "comb") {
            ImVec2 p(lobex + (x * 20) + 5, lobey + (y * 20) + 5);
            drawlist->AddRect(cur + p, cur + p + ImVec2(10, 10), color);
					}
				}

				// if below threshold, don't draw
				float var = neuron->variables[neuron_var];
				// TODO: muh
				if (threshold == 0.0f) {
					if (var == threshold) continue;
				} else {
					if (var <= threshold) continue;
				}

				// calculate appropriate colour
				float multiplier = 0.5 + (var / 2.2);
				ImU32 color = IM_COL32(lobe->red * multiplier, lobe->green * multiplier, lobe->blue * multiplier, 255);
		
				// draw neuron
        ImVec2 p(lobex + (x * 20) + 6, lobey + (y * 20) + 6);
				drawlist->AddRect(cur + p, cur + p + ImVec2(8, 8), color);
			}
		}
	}

	// draw dendrites
	for (std::vector<c2eTract *>::iterator i = b->tracts.begin(); i != b->tracts.end(); i++) {
		c2eBrainTractGene *tract = (*i)->getGene();
		std::string destlobename = std::string((char *)tract->destlobe, 4);
		if (b->lobes.find(destlobename) == b->lobes.end())
			continue; // can't find the source lobe, so whu? must be a bad tract
		c2eBrainLobeGene *destlobe = b->lobes[destlobename]->getGene();
		
		for (unsigned int j = 0; j < (*i)->getNoDendrites(); j++) {
			c2eDendrite *dend = (*i)->getDendrite(j);

			float var = dend->variables[dendrite_var];
			if (threshold == 0.0f) {
				if (var == threshold) continue;
			} else {
				if (var <= threshold) continue;
			}

			float multiplier = 0.5 + (var / 2.2);
			ImU32 color = IM_COL32(destlobe->red * multiplier, destlobe->green * multiplier, destlobe->blue * multiplier, 255);

			assert(neuroncoords.find(dend->source) != neuroncoords.end());
			assert(neuroncoords.find(dend->dest) != neuroncoords.end());
      drawlist->AddLine(
        cur + ImVec2(neuroncoords[dend->source].first, neuroncoords[dend->source].second),
        cur + ImVec2(neuroncoords[dend->dest].first, neuroncoords[dend->dest].second),
        color
      );
		}
	}
}

void DrawBrainViewer() {
  if (ImGuiUtils::BeginWindow("Brain Viewer", &s_brain_viewer_open, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar)) {
      CreatureAgent *creatureagent = dynamic_cast<CreatureAgent *>(world.selectedcreature.get());
      Creature *creature = creatureagent ? creatureagent->getCreature() : nullptr;
        
      if (c2eCreature *c = dynamic_cast<c2eCreature *>(creature)) {
    		drawC2eBrain(c->getBrain());
    	} else if (oldCreature *oc = dynamic_cast<oldCreature *>(creature)) {
    		drawOldBrain(oc->getBrain());
    	}
      ImGui::Dummy(GetBrainViewSize(creature));
  
      ImGui::End();
  }
}

} // namespace Openc2eImgui