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

	static_assert(sizeof(size_t) >= sizeof(int64_t), "positive int64_t can fit into size_t");
	// static_cast is safe because sb.st_size is non-negative and can fit into a size_t
	if (static_cast<size_t>(sb.st_size) < pos_) {
		throw IOException(fmt::format(
			"stat.st_size < pos_ for file {:?}", path_.string()));
	}

	// static_cast is safe as above
	// unsigned subtraction is safe because sb.st_size >= pos_;
	auto remaining = static_cast<size_t>(sb.st_size) - pos_;
	std::vector<uint8_t> buf(remaining); // TODO: don't initialize vector members
	do_read(buf.data(), buf.size());
	return buf;
}

void FileReader::seek_absolute(size_t n) {
	if (n == pos_) {
		return;
	}

	if (n < pos_) {
		// Warn on backwards seeks. Most of the time, it's more efficient to read a file
		// forwards-only, and this likely represents some weird logic that can be cleaned up.
		fmt::print("WARNING: Backwards seek on file {} (-{})\n",
			path_.string(),
			pos_ - n);
	}

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
	pos_ = n;
}

void FileReader::seek_relative(int64_t off) {
	// do these calcs because we want to track pos_ manually. once we're doing that,
	// we might as well just call seek_absolute() with the final position anyways
	// (and avoids a syscall on certain implementations of fseek(SEEK_CUR) which
	// call tell() and fseek(SEEK_SET) under the hood).
	static_assert(sizeof(size_t) >= sizeof(int64_t), "positive int64_t can fit into size_t");
	if (off >= 0) {
		// static_cast is safe because off is positive and fits into size_t.
		return seek_absolute(static_cast<size_t>(off) + pos_);
	} else if (off < 0) {
		// -(off + 1) is safe because it's guaranteed to fit into an off_t (we
		// don't use the simpler -off because it might be the minimum value,
		// which can't fit into a positive).
		// static_cast is safe because -(off + 1) is positive and fits into size_t.
		// off_negated is mathematically: -(off + 1) + 1 = -off - 1 + 1 = -off.
		size_t off_negated = static_cast<size_t>(-(off + 1)) + 1;
		if (off_negated > pos_) {
			throw IOException(fmt::format("seek_relative() would end up in a negative offset for file {:?}",
				path_.string()));
		}
		// subtraction is safe because we know off_negated <= pos so the result
		// is positive or zero.
		return seek_absolute(pos_ - off_negated);
	}
}

size_t FileReader::tell() const {
	return pos_;
}

bool FileReader::has_data_left() {
	clearerr(ptr_); // reset eof marker, in case file has been written to since last time we checked.
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
	pos_ += bytes_read;
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
