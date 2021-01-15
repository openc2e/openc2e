#include "fileformats/mngparser.h"
#include "creaturesException.h"
#include "utils/overload.h"
#include <assert.h>
#include <unordered_set>

using namespace mngtoktype;

struct MNGParserState {
	MNGParserState(std::vector<mngtoken> tokens_) : tokens(tokens_) {}
	std::vector<mngtoken> tokens;
	size_t p = 0;
	
	mngtoken peek();
	toktype peektype();
	mngtoken consume();
	mngtoken consume(toktype type);
	
	MNGScript parse_script();
	std::pair<std::string, float> parse_variable_declaration();
	MNGEffect parse_effect();
	MNGStage parse_stage();
	MNGTrack parse_track();
	MNGAleotoricLayer parse_aleotoric_layer();
	MNGVoice parse_voice();
	MNGLoopLayer parse_loop_layer();
	std::vector<MNGUpdate> parse_update_block();
	MNGExpression parse_expression();
	
	float parse_constant_holder(toktype type);
	std::string parse_ident_holder(toktype type);
	MNGExpression parse_expression_holder(toktype type);
};

mngtoken MNGParserState::peek() {
	while (tokens[p].type == MNG_WHITESPACE
		|| tokens[p].type == MNG_NEWLINE
		|| tokens[p].type == MNG_COMMENT
	) {
		p++;
	}
	return tokens[p];
}
toktype MNGParserState::peektype() {
	return peek().type;
}
mngtoken MNGParserState::consume() {
	return tokens[p++];
}
mngtoken MNGParserState::consume(toktype type) {
	if (peektype() == type) {
		return consume();
	} else {
		// TODO: better error reporting
		printf("Unexpected token: %s\n", peek().dump().c_str());
		printf("Expected: %s\n", mngtoktype::to_string(type).c_str());
		printf("Around:\n");
		for (int i = -5; i < 5; ++i) {
			if (i >= 0 && p + i < tokens.size()) {
				printf("%s\n", tokens[p + i].dump().c_str());
			}
		}
		throw creaturesException("Unexpected token: " + peek().dump());
	}
}

std::pair<std::string, float> MNGParserState::parse_variable_declaration() {
	consume(MNG_VARIABLE);
	consume(MNG_LPAREN);
	std::string name = consume(MNG_CONST_NAME).value;
	consume(MNG_COMMA);
	float value = std::stof(consume(MNG_CONST_NUMBER).value);
	consume(MNG_RPAREN);
	return {name, value};
}

MNGExpression MNGParserState::parse_expression() {
	#define BINARY_OP(type) \
		if (peektype() == type) { \
			consume(type); \
			consume(MNG_LPAREN); \
			auto left = parse_expression(); \
			consume(MNG_COMMA); \
			auto right = parse_expression(); \
			consume(MNG_RPAREN); \
			return MNGFunction(type, left, right); \
		}
	
	BINARY_OP(MNG_ADD);
	BINARY_OP(MNG_SUBTRACT);
	BINARY_OP(MNG_MULTIPLY);
	BINARY_OP(MNG_DIVIDE);
	BINARY_OP(MNG_RANDOM);
	BINARY_OP(MNG_SINEWAVE);
	BINARY_OP(MNG_COSINEWAVE);
	
	if (peektype() == MNG_CONST_NUMBER) {
		float value = std::stof(consume(MNG_CONST_NUMBER).value);
		return value;
	}
	if (peektype() == MNG_CONST_NAME) {
		return consume(MNG_CONST_NAME).value;
	}
	if (peektype() == MNG_INTERVAL) {
		consume(MNG_INTERVAL);
		return std::string("Interval");
	}
	if (peektype() == MNG_VOLUME) {
		consume(MNG_VOLUME);
		return std::string("Volume");
	}
	if (peektype() == MNG_PAN) {
		consume(MNG_PAN);
		return std::string("Pan");
	}
	throw creaturesException("Unexpected token when parsing expression: " + peek().dump());
}

float MNGParserState::parse_constant_holder(toktype type) {
	consume(type);
	consume(MNG_LPAREN);
	float value = std::stof(consume(MNG_CONST_NUMBER).value);
	consume(MNG_RPAREN);
	return value;
}

std::string MNGParserState::parse_ident_holder(toktype type) {
	consume(type);
	consume(MNG_LPAREN);
	std::string value = consume(MNG_CONST_NAME).value;
	consume(MNG_RPAREN);
	return value;
}

