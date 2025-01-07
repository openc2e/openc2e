#include "MFCWriter.h"

#include "common/encoding.h"
#include "common/endianlove.h"

#include <fmt/core.h>
#include <iostream>
#include <stdexcept>

MFCWriter::MFCWriter(std::ostream& out)
	: m_out(out) {
	out.exceptions(std::ios_base::failbit | std::ios_base::badbit);
}

MFCWriter::~MFCWriter() {
}

void MFCWriter::ascii_dword(const std::string& val) {
	if (!is_valid_ascii(reinterpret_cast<const uint8_t*>(val.data()), val.size())) {
		throw Exception(fmt::format("Invalid ASCII string: {:?}", val));
	}

	if (val.size() == 0) {
		m_out.write("\0\0\0\0", 4);
	} else if (val.size() == 4) {
		m_out.write(val.c_str(), val.size());
	} else {
		throw Exception(fmt::format("Expected ASCII 4-byte string, got {:?}", val));
	}
}

void MFCWriter::ascii_mfcstring(const std::string& val) {
	if (!is_valid_ascii(reinterpret_cast<const uint8_t*>(val.data()), val.size())) {
		throw Exception(fmt::format("Invalid ASCII string: {:?}", val));
	}
	if (val.size() < 0xFF) {
		::write8(m_out, val.size());
	} else if (val.size() < 0xFFFF) {
		::write8(m_out, 0xFF);
		::write16le(m_out, val.size());
	} else {
		::write8(m_out, 0xFF);
		::write16le(m_out, 0xFFFF);
		::write32le(m_out, val.size());
	}
	m_out.write(val.c_str(), val.size());
}

void MFCWriter::win1252_mfcstring(const std::string& val_) {
	std::string val = utf8_to_cp1252(val_);
	if (val.size() < 0xFF) {
		::write8(m_out, val.size());
	} else if (val.size() < 0xFFFF) {
		::write8(m_out, 0xFF);
		::write16le(m_out, val.size());
	} else {
		::write8(m_out, 0xFF);
		::write16le(m_out, 0xFFFF);
		::write32le(m_out, val.size());
	}
	m_out.write(val.c_str(), val.size());
}

void MFCWriter::ascii_nullterminated(const std::string& val, size_t n) {
	if (val.size() >= n) {
		throw Exception(fmt::format("ascii_nullterminated: '{}' is bigger than {}", val, n));
	}

	if (!is_valid_ascii(reinterpret_cast<const uint8_t*>(val.data()), val.size())) {
		throw Exception(fmt::format("Invalid ASCII string: {:?}", val));
	}

	m_out.write(val.c_str(), val.size());
	for (size_t i = 0; i < n - val.size(); ++i) {
		m_out.write("\0", 1);
	}
}

void MFCWriter::size_u8(ResizableContainerView val) {
	::write8(m_out, val.size());
}

void MFCWriter::size_u16(ResizableContainerView val) {
	::write16le(m_out, val.size());
}

void MFCWriter::size_u32(ResizableContainerView val) {
	::write32le(m_out, val.size());
}

void MFCWriter::operator()(uint8_t val) {
	::write8(m_out, val);
}

void MFCWriter::operator()(int8_t val) {
	::writesigned8(m_out, val);
}

void MFCWriter::operator()(uint16_t val) {
	::write16le(m_out, val);
}

void MFCWriter::operator()(int16_t val) {
	::writesigned16le(m_out, val);
}

void MFCWriter::operator()(uint32_t val) {
	::write32le(m_out, val);
}

void MFCWriter::operator()(int32_t val) {
	::writesigned32le(m_out, val);
}

void MFCWriter::operator()(span<const uint8_t> buf) {
	m_out.write(reinterpret_cast<const char*>(buf.data()), buf.size());
	if (!m_out) {
		throw Exception(fmt::format("Failed writing buffer of size {}", buf.size()));
	}
}

void MFCWriter::write_object(MFCObject* obj, std::type_index type) {
	// null pointer
	if (obj == nullptr) {
		::write16le(m_out, 0);
		return;
	}

	// object that's already been serialized
	auto pid_it = m_pids.find(obj);
	if (pid_it != m_pids.end()) {
		::write16le(m_out, pid_it->second);
		return;
	}

	// otherwise, we need to serialize the full object
	auto it = m_classregistry.find(type);
	if (it == m_classregistry.end()) {
		throw Exception(fmt::format("Unknown object type {}", type.name()));
	}
	ClassInfo& info = it->second;

	if (info.pid == 0) {
		// new object of a new class
		info.pid = m_next_pid++;
		::write16le(m_out, 0xFFFF);
		::write16le(m_out, info.schema_number);
		::write16le(m_out, info.name.size());
		m_out.write(info.name.c_str(), info.name.size());

		m_pids[obj] = m_next_pid++;
		info.writefunc(obj, *this);
	} else {
		// new object of an existing class
		::write16le(m_out, info.pid | 0x8000);
		m_pids[obj] = m_next_pid++;
		info.writefunc(obj, *this);
	}
}