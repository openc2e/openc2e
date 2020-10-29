#include "CreatureGrapher.h"

#include <array>
#include "ImGuiUtils.h"
#include "World.h"
#include "creatures/CreatureAgent.h"
#include "creatures/c2eCreature.h"
#include "creatures/oldCreature.h"
#include "utils/enumerate.h"
#include "utils/range.h"
#include "ChemicalNamesManager.h"
#include <imgui.h>

namespace Openc2eImgui {

static bool s_creature_grapher_open = false;

static ChemicalNamesManager s_chemical_names_manager;

void SetCreatureGrapherOpen(bool value) {
  s_creature_grapher_open = value;
}

void DrawCreatureGrapher() {
  if (ImGuiUtils::BeginWindow("Creature Grapher", &s_creature_grapher_open)) {
    
    static unsigned int s_last_tickcount = 0;
    static CreatureAgent* s_selected_creature = nullptr;
    static std::array<std::array<float, 200>, 256> s_data;
    static size_t s_data_position = 0;
    static size_t s_selected_chemical_group = 0;
    static int s_selected_chemical = 0;
    
    if (world.tickcount != s_last_tickcount) {
      s_last_tickcount = world.tickcount;
      // TODO: we should only update on biochem ticks..
      
      if (dynamic_cast<CreatureAgent *>(world.selectedcreature.get()) != s_selected_creature) {
        s_data = {};
        s_selected_creature = dynamic_cast<CreatureAgent *>(world.selectedcreature.get());
      }
      if (s_selected_creature) {
      	c2eCreature *cc = dynamic_cast<c2eCreature *>(s_selected_creature->getCreature());
      	if (cc) {
          for (auto i : range(256)) {
            s_data[i][s_data_position] = cc->getChemical(i);
          }
      	}
      	oldCreature *oc = dynamic_cast<oldCreature *>(s_selected_creature->getCreature());
      	if (oc) {
          for (auto i : range(256)) {
            s_data[i][s_data_position] = oc->getChemical(i);
          }
      	}
        s_data_position = (s_data_position + 1) % s_data[0].size();
      }
    }
    
    std::string overlay_text = "Chemical: " + s_chemical_names_manager.getChemicalNames()[s_selected_chemical];
    ImGui::PlotLines(
      "##Creature Grapher Graph",
      s_data[s_selected_chemical].data(),
      s_data[s_selected_chemical].size(),
      s_data_position,
      overlay_text.c_str(),
      -1,
      1,
      ImVec2(320, 200)
    );
  
    float content_width = ImGui::GetWindowContentRegionWidth();
  
    if (ImGui::ListBoxHeader("##Creature Grapher - Chemical Groups", ImVec2(content_width / 2, 0))) {
      for (auto g : enumerate(s_chemical_names_manager.getChemicalGroups())) {
        if (ImGui::Selectable(g->first.c_str(), g.i == s_selected_chemical_group)) {
          s_selected_chemical_group = g.i;
        }
      }
      ImGui::ListBoxFooter();
    }
    
    ImGui::SameLine();
    
    
    if (ImGui::ListBoxHeader("##Creature Grapher - Chemicals", ImVec2(content_width / 2, 0))) {
      auto &group = s_chemical_names_manager.getChemicalGroups()[s_selected_chemical_group];
      auto &chemical_names = s_chemical_names_manager.getChemicalNames();
      
      for (auto chemid : group.second) {
        if (ImGui::Selectable(chemical_names[chemid].c_str(), chemid == s_selected_chemical)) {
          s_selected_chemical = chemid;
        }
      }
      
      ImGui::ListBoxFooter();
    }
  
    ImGui::End();
  }
}

} // namespace Openc2eImgui