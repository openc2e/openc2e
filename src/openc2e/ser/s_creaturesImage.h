#ifndef S_CREATURESIMAGE_H
#define S_CREATURESIMAGE_H 1

#include "World.h"
#include "common/creaturesImage.h"
#include "serialization.h"

typedef creaturesImage* p_img;

namespace boost {
namespace serialization {
template <class Archive>
inline void save(Archive& ar, const p_img& obj, const int version) {
	bool isNull = (obj == NULL);
	ar& isNull;
	if (!isNull) {
		std::string name = obj->getName();
		ar& name;
	}
}

template <class Archive>
inline void load(Archive& ar, p_img& obj, const int version) {
	bool isNull;
	ar& isNull;
	if (isNull)
		obj = NULL;
	else {
		std::string name;
		ar& name;
		obj = world.gallery->getImage(name);
	}
}
} // namespace serialization
} // namespace boost

BOOST_SERIALIZATION_SPLIT_FREE(creaturesImage*);
BOOST_CLASS_IMPLEMENTATION(creaturesImage*, boost::serialization::primitive_type);

#endif
