#include "mappedfile.h"

#include <cassert>
#include <fmt/core.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else // assume POSIX
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#endif

mappedfile::mappedfile() {
}

mappedfile::mappedfile(const ghc::filesystem::path& filename) {
#ifdef _WIN32
	// todo: store the handle somewhere?
	m_file = CreateFileW(filename.wstring().c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if (m_file == INVALID_HANDLE_VALUE) {
		throw mappedfileerror(fmt::format("Error opening file, error = {}", GetLastError()));
	}
	LARGE_INTEGER lifilesize;
	if (GetFileSizeEx(m_file, &lifilesize) == 0) {
		auto sizeerr = GetLastError();
		CloseHandle(m_file); // TODO: handle error
		throw mappedfileerror(fmt::format("Error getting size of file, error = {}", sizeerr));
	};
	m_size = lifilesize.QuadPart;
	if (m_size == 0) {
		// TODO: allow empty?
		throw mappedfileerror("Error mapping empty file");
	}
	m_mapping = CreateFileMapping(m_file, nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (m_mapping == nullptr) {
		auto mappingerr = GetLastError();
		CloseHandle(m_file); // TODO: handle error
		throw mappedfileerror(fmt::format("Error creating file mapping, error = {}", mappingerr));
	}
	m_view = MapViewOfFile(m_mapping, FILE_MAP_READ, 0, 0, 0);
	if (m_view == nullptr) {
		auto viewerr = GetLastError();
		CloseHandle(m_mapping); // TODO: handle error
		CloseHandle(m_file); // TODO: handle error
		throw mappedfileerror(fmt::format("Error mapping view of file, error = {}", viewerr));
	}

#else
	int fno = open(filename.c_str(), O_RDONLY);
	if (fno == -1) {
		throw mappedfileerror(fmt::format("Error opening file, errno = {}", errno));
	}
	m_size = static_cast<size_t>(lseek(fno, 0, SEEK_END));
	if (m_size == static_cast<size_t>(-1)) {
		int seekerr = errno;
		::close(fno); // TODO: handle error
		throw mappedfileerror(fmt::format("Error seeking to end of file, errno = {}", seekerr));
	}
	if (m_size == 0) {
		// TODO: allow empty?
		throw mappedfileerror("Error mapping empty file");
	}

	m_map = mmap(nullptr, m_size, PROT_READ, MAP_PRIVATE, fno, 0);
	if (m_map == (void*)-1) {
		int maperr = errno;
		::close(fno); // TODO: handle error
		throw mappedfileerror(fmt::format("Error mapping file, errno = {}", maperr));
	}
	::close(fno); // TODO: handle error
#endif
}

mappedfile::mappedfile(mappedfile&& other)
	: mappedfile() {
	*this = std::move(other);
}

mappedfile& mappedfile::operator=(mappedfile&& other) {
	close();

#ifdef _WIN32
	std::swap(m_file, other.m_file);
	std::swap(m_mapping, other.m_mapping);
	std::swap(m_view, other.m_view);
#else
	std::swap(m_map, other.m_map);
#endif
	std::swap(m_size, other.m_size);

	return *this;
}

mappedfile::~mappedfile() {
	try {
		close();
	} catch (const mappedfileerror&) {
		// welp???
	}
}

void mappedfile::close() {
#ifdef _WIN32
	if (!m_view) {
		return;
	}
	UnmapViewOfFile(m_view); // TODO: handle error
	m_view = nullptr;
	CloseHandle(m_mapping); // TODO: handle error
	m_mapping = nullptr;
	CloseHandle(m_file); // TODO: handle error
	m_file = INVALID_HANDLE_VALUE;
	m_size = 0;
#else
	if (!m_map) {
		return;
	}
	if (munmap(m_map, m_size) == 0) {
		m_map = 0;
		m_size = 0;
	} else {
		throw mappedfileerror(fmt::format("Error unmapping file, errno = {}", errno));
	}
#endif
}

const uint8_t* mappedfile::data() const {
#ifdef _WIN32
	return (const uint8_t*)m_view;
#else
	return (const uint8_t*)m_map;
#endif
}

size_t mappedfile::size() const {
	return m_size;
}