MNGExpression MNGParserState::parse_expression_holder(toktype type) {
	consume(type);
	consume(MNG_LPAREN);
	auto value = parse_expression();
	consume(MNG_RPAREN);
	return value;
}

MNGStage MNGParserState::parse_stage() {
	MNGStage stage;
	consume(MNG_STAGE);
	consume(MNG_LCURLY);
	while (true) {
		auto type = peektype();
		if (type == MNG_RCURLY) {
			break;
		}
		else if (type == MNG_PAN) {
			stage.pan = parse_expression_holder(MNG_PAN);
			// TODO: error if pan defined twice?
		}
		else if (type == MNG_VOLUME) {
			stage.volume = parse_expression_holder(MNG_VOLUME);
			// TODO: error if volume defined twice?
		}
		else if (type == MNG_DELAY) {
			stage.delay = parse_expression_holder(MNG_DELAY);
			// TODO: error if delay defined twice?
			// TODO: error if delay already defined?
		}
		else if (type == MNG_TEMPODELAY) {
			stage.tempodelay = parse_expression_holder(MNG_TEMPODELAY);
			// TODO: error if tempodelay defined twice?
			// TODO: error if delay already defined?
		}
		else {
			throw creaturesException("Unexpected token when parsing stage: " + peek().dump());
		}
	}
	consume(MNG_RCURLY);
	return stage;
}

MNGEffect MNGParserState::parse_effect() {
	MNGEffect effect;
	consume(MNG_EFFECT);
	consume(MNG_LPAREN);
	effect.name = consume(MNG_CONST_NAME).value;
	consume(MNG_RPAREN);
	consume(MNG_LCURLY);
	while (true) {
		auto type = peektype();
		if (type == MNG_RCURLY) {
			break;
		}
		else if (type == MNG_STAGE) {
			effect.stages.push_back(parse_stage());
		}
		else {
			throw creaturesException("Unexpected token when parsing effect: " + peek().dump());
		}
	}
	consume(MNG_RCURLY);
	return effect;
}

std::vector<MNGUpdate> MNGParserState::parse_update_block() {
	std::vector<MNGUpdate> updates;
	consume(MNG_UPDATE);
	consume(MNG_LCURLY);
	while (true) {
		auto type = peektype();
		if (type == MNG_RCURLY) {
			break;
		}
		else if (type == MNG_CONST_NAME || type == MNG_VOLUME || type == MNG_INTERVAL || type == MNG_PAN) {
			std::string name = consume().value;
			assert(name.size());
			consume(MNG_EQUALS);
			auto expr = parse_expression();
			updates.push_back({name, expr});
			// TODO: should require a newline after this?
		}
		else {
			throw creaturesException("Unexpected token when parsing update block: " + peek().dump());
		}
	}
	consume(MNG_RCURLY);
	return updates;	
}

MNGVoice MNGParserState::parse_voice() {
	MNGVoice voice;
	consume(MNG_VOICE);
	consume(MNG_LCURLY);
	while (true) {
		auto type = peektype();
		if (type == MNG_RCURLY) {
			break;
		}
		else if (type == MNG_CONDITION) {
			consume(MNG_CONDITION);
			consume(MNG_LPAREN);
			auto variable = consume(MNG_CONST_NAME).value;
			consume(MNG_COMMA);
			auto left = std::stof(consume(MNG_CONST_NUMBER).value);
			consume(MNG_COMMA);
			auto right = std::stof(consume(MNG_CONST_NUMBER).value);
			consume(MNG_RPAREN);
			voice.conditions.push_back({variable, left, right});
			// TODO: are multiple conditions actually allowed?
		}
		else if (type == MNG_EFFECT) {
			voice.effect = parse_ident_holder(MNG_EFFECT);
			// TODO: error if multiple effects defined
		}
		else if (type == MNG_WAVE) {
			voice.wave = parse_ident_holder(MNG_WAVE);
			// TODO: error if multiple waves defined
		}
		else if (type == MNG_INTERVAL) {
			consume(MNG_INTERVAL);
			consume(MNG_LPAREN);
			voice.interval = parse_expression();
			consume(MNG_RPAREN);
			// TODO: error if multiple voices defined
		}
		else if (type == MNG_UPDATE) {
			auto updates = parse_update_block();
			voice.updates.insert(voice.updates.end(), updates.begin(), updates.end());
		}
		else {
			throw creaturesException("Unexpected token when parsing voice: " + peek().dump());
		}
	}
	consume(MNG_RCURLY);
	// TODO: error if no wave defined
	return voice;
}

