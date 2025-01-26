#pragma once

#include "fileformats/MFCObject.h"

#include <array>
#include <stdint.h>
#include <vector>

namespace sfc {

struct CreatureV1;

struct ChemicalDataV1 {
	// not CArchive serialized
	uint8_t concentration;
	uint8_t decay;
};

struct EmitterV1 {
	// not CArchive serialized
	uint8_t organ;
	uint8_t tissue;
	uint8_t locus;
	uint8_t chemical;
	uint8_t threshold;
	uint8_t rate;
	uint8_t gain;
	uint8_t effect;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(organ);
		ar(tissue);
		ar(locus);
		ar(chemical);
		ar(threshold);
		ar(rate);
		ar(gain);
		ar(effect);
	}
};

struct ReceptorV1 {
	// not CArchive serialized
	uint8_t organ;
	uint8_t tissue;
	uint8_t locus;
	uint8_t chemical;
	uint8_t threshold;
	uint8_t nominal;
	uint8_t gain;
	uint8_t effect;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(organ);
		ar(tissue);
		ar(locus);
		ar(chemical);
		ar(threshold);
		ar(nominal);
		ar(gain);
		ar(effect);
	}
};

struct ReactionV1 {
	// not CArchive serialized
	uint8_t r1_amount;
	uint8_t r1_chem;
	uint8_t r2_amount;
	uint8_t r2_chem;
	uint8_t rate;
	uint8_t p1_amount;
	uint8_t p1_chem;
	uint8_t p2_amount;
	uint8_t p2_chem;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(r1_amount);
		ar(r1_chem);
		ar(r2_amount);
		ar(r2_chem);
		ar(rate);
		ar(p1_amount);
		ar(p1_chem);
		ar(p2_amount);
		ar(p2_chem);
	}
};

struct CBiochemistryV1 : MFCObject {
	CreatureV1* owner = nullptr;
	std::array<ChemicalDataV1, 256> chemicals;
	std::vector<EmitterV1> emitters;
	std::vector<ReceptorV1> receptors;
	std::vector<ReactionV1> reactions;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(owner);
		ar.size_u32(emitters);
		ar.size_u32(receptors);
		ar.size_u32(reactions);
		for (auto& c : chemicals) {
			ar(c.concentration);
			ar(c.decay);
		}
		for (auto& e : emitters) {
			e.serialize(ar);
		}
		for (auto& r : receptors) {
			r.serialize(ar);
		}
		for (auto& r : reactions) {
			r.serialize(ar);
		}
	}
};

} // namespace sfc