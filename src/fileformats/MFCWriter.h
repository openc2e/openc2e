#pragma once

#include "MFCObject.h"
#include "common/Exception.h"
#include "common/ResizableContainerView.h"
#include "common/span.h"

#include <iosfwd>
#include <map>
#include <stdint.h>
#include <string>
#include <typeindex>

// Windows MFC serialized data / CArchive serialized data

class MFCWriter {
  public:
	MFCWriter(std::ostream& out);
	~MFCWriter();

	void ascii_dword(const std::string&);
	void ascii_mfcstring(const std::string&);
	void ascii_nullterminated(const std::string&, size_t);
	void win1252_mfcstring(const std::string&);

	void size_u8(ResizableContainerView out);
	void size_u16(ResizableContainerView out);
	void size_u32(ResizableContainerView out);

	void operator()(uint8_t);
	void operator()(int8_t);
	void operator()(uint16_t);
	void operator()(int16_t);
	void operator()(uint32_t);
	void operator()(int32_t);
	void operator()(span<const uint8_t>);

	template <typename T>
	void operator()(T* obj) {
		write_object(obj, obj ? typeid(*obj) : typeid(T));
	}

	template <typename T>
	void operator()(const std::shared_ptr<T>& obj) {
		return (*this)(obj.get());
	}

	template <typename T>
	void register_class(const std::string& name, int schema_number) {
		ClassInfo info;
		info.name = name;
		info.schema_number = schema_number;
		info.writefunc = [](MFCObject* obj, MFCWriter& writer) {
			// the inner static_cast is unchecked, but we know for sure that the
			// MFCObject passed in is in fact a T so we're safe
			static_cast<T*>(obj)->serialize(writer);
		};
		m_classregistry[typeid(T)] = info;
	}

  private:
	struct ClassInfo {
		std::string name;
		int schema_number;
		uint16_t pid = 0;
		bool (*isfunc)(const std::type_info& type);
		void (*writefunc)(MFCObject*, MFCWriter&);
	};

	void write_object(MFCObject*, std::type_index);

	std::ostream& m_out;

	std::map<std::type_index, ClassInfo> m_classregistry;
	std::map<const MFCObject*, uint16_t> m_pids;
	uint16_t m_next_pid = 1;
};
