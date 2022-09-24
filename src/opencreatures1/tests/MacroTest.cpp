#include "Macro.h"

#include "MacroCommands.h"
#include "MacroContext.h"

#include <gtest/gtest.h>

TEST(Macro, UnexpectedEndOfMacro) {
	MacroContext ctx;
	Macro m;
	EXPECT_THROW(ctx.tick_macro(m), UnexpectedEndOfMacro);
}

TEST(Macro, UnknownCommand) {
	MacroContext ctx;
	Macro m{"foob"};

	EXPECT_THROW(ctx.tick_macro(m), UnknownMacroCommand);
}

TEST(Macro, VarsStartAtZero) {
	Macro m;
	for (auto var : m.vars) {
		EXPECT_EQ(var, 0);
	}
}

TEST(Macro, Math) {
	MacroContext ctx;
	MacroCommands::install_math_commands(ctx);

	Macro m{"addv var0 5,endm"};
	ctx.tick_macro(m);
	EXPECT_EQ(m.vars[0], 5);

	m = Macro{"subv var1 3,endm"};
	ctx.tick_macro(m);
	EXPECT_EQ(m.vars[1], -3);

	m = Macro{"rndv var5 8 24,endm"};
	ctx.tick_macro(m);
	EXPECT_GE(m.vars[5], 8);
	EXPECT_LE(m.vars[5], 24);
}