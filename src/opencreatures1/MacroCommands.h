#pragma once

class MacroContext;

namespace MacroCommands {

void install_math_commands(MacroContext& ctx);
void install_default_commands(MacroContext& ctx);

}; // namespace MacroCommands