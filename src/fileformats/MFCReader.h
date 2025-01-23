#pragma once

#include "MFCObject.h"
#include "common/Exception.h"
#include "common/ResizableContainerView.h"
#include "common/span.h"

#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

class Reader;

// Windows MFC serialized data / CArchive serialized data

class MFCReader {
  public:
	MFCReader(Reader& in);
	~MFCReader();

	void ascii_dword(std::string&);
	void ascii_mfcstring(std::string&);
	void ascii_nullterminated(std::string&, size_t);
	void win1252_mfcstring(std::string&);

	void size_u8(ResizableContainerView out);
	void size_u16(ResizableContainerView out);
	void size_u32(ResizableContainerView out);

	void operator()(uint8_t&);
	void operator()(int8_t&);
	void operator()(uint16_t&);
	void operator()(int16_t&);
	void operator()(uint32_t&);
	void operator()(int32_t&);
	void operator()(span<uint8_t>);

	template <typename T>
	void operator()(T*& out) {
		MFCObject* base_object = read_object().get();
		if (base_object == nullptr) {
			out = nullptr;
			return;
		}
		out = dynamic_cast<T*>(base_object);
		if (out == nullptr) {
			// TODO: name of type we got, name of type we expected
			throw Exception("Wrong object type");
		}
	}

	template <typename T>
	void operator()(std::shared_ptr<T>& out) {
		std::shared_ptr<MFCObject> base_object = read_object();
		if (base_object == nullptr) {
			out = nullptr;
			return;
		}
		out = std::dynamic_pointer_cast<T>(base_object);
		if (out == nullptr) {
			// TODO: name of type we got, name of type we expected
			throw Exception("Wrong object type");
		}
	}

	template <typename T>
	void register_class(const std::string& name, int schema_number) {
		ClassInfo info;
		info.newfunc = [] {
			return std::static_pointer_cast<MFCObject>(std::make_shared<T>());
		};
		info.readfunc = [](MFCObject* obj, MFCReader& reader) {
			// the static_cast is unchecked, but we know for sure that the
			// MFCObject passed in is in fact a T so we're safe
			static_cast<T*>(obj)->serialize(reader);
		};
		m_classregistry[std::make_pair(name, schema_number)] = info;
	}

  private:
	struct ClassInfo {
		std::shared_ptr<MFCObject> (*newfunc)();
		void (*readfunc)(MFCObject*, MFCReader&);
	};

	std::shared_ptr<MFCObject> read_object();
	std::string read_ascii(size_t n);

	Reader& m_in;

	std::map<std::pair<std::string, int>, ClassInfo> m_classregistry;
	std::map<uint16_t, ClassInfo> m_classids;
	std::vector<std::shared_ptr<MFCObject>> m_objects;
};
