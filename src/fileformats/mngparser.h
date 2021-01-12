#pragma once

#include "optional.h"
#include "utils/heap_value.h"

#include <mpark/variant.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace mngtoktype {
enum toktype {
	MNG_VARIABLE,
	MNG_EFFECT,
	MNG_TRACK,
	MNG_STAGE,
	MNG_PAN,
	MNG_VOLUME,
	MNG_DELAY,
	MNG_TEMPODELAY,
	MNG_RANDOM,
	MNG_FADEIN,
	MNG_FADEOUT,
	MNG_BEATLENGTH,
	MNG_ALEOTORICLAYER,
	MNG_LOOPLAYER,
	MNG_UPDATE,
	MNG_ADD,
	MNG_SUBTRACT,
	MNG_MULTIPLY,
	MNG_DIVIDE,
	MNG_SINEWAVE,
	MNG_COSINEWAVE,
	MNG_VOICE,
	MNG_INTERVAL,
	MNG_CONDITION,
	MNG_BEATSYNCH,
	MNG_UPDATERATE,
	MNG_WAVE,
	MNG_CONST_NUMBER,
	MNG_CONST_NAME,
	MNG_COMMENT,
	MNG_LPAREN,
	MNG_RPAREN,
	MNG_LCURLY,
	MNG_RCURLY,
	MNG_COMMA,
	MNG_EQUALS,
	MNG_WHITESPACE,
	MNG_NEWLINE,
	MNG_EOI,
	MNG_ERROR
};
std::string to_string(toktype type);
} // namespace mngtoktype

struct mngtoken {
  public:
	mngtoktype::toktype type;
	std::string value;

	mngtoken(mngtoktype::toktype type_)
		: type(type_) {}
	mngtoken(mngtoktype::toktype type_, std::string value_)
		: type(type_), value(value_) {}

	std::string dump();
};

std::vector<mngtoken> mnglex(const std::string& script);
std::vector<mngtoken> mnglex(const char* script);

struct MNGScript;
MNGScript mngparse(const std::string& script);
MNGScript mngparse(const char* script);
MNGScript mngparse(const std::vector<mngtoken>& tokens);

struct MNGFunction;

using MNGExpression = mpark::variant<float, std::string, heap_value<MNGFunction>>;

struct MNGFunction {
	mngtoktype::toktype type;
	MNGExpression first;
	MNGExpression second;
	MNGFunction()
		: type(mngtoktype::MNG_ERROR) {}
	MNGFunction(mngtoktype::toktype type_, MNGExpression first_, MNGExpression second_)
		: type(type_), first(first_), second(second_) {}
};

struct MNGStage {
	optional<MNGExpression> pan;
	optional<MNGExpression> volume;
	optional<MNGExpression> delay;
	optional<MNGExpression> tempodelay;
};

struct MNGEffect {
	std::string name;
	std::vector<MNGStage> stages;
};

struct MNGCondition {
	std::string variable;
	float minimum;
	float maximum;
};

struct MNGUpdate {
	std::string variable;
	MNGExpression value;
};

struct MNGVoice {
	std::string wave;
	// TODO: are multiple conditions actually allowed?
	std::vector<MNGCondition> conditions;
	optional<std::string> effect;
	optional<MNGExpression> interval;
	std::vector<MNGUpdate> updates;
};

struct MNGAleotoricLayer {
	std::string name;
	optional<float> volume;
	optional<std::string> effect;
	optional<float> beatsynch;
	optional<float> updaterate;
	optional<float> interval;
	std::unordered_map<std::string, float> variables;
	std::vector<MNGUpdate> updates;
	std::vector<MNGVoice> voices;
};

struct MNGLoopLayer {
	std::string name;
	std::string wave;
	optional<float> volume;
	optional<float> updaterate;
	std::unordered_map<std::string, float> variables;
	std::vector<MNGUpdate> updates;
};

using MNGLayer = mpark::variant<MNGLoopLayer, MNGAleotoricLayer>;

struct MNGTrack {
	std::string name;
	optional<float> fadein;
	optional<float> fadeout;
	optional<float> beatlength;
	optional<float> volume;
	std::vector<MNGLayer> layers;
};

struct MNGScript {
	std::vector<MNGEffect> effects;
	std::vector<MNGTrack> tracks;
	std::vector<std::string> getWaveNames() const;
};