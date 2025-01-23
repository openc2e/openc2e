#include "FileReader.h"

#include "common/io/IOException.h"

#include <assert.h>
#include <fmt/format.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#if !defined(_WIN32)
static_assert(sizeof(off_t) == sizeof(int64_t), "off_t == int64_t");
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

FileReader::FileReader(const fs::path& name) {
#ifdef _WIN32
	ptr_ = _wfopen(name.wstring().c_str(), L"rb");
#else
	ptr_ = fopen(name.c_str(), "rb");
#endif
	if (ptr_ == nullptr) {
		// TODO: better message
		// TODO: get error message on Windows
		throw IOException(fmt::format("failed to open file {:?}: {}",
			name.string(), errno_message()));
	}
	path_ = name;
}

FileReader::FileReader(FileReader&& other) {
	ptr_ = other.ptr_;
	path_ = other.path_;
	other.ptr_ = nullptr;
	other.path_ = fs::path{};
}

FileReader::~FileReader() {
	if (ptr_ != nullptr) {
		fclose(ptr_);
		ptr_ = nullptr;
	}
}

const fs::path& FileReader::path() const {
	return path_;
}

std::vector<uint8_t> FileReader::read_to_end() {
#ifdef _WIN32
	struct _stat64 sb;
	int ret = _fstat64(_fileno(ptr_), &sb);
#else
	// TODO: define _FILE_OFFSET_BITS=64 on Linux?
	struct stat sb;
	int ret = fstat(fileno(ptr_), &sb);
#endif
	if (ret != 0) {
		// TODO: fallback implementation?
		throw IOException(fmt::format("stat() failed for file {:?}: {}",
			path_.string(), errno_message()));
	}

	if (sb.st_size < 0) {
		throw IOException(fmt::format("st_size was negative for file {:?}",
			path_.string()));
	}

#ifdef _WIN32
	int64_t pos = _ftelli64(ptr_);
#else
	// TODO: define _FILE_OFFSET_BITS=64 on Linux?
	int64_t pos = ftello(ptr_);
#endif
	if (pos < 0) {
		throw IOException(fmt::format("tell() failed for file {:?}: {}",
			path_.string(), errno_message()));
	}

	auto remaining = sb.st_size - pos;
	if (remaining < 0) {
		throw IOException(fmt::format(
			"stat.st_size - tell() was negative for file {:?}", path_.string()));
	}
	static_assert(sizeof(size_t) >= sizeof(int64_t), "positive int64_t can fit into size_t");
	std::vector<uint8_t> buf(static_cast<size_t>(remaining)); // TODO: don't initialize vector members
	do_read(buf.data(), buf.size());
	return buf;
}

void FileReader::seek_absolute(size_t n) {
	// some static asserts to make sure the size check is well-formed
	static_assert(std::numeric_limits<int64_t>::max() >= 0, "max(int64_t) >= 0");
	static_assert(sizeof(size_t) >= sizeof(int64_t), "positive int64_t can fit into size_t");
	if (n > static_cast<size_t>(std::numeric_limits<int64_t>::max())) {
		throw IOException(fmt::format(
			"seek() failed for file {:?}: n is too big to fit into an int64_t", path_.string()));
	}
#ifdef _WIN32
	int ret = _fseeki64(ptr_, n, SEEK_SET);
#else
	int ret = fseeko(ptr_, static_cast<off_t>(n), SEEK_SET);
#endif
	if (ret != 0) {
		throw IOException(fmt::format("seek() failed for file {:?}: {}",
			path_.string(), errno_message()));
	}
}

void FileReader::seek_relative(int64_t n) {
#ifdef _WIN32
	int ret = _fseeki64(ptr_, n, SEEK_CUR);
#else
	// TODO: define _FILE_OFFSET_BITS=64 on Linux?
	static_assert(sizeof(off_t) >= sizeof(int64_t), "int64_t can fit into an off_t");
	int ret = fseeko(ptr_, static_cast<off_t>(n), SEEK_CUR);
#endif
	if (ret != 0) {
		throw IOException(fmt::format("seek() failed for file {:?}: {}",
			path_.string(), errno_message()));
	}
}

size_t FileReader::tell() const {
#ifdef _WIN32
	int64_t pos = _ftelli64(ptr_);
#else
	// TODO: define _FILE_OFFSET_BITS=64 on Linux?
	int64_t pos = ftello(ptr_);
#endif
	if (pos < 0) {
		throw IOException(fmt::format("tell() failed for file {:?}: {}",
			path_.string(), errno_message()));
	}
	static_assert(sizeof(size_t) >= sizeof(int64_t), "positive int64_t can fit into a size_t");
	return static_cast<size_t>(pos);
}

bool FileReader::has_data_left() {
	if (feof(ptr_) != 0) {
		return false;
	}
	int c;
	c = fgetc(ptr_);
	ungetc(c, ptr_);
	if (ferror(ptr_) != 0) {
		throw IOException(fmt::format("fgetc/ungetc failed for file {:?}: {}",
			path_.string(), errno_message()));
	}
	return c != EOF;
}

uint8_t FileReader::peek_byte() {
	int c;
	c = fgetc(ptr_);
	ungetc(c, ptr_);
	if (ferror(ptr_) != 0) {
		throw IOException(fmt::format("fgetc/ungetc failed for file {:?}: {}",
			path_.string(), errno_message()));
	}
	if (c == EOF) {
		throw IOException(fmt::format("got end of input while peeking byte for file {:?}",
			path_.string()));
	}
	if (c < 0 || c > 0xff) {
		throw IOException(fmt::format("can't represent peeked byte {} as uint8_t for file {:?}",
			c, path_.string()));
	}
	// safe because of check above
	return static_cast<uint8_t>(c);
}

void FileReader::do_read(uint8_t* out, size_t n) {
	size_t bytes_read = fread(out, 1, n, ptr_);
	if (bytes_read != n) {
		// TODO: handle EINTR
		if (ferror(ptr_) != 0) {
			throw IOException(fmt::format("fread() failed for file {:?}: {}",
				path_.string(), errno_message()));
		} else {
			throw IOException(fmt::format("expected to read {} bytes but only got {} for file {:?}",
				n, bytes_read, path_.string()));
		}
	}
}
