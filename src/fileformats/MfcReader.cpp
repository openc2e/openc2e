#include "MfcReader.h"

#include "common/encoding.h"
#include "common/endianlove.h"

#include <fmt/core.h>
#include <iostream>
#include <stdexcept>

MFCReader::MFCReader(std::istream& in)
	: m_in(in) {
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
		throw std::domain_error(fmt::format("Invalid ASCII string: '{}'", val));
	}
	return val;
}

std::string MFCReader::read_ascii_nullterminated(size_t n) {
	std::string val(n, '\0');
	m_in.read(&val[0], n);

	auto first_nul = val.find('\0');
	if (first_nul != std::string::npos) {
		val.resize(first_nul);
	}

	if (!is_valid_ascii(reinterpret_cast<const uint8_t*>(val.data()), val.size())) {
		// TODO: debug representation?
		throw std::domain_error(fmt::format("Invalid ASCII string: '{}'", val));
	}
	return val;
}

std::string MFCReader::read_ascii_mfcstring() {
	uint32_t length = read8();
	if (length == 0xFF) {
		length = read16le();
	}
	if (length == 0xFFFF) {
		length = read32le();
	}

	return read_ascii(length);
}

uint8_t MFCReader::read8() {
	return ::read8(m_in);
}
int8_t MFCReader::reads8() {
	uint8_t value = read8();
	if (value <= INT8_MAX) {
		return static_cast<int8_t>(value);
	}
	return static_cast<int8_t>(value - INT8_MIN) + INT8_MIN;
}
uint16_t MFCReader::read16le() {
	return ::read16le(m_in);
}
uint32_t MFCReader::read32le() {
	return ::read32le(m_in);
}
int32_t MFCReader::reads32le() {
	uint32_t value = read32le();
	if (value <= INT32_MAX) {
		return static_cast<int32_t>(value);
	}
	return static_cast<int32_t>(value - INT32_MIN) + INT32_MIN;
}

MFCObject* MFCReader::read_object() {
	uint16_t pid = read16le();
	if (pid == 0x7FFF) {
		// 32-bit PIDs
		throw Exception("32-bit ID support is not implemented");

	} else if (pid == 0xFFFF) {
		// new object of a new class
		int schema_number = read16le();
		int name_length = read16le();
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
		value->deserialize(*this);
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
		value->deserialize(*this);
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
