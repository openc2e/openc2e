#pragma once

#include "common/Exception.h"
#include "fileformats/MFCObject.h"

#include <array>
#include <stdint.h>
#include <string>
#include <vector>

namespace sfc {

struct DendriteTypeV1 {
	// not CArchive serialized
	uint32_t source_lobe;
	uint8_t min;
	uint8_t max;
	uint8_t spread;
	uint8_t fanout;
	uint8_t min_ltw;
	uint8_t max_ltw;
	uint8_t min_strength;
	uint8_t max_strength;
	uint8_t migration;
	uint8_t relax_suscept;
	uint8_t relax_stw;
	uint8_t ltw_gain_rate;
	uint8_t gain_rate;
	uint8_t lose_rate;
	std::array<uint8_t, 10> gain_svrule;
	std::array<uint8_t, 10> lose_svrule;
	std::array<uint8_t, 10> suscept_svrule;
	std::array<uint8_t, 10> reinforce_svrule;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(source_lobe);
		ar(min);
		ar(max);
		ar(spread);
		ar(fanout);
		ar(min_ltw);
		ar(max_ltw);
		ar(min_strength);
		ar(max_strength);
		ar(migration);
		ar(relax_suscept);
		ar(relax_stw);
		ar(ltw_gain_rate);
		ar(gain_rate);
		ar(lose_rate);
		for (auto& i : gain_svrule) {
			ar(i);
		}
		for (auto& i : lose_svrule) {
			ar(i);
		}
		for (auto& i : suscept_svrule) {
			ar(i);
		}
		for (auto& i : reinforce_svrule) {
			ar(i);
		}
	}
};

struct LobeV1 {
	// not CArchive serialized
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
	uint32_t perceptible;
	uint8_t activity;
	std::array<uint8_t, 4> chemicals;
	uint8_t threshold;
	uint8_t leakage;
	uint8_t reststate;
	uint8_t inputgain;
	std::array<uint8_t, 10> svrule;
	uint8_t flags;
	DendriteTypeV1 d0_type;
	DendriteTypeV1 d1_type;
	uint32_t num_neurons;
	uint32_t num_dendrites;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(x);
		ar(y);
		ar(width);
		ar(height);
		ar(perceptible);
		ar(activity);
		for (auto& c : chemicals) {
			ar(c);
		}
		ar(threshold);
		ar(leakage);
		ar(reststate);
		ar(inputgain);
		for (auto& sv : svrule) {
			ar(sv);
		}
		ar(flags);
		d0_type.serialize(ar);
		d1_type.serialize(ar);
		ar(num_neurons);
		ar(num_dendrites);
	}
};

struct DendriteV1 {
	// not CArchive serialized
	uint32_t source_index;
	uint8_t home_x;
	uint8_t home_y;
	uint8_t suscept;
	uint8_t stw;
	uint8_t ltw;
	uint8_t strength;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(source_index);
		ar(home_x);
		ar(home_y);
		ar(suscept);
		ar(stw);
		ar(ltw);
		ar(strength);
	}
};

struct NeuronV1 {
	// not CArchive serialized
	uint8_t x;
	uint8_t y;
	uint8_t output;
	uint8_t state;
	uint8_t wta_disable;
	uint8_t exclusive;
	uint32_t dendrite0_index;
	std::vector<DendriteV1> dendrites0;
	uint32_t dendrite1_index;
	std::vector<DendriteV1> dendrites1;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(x);
		ar(y);
		ar(output);
		ar(state);
		ar(wta_disable);
		ar(exclusive);
		ar.size_u8(dendrites0);
		ar(dendrite0_index);
		for (auto& d : dendrites0) {
			d.serialize(ar);
		}
		ar.size_u8(dendrites1);
		ar(dendrite1_index);
		for (auto& d : dendrites1) {
			d.serialize(ar);
		}
	}
};

struct CBrainV1 : MFCObject {
	std::vector<LobeV1> lobes;
	std::vector<NeuronV1> neurons;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar.size_u32(lobes);
		size_t total_neurons = 0;
		for (auto& l : lobes) {
			l.serialize(ar);
			total_neurons += l.num_neurons;
		}
		if (neurons.size() == 0) {
			// we're reading, resize to the correct size
			neurons.resize(total_neurons);
		} else {
			// we're writing, make sure the numbers match up
			// TODO: should we just drop extra neurons...?
			if (neurons.size() != total_neurons) {
				throw Exception("whoops, neuron count doesn't line up");
			}
		}
		for (auto& n : neurons) {
			n.serialize(ar);
		}
	}
};

} // namespace sfc