#include "MFCReader.h"

#include "common/Repr.h"
#include "common/encoding.h"
#include "common/endianlove.h"

#include <fmt/core.h>
#include <iostream>
#include <stdexcept>

MFCReader::MFCReader(std::istream& in)
	: m_in(in) {
	in.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	m_objects.emplace_back(nullptr);
}

MFCReader::~MFCReader() {
}

const std::vector<std::unique_ptr<MFCObject>>& MFCReader::objects() const {
	return m_objects;
}

std::vector<std::unique_ptr<MFCObject>>&& MFCReader::release_objects() {
	return std::move(m_objects);
}

std::string MFCReader::read_ascii(size_t n) {
	std::string val(n, '\0');
	m_in.read(&val[0], n);
	if (!is_valid_ascii(reinterpret_cast<const uint8_t*>(val.data()), val.size())) {
		// TODO: debug representation?
		throw std::domain_error(fmt::format("Invalid ASCII string: '{}'", repr(val)));
	}
	return val;
}

void MFCReader::ascii_dword(std::string& out) {
	out = read_ascii(4);
	if (out == std::string("\0\0\0\0", 4)) {
		out = {};
	}
	auto first_nul = out.find('\0');
	if (first_nul != std::string::npos) {
		throw Exception(fmt::format("Expected ASCII 4-byte string, got {}", repr(out)));
	}
}

void MFCReader::ascii_mfcstring(std::string& out) {
	uint32_t length = ::read8(m_in);
	if (length == 0xFF) {
		length = ::read16le(m_in);
	}
	if (length == 0xFFFF) {
		length = ::read32le(m_in);
	}
	out = read_ascii(length);
}

void MFCReader::ascii_nullterminated(std::string& out, size_t n) {
	std::string val(n, '\0');
	m_in.read(&val[0], n);

	auto first_nul = val.find('\0');
	if (first_nul != std::string::npos) {
		val.resize(first_nul);
	}

	if (!is_valid_ascii(reinterpret_cast<const uint8_t*>(val.data()), val.size())) {
		// TODO: debug representation?
		throw std::domain_error(fmt::format("Invalid ASCII string: '{}'", repr(val)));
	}
	out = val;
}

void MFCReader::win1252_mfcstring(std::string& out) {
	uint32_t length = ::read8(m_in);
	if (length == 0xFF) {
		length = ::read16le(m_in);
	}
	if (length == 0xFFFF) {
		length = ::read32le(m_in);
	}
	std::string val(length, '\0');
	m_in.read(&val[0], length);
	out = cp1252_to_utf8(val);
}

void MFCReader::size_u8(ResizableContainerView out) {
	out.resize(::read8(m_in));
}

void MFCReader::size_u16(ResizableContainerView out) {
	out.resize(::read16le(m_in));
}

void MFCReader::size_u32(ResizableContainerView out) {
	out.resize(::read32le(m_in));
}

void MFCReader::operator()(uint8_t& out) {
	out = ::read8(m_in);
}

void MFCReader::operator()(int8_t& out) {
	out = ::readsigned8(m_in);
}

void MFCReader::operator()(uint16_t& out) {
	out = ::read16le(m_in);
}

void MFCReader::operator()(int16_t& out) {
	out = ::readsigned16le(m_in);
}

void MFCReader::operator()(uint32_t& out) {
	out = ::read32le(m_in);
}

void MFCReader::operator()(int32_t& out) {
	out = ::readsigned32le(m_in);
}

void MFCReader::operator()(span<uint8_t> out) {
	m_in.read(reinterpret_cast<char*>(out.data()), out.size());
	if (!m_in) {
		throw Exception(fmt::format("Could only read {} out of {} bytes", m_in.gcount(), out.size()));
	}
}

MFCObject* MFCReader::read_object() {
	uint16_t pid = ::read16le(m_in);
	if (pid == 0x7FFF) {
		// 32-bit PIDs
		throw Exception("32-bit ID support is not implemented");

	} else if (pid == 0xFFFF) {
		// new object of a new class
		int schema_number = ::read16le(m_in);
		int name_length = ::read16le(m_in);
		std::string name = read_ascii(name_length);

		auto it = m_classregistry.find(std::make_pair(name, schema_number));
		if (it == m_classregistry.end()) {
			throw Exception(fmt::format("Found unregistered class {}:{}\n", name, schema_number));
		}
		auto classinfo = it->second;
		m_classids[m_objects.size()] = classinfo;
		m_objects.emplace_back(nullptr); // to increment the pids

		// new object
		MFCObject* value = classinfo.newfunc();
		m_objects.emplace_back(value);
		classinfo.readfunc(value, *this);
		return value;

	} else if (pid & 0x8000) {
		// new object of an existing class
		pid = pid & ~0x8000;

		auto it = m_classids.find(pid);
		if (it == m_classids.end()) {
			throw Exception(fmt::format("Reference to unknown class ID {}", pid));
		}
		// new object
		auto classinfo = it->second;
		MFCObject* value = classinfo.newfunc();
		m_objects.emplace_back(value);
		classinfo.readfunc(value, *this);
		return value;

	} else {
		// existing object
		if (pid >= m_objects.size()) {
			throw Exception(fmt::format("Reference to object at ID {} which has not yet been read", pid));
		}
		if (m_classids.find(pid) != m_classids.end()) {
			throw Exception(fmt::format("Referenced object ID {}, but it was a class", pid));
		}
		return m_objects[pid].get();
	}
}