MNGAleotoricLayer MNGParserState::parse_aleotoric_layer() {
	MNGAleotoricLayer aleotoriclayer;
	consume(MNG_ALEOTORICLAYER);
	consume(MNG_LPAREN);
	aleotoriclayer.name = consume(MNG_CONST_NAME).value;
	consume(MNG_RPAREN);
	consume(MNG_LCURLY);
	while (true) {
		auto type = peektype();
		if (type == MNG_RCURLY) {
			break;
		}
		else if (type == MNG_EFFECT) {
			aleotoriclayer.effect = parse_ident_holder(MNG_EFFECT);
		}
		else if (type == MNG_VARIABLE) {
			aleotoriclayer.variables.insert(parse_variable_declaration());
		}
		else if (type == MNG_VOLUME) {
			aleotoriclayer.volume = parse_constant_holder(MNG_VOLUME);
		}
		else if (type == MNG_UPDATE) {
			auto updates = parse_update_block();
			aleotoriclayer.updates.insert(aleotoriclayer.updates.end(), updates.begin(), updates.end());
		}
		else if (type == MNG_INTERVAL) {
			aleotoriclayer.interval = parse_constant_holder(MNG_INTERVAL);
		}
		else if (type == MNG_UPDATERATE) {
			aleotoriclayer.updaterate = parse_constant_holder(MNG_UPDATERATE);
		} else if (type == MNG_BEATSYNCH) {
			aleotoriclayer.beatsynch = parse_constant_holder(MNG_BEATSYNCH);
		}
		else if (type == MNG_VOICE) {
			aleotoriclayer.voices.push_back(parse_voice());
		}
		else {
			throw creaturesException("Unexpected token when parsing aleotoriclayer: " + peek().dump());
		}
	}
	consume(MNG_RCURLY);
	return aleotoriclayer;
}

MNGLoopLayer MNGParserState::parse_loop_layer() {
	MNGLoopLayer looplayer;
	consume(MNG_LOOPLAYER);
	consume(MNG_LPAREN);
	looplayer.name = consume(MNG_CONST_NAME).value;
	consume(MNG_RPAREN);
	consume(MNG_LCURLY);
	while (true) {
		auto type = peektype();
		if (type == MNG_RCURLY) {
			break;
		}
		else if (type == MNG_VARIABLE) {
			looplayer.variables.insert(parse_variable_declaration());
			// TODO: error if variable defined twice?
		}
		else if (type == MNG_UPDATERATE) {
			looplayer.updaterate = parse_constant_holder(MNG_UPDATERATE);
		}
		else if (type == MNG_WAVE) {
			looplayer.wave = parse_ident_holder(MNG_WAVE);
		}
		else if (type == MNG_UPDATE) {
			auto updates = parse_update_block();
			looplayer.updates.insert(looplayer.updates.end(), updates.begin(), updates.end());
		}
		else {
			throw creaturesException("Unexpected token when parsing looplayer: " + peek().dump());
		}
	}
	consume(MNG_RCURLY);
	return looplayer;
}

MNGTrack MNGParserState::parse_track() {
	MNGTrack track;
	consume(MNG_TRACK);
	consume(MNG_LPAREN);
	track.name = consume(MNG_CONST_NAME).value;
	consume(MNG_RPAREN);
	consume(MNG_LCURLY);
	while (true) {
		auto type = peektype();
		if (type == MNG_RCURLY) {
			break;
		}
		else if (type == MNG_FADEIN) {
			track.fadein = parse_constant_holder(MNG_FADEIN);
			// TODO: error if fadein defined twice?
		}
		else if (type == MNG_FADEOUT) {
			track.fadeout = parse_constant_holder(MNG_FADEOUT);
			// TODO: error if fadeout defined twice?
		}
		else if (type == MNG_VOLUME) {
			track.volume = parse_constant_holder(MNG_VOLUME);
			// TODO: error if volume defined twice?
		} else if (type == MNG_BEATLENGTH) {
			track.beatlength = parse_constant_holder(MNG_BEATLENGTH);
			// TODO: error if beatlength defined twice?
		}
		else if (type == MNG_LOOPLAYER) {
			track.layers.push_back(parse_loop_layer());
			// TODO: error if a layer is defined twice?
		}
		else if (type == MNG_ALEOTORICLAYER) {
			track.layers.push_back(parse_aleotoric_layer());
			// TODO: error if a layer is defined twice?
		}
		else {
			throw creaturesException("Unexpected token when parsing track: " + peek().dump());
		}
	}
	consume(MNG_RCURLY);
	return track;
}

