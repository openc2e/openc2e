#ifndef SER_CAOSSCRIPT_H
#define SER_CAOSSCRIPT_H 1

#include "serialization.h"
#include "ser/s_caosVar.h"
#include "caosScript.h"
#include "ser/s_bytecode.h"
#include "ser/s_shared_str.h"

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>

BOOST_CLASS_IMPLEMENTATION(toktrace, boost::serialization::object_serializable);
BOOST_CLASS_TRACKING(toktrace, boost::serialization::track_never);
SERIALIZE(toktrace) {
	ar & obj.width & obj.lineno;
}

SERIALIZE(script) {
//	ar & obj.linked;
//	ar & obj.relocations;
//	ar & obj.labels;

	ar & obj.ops;
	ar & obj.consts;
	ar & obj.bytestrs;
	ar & obj.code;
	ar & obj.tokinfo;

//	ar & obj.fmly & obj.gnus & obj.spcs & obj.scrp;
	// dialect handled in post-serialization code
	ar & obj.filename;
//	ar & obj.gsub; // XXX duplicate with labels
}

static std::map<const Dialect *, std::shared_ptr<std::vector<std::string> > >
	dialect_trans_map;

static void make_trans_map(const Dialect *d) {
	if (dialect_trans_map[d])
		return;
	std::vector<std::string> *p = new std::vector<std::string>(d->cmdcount());
	dialect_trans_map[d] = std::shared_ptr<std::vector<std::string> >(p);
	const struct cmdinfo *ci;
	for (int i = 0; i < d->cmdcount(); i++) {
		(*p)[i] = std::string(d->getcmd(i)->lookup_key);
	}
}

POST_SAVE(script) {
	ar & obj.dialect->name;
	make_trans_map(obj.dialect);
	ar & dialect_trans_map[obj.dialect];
}

POST_LOAD(script) {
	std::string name;
	std::shared_ptr<std::vector<std::string> > trans_map;
	ar & name;
	ar & trans_map;

	const Dialect *d = obj.dialect = dialects[name].get();
	if (!d) {
		throw creaturesException(
				boost::str(
					boost::format("Deserialization error: Unknown dialect '%s'") % name
				)
			);
	}

	for (size_t i = 0; i < obj.ops.size(); i++) {
		if (obj.ops[i].opcode == CAOS_CMD || obj.ops[i].opcode == CAOS_SAVE_CMD) {
			std::string cmdname = (*trans_map)[obj.ops[i].argument];
			const cmdinfo *ci = d->find_command(cmdname.c_str());
			if (!ci) {
				throw creaturesException(
						boost::str(boost::format("Deserialization error: Unknown key '%s' in dialect '%s'") % cmdname % name));
			}
			obj.ops[i].argument = d->cmd_index(ci);
		}
	}
}

#endif
