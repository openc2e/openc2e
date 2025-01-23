#include "FileWriter.h"

#include "common/io/IOException.h"

#include <string.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace fs = ghc::filesystem;

static inline std::string errno_message() {
	char buf[128];
#ifdef _WIN32
	strerror_s(buf, 128, errno);
#else
	strerror_r(errno, buf, 128);
#endif
	return buf;
}

FileWriter::FileWriter(const fs::path& name) {
#ifdef _WIN32
	ptr_ = _wfopen(name.wstring().c_str(), L"wb");
#else
	ptr_ = fopen(name.c_str(), "wb");
#endif
	if (ptr_ == nullptr) {
		// TODO: better message
		// TODO: get error message on Windows
		throw IOException(fmt::format("failed to open file {:?}: {}",
			name.string(), errno_message()));
	}
	path_ = name;
}

FileWriter::FileWriter(const fs::path& name, FileWriter::option_append_t) {
#ifdef _WIN32
	ptr_ = _wfopen(name.wstring().c_str(), L"ab");
#else
	ptr_ = fopen(name.c_str(), "ab");
#endif
	if (ptr_ == nullptr) {
		// TODO: better message
		// TODO: get error message on Windows
		throw IOException(fmt::format("failed to open file {:?}: {}",
			name.string(), errno_message()));
	}
	path_ = name;
}

FileWriter::FileWriter(FileWriter&& other) {
	ptr_ = other.ptr_;
	path_ = other.path_;
	other.ptr_ = nullptr;
	other.path_ = fs::path{};
}

FileWriter::~FileWriter() {
	if (ptr_ != nullptr) {
		flush();
		fclose(ptr_);
		ptr_ = nullptr;
	}
	path_ = fs::path{};
}

const fs::path& FileWriter::path() const {
	return path_;
}

void FileWriter::flush() {
	if (fflush(ptr_) != 0) {
		throw IOException(fmt::format("fflush() failed for file {:?}: {}",
			path_.string(), errno_message()));
	}
#ifdef _WIN32
	if (_commit(_fileno(ptr_)) != 0) {
		throw IOException(fmt::format("_commit() failed for file {:?}: {}",
			path_.string(), errno_message()));
	}
#else
	if (fsync(fileno(ptr_)) != 0) {
		throw IOException(fmt::format("fsync() failed for file {:?}: {}",
			path_.string(), errno_message()));
	}
#endif
}


void FileWriter::do_write(const uint8_t* buf, size_t n) {
	size_t bytes_written = fwrite(buf, 1, n, ptr_);
	if (bytes_written != n) {
		// TODO: handle EINTR
		if (ferror(ptr_) != 0) {
			throw IOException(fmt::format("fwrite() failed for file {:?}: {}",
				path_.string(), errno_message()));
		} else {
			throw IOException(fmt::format("expected to write {} bytes but only wrote {} for file {:?}",
				n, bytes_written, path_.string()));
		}
	}
}