MNGScript MNGParserState::parse_script() {
	MNGScript script;
	
	// parse variables and effects and tracks
	while (true) {
		auto type = peektype();
		if (type == MNG_EOI) {
			return script;
		}
		else if (type == MNG_VARIABLE) {
			// TODO: does managerVar actually do anything?
			consume(MNG_VARIABLE);
			consume(MNG_LPAREN);
			std::string name = consume(MNG_CONST_NAME).value;
			if (name != "managerVar") {
				throw creaturesException("Unexpected toplevel variable declaration '" + name + "'");
			}
			consume(MNG_COMMA);
			float value = std::stof(consume(MNG_CONST_NUMBER).value);
			if (value != 0) {
				throw creaturesException("Toplevel variable declaration 'managerVar' not 0.0");
			}
			consume(MNG_RPAREN);
		}
		else if (type == MNG_EFFECT) {
			script.effects.push_back(parse_effect());
			// TODO: error if effect defined twice?
		}
		else if (type == MNG_TRACK) {
			script.tracks.push_back(parse_track());
			// TODO: error if track defined twice?
		}
		else {
			throw creaturesException("Unexpected token " + peek().dump());
		}
	}
}

MNGScript mngparse(const std::string& script) {
	return mngparse(script.c_str());
}
MNGScript mngparse(const char *script) {
	return mngparse(mnglex(script));
}
MNGScript mngparse(const std::vector<mngtoken>& tokens) {
	MNGParserState state(tokens);
	return state.parse_script();
}

std::vector<mngtoken> mnglex(const std::string& script) {
	return mnglex(script.c_str());
}

std::vector<mngtoken> mnglex(const char *p) {
	std::vector<mngtoken> v;
	const char *basep;
	const char *YYMARKER = NULL;
	assert(p);

start:
	basep = p;

#define push_token(type) v.push_back(mngtoken(type)); goto start;
#define push_value(type) v.push_back(mngtoken(type, std::string(basep, p - basep))); goto start;

/*!re2c
	re2c:define:YYCTYPE = "unsigned char";
	re2c:define:YYCURSOR = p;
	re2c:yyfill:enable = 0;
	re2c:yych:conversion = 1;
	re2c:indent:top = 1;

	eoi = [\000];

    eoi { goto eoi; }
    ("\r\n" | "\n") { push_value(MNG_NEWLINE); }
    [ \t\r]+    { push_value(MNG_WHITESPACE); }
    "Variable"  { push_token(MNG_VARIABLE); }
    "Effect"    { push_token(MNG_EFFECT); }
    "Track"     { push_token(MNG_TRACK); }
    "Stage"     { push_token(MNG_STAGE); }
    "Pan"       { push_value(MNG_PAN); }
    "Volume"    { push_value(MNG_VOLUME); }
    "Delay"     { push_token(MNG_DELAY); }
    "TempoDelay" { push_token(MNG_TEMPODELAY); }
    "Random"    { push_token(MNG_RANDOM); }
    "FadeIn"    { push_token(MNG_FADEIN); }
    "FadeOut"   { push_token(MNG_FADEOUT); }
    "BeatLength" { push_token(MNG_BEATLENGTH); }
    "AleotoricLayer" { push_token(MNG_ALEOTORICLAYER); }
    "LoopLayer" { push_token(MNG_LOOPLAYER); }
    "Update"    { push_token(MNG_UPDATE); }
    "Add"       { push_token(MNG_ADD); }
    "Subtract"  { push_token(MNG_SUBTRACT); }
    "Multiply"  { push_token(MNG_MULTIPLY); }
    "Divide"    { push_token(MNG_DIVIDE); }
    "SineWave"  { push_token(MNG_SINEWAVE); }
    "CosineWave" { push_token(MNG_COSINEWAVE); }
    "Voice"     { push_token(MNG_VOICE); }
    "Interval"  { push_value(MNG_INTERVAL); }
    "Condition" { push_token(MNG_CONDITION); }
    "BeatSynch" { push_token(MNG_BEATSYNCH); }
    "UpdateRate" { push_token(MNG_UPDATERATE); }
    "Wave"      { push_token(MNG_WAVE); }
    [A-Za-z]([A-Za-z0-9])* { push_value(MNG_CONST_NAME); }
    [-]?[0-9]+([.][0-9]+)? { push_value(MNG_CONST_NUMBER); }
    "(" { push_token(MNG_LPAREN); }
    ")" { push_token(MNG_RPAREN); }
    "{" { push_token(MNG_LCURLY); }
    "}" { push_token(MNG_RCURLY); }
    "," { push_token(MNG_COMMA); }
    "=" { push_token(MNG_EQUALS); }
    "//"[^\r\n\000]* { push_value(MNG_COMMENT); }
    . { push_value(MNG_ERROR); }
*/

eoi:
	v.push_back(mngtoken(MNG_EOI));
	return v;
}

