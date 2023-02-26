#pragma once

#include "common/Exception.h"
#include "common/ResizableContainerView.h"
#include "common/span.h"

#include <iosfwd>
#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

// Windows MFC serialized data / CArchive serialized data

class MFCObject {
  public:
	virtual ~MFCObject() = default;
};

class MFCReader {
  public:
	MFCReader(std::istream& in);
	~MFCReader();

	const std::vector<std::unique_ptr<MFCObject>>& objects() const;
	std::vector<std::unique_ptr<MFCObject>>&& release_objects();

	void ascii_dword(std::string&);
	void ascii_mfcstring(std::string&);
	void ascii_nullterminated(std::string&, size_t);

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
		MFCObject* base_object = read_object();
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
	void register_class(const std::string& name, int schema_number) {
		ClassInfo info;
		info.newfunc = [] {
			return static_cast<MFCObject*>(new MFCObjectImpl<T>());
		};
		info.readfunc = [](MFCObject* obj, MFCReader& reader) {
			// the inner static_cast is unchecked, but we know for sure that the
			// MFCObject passed in is in fact an MFCObjectImpl<T> so we're safe
			static_cast<T*>(static_cast<MFCObjectImpl<T>*>(obj))->serialize(reader);
		};
		m_classregistry[std::make_pair(name, schema_number)] = info;
	}

  private:
	template <typename T>
	struct MFCObjectImpl : T, MFCObject {
	};

	struct ClassInfo {
		MFCObject* (*newfunc)();
		void (*readfunc)(MFCObject*, MFCReader&);
	};

	MFCObject* read_object();
	std::string read_ascii(size_t n);

	std::istream& m_in;

	std::map<std::pair<std::string, int>, ClassInfo> m_classregistry;
	std::map<uint16_t, ClassInfo> m_classids;
	std::vector<std::unique_ptr<MFCObject>> m_objects;
};