std::string mngtoktype::to_string(mngtoktype::toktype type) {
	switch (type) {
		case MNG_VARIABLE:
			return "variable";
		case MNG_EFFECT:
			return "effect";
		case MNG_TRACK:
			return "track";
		case MNG_STAGE:
			return "stage";
		case MNG_PAN:
			return "pan";
		case MNG_VOLUME:
			return "volume";
		case MNG_DELAY:
			return "delay";
		case MNG_TEMPODELAY:
			return "tempodelay";
		case MNG_RANDOM:
			return "random";
		case MNG_FADEIN:
			return "fadein";
		case MNG_FADEOUT:
			return "fadeout";
		case MNG_BEATLENGTH:
			return "beatlength";
		case MNG_ALEOTORICLAYER:
			return "aleotoriclayer";
		case MNG_LOOPLAYER:
			return "looplayer";
		case MNG_UPDATE:
			return "update";
		case MNG_ADD:
			return "add";
		case MNG_SUBTRACT:
			return "subtract";
		case MNG_MULTIPLY:
			return "multiply";
		case MNG_DIVIDE:
			return "divide";
		case MNG_SINEWAVE:
			return "sinewave";
		case MNG_COSINEWAVE:
			return "cosinewave";
		case MNG_VOICE:
			return "voice";
		case MNG_INTERVAL:
			return "interval";
		case MNG_CONDITION:
			return "condition";
		case MNG_BEATSYNCH:
			return "beatsynch";
		case MNG_UPDATERATE:
			return "updaterate";
		case MNG_WAVE:
			return "wave";
		case MNG_CONST_NUMBER:
			return "const_number";
		case MNG_CONST_NAME:
			return "const_name";
		case MNG_COMMENT:
			return "comment";
		case MNG_LPAREN:
			return "lparen";
		case MNG_RPAREN:
			return "rparen";
		case MNG_LCURLY:
			return "lcurly";
		case MNG_RCURLY:
			return "rcurly";
		case MNG_COMMA:
			return "comma";
		case MNG_EQUALS:
			return "equals";
		case MNG_WHITESPACE:
			return "whitespace";
		case MNG_NEWLINE:
			return "newline";
		case MNG_EOI:
			return "eoi";
		case MNG_ERROR:
			return "error";
	}
}

std::string mngtoken::dump() {
	if (type == MNG_CONST_NAME) {
		return mngtoktype::to_string(type) + " " + value;
	}
	if (type == MNG_CONST_NUMBER) {
		return mngtoktype::to_string(type) + " " + value;
	}
	return mngtoktype::to_string(type);
}

std::vector<std::string> MNGScript::getWaveNames() const {
	std::vector<std::string> names;
	std::unordered_set<std::string> seen;
	for (auto t : tracks) {
		for (auto& l : t.layers) {
			visit(overload(
				[&](const MNGAleotoricLayer& al) {
					for (auto v : al.voices) {
						if (seen.find(v.wave) == seen.end()) {
							seen.insert(v.wave);
							names.push_back(v.wave);
						}
					}
				},
				[&](const MNGLoopLayer& ll) {
					if (seen.find(ll.wave) == seen.end()) {
						seen.insert(ll.wave);
						names.push_back(ll.wave);
					}
				}
			), l);
		}
	}
	return names;
}