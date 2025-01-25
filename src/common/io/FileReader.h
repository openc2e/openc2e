#pragma once

/*

A Reader implementation for local files.

Couple things to note:

- There is no "empty" FileReader: the constructor always
  throws if it can't open the file. If you have a FileReader,
  (and you haven't std::move'd from it), then it is valid
  and points to a file.

  If you really need a way to distinguish between a FileReader
  that is valid and one which is not, try Optional<FileReader>.

- FileReader always keeps a copy of the path it opened for query
  by the user and for reporting better error messages. IMO this
  is a big oversight of iostreams, as it's often useful to know
  the name of the file.

- read_to_end() reads the file size with stat() and then consumes
  the entire thing in one go, rather than mucking about with
  iterators or what have you.

- tell() does not actually make a syscall, as FileReader tracks
  its position manually. This may give incorrect results for
  special files or when sending file descriptors to other processes!
  Luckily we don't do any of that. Cheap tell() is useful for
  reading different file types (especially sprites, and especially
  C16 sprites which check offsets for each line).

- Because position is tracked manually, seek() is also cheap (in
  fact, a no-op) if we're already at the correct position.

And a wish for future improvements:

- Cheap seek()'s if that position is already in the underlying
  buffer. We have a number of files where we jumo forward to skip
  some section of data that we don't care about. This should just
  be a simple position increment in most cases, but currently we
  have to either seek_relative() and cause a syscall or do an
  fread() and ignore the data, which is inefficient and semantically
  weird.

- Access to peek() multiple bytes if they're already in the buffer
  (or even asking to fill the buffer to a certain size, and then
  peek()'ing that). This would be useful for looking at file magics,
  or helping reduce copies for certain types of reads where we
  immediately transform the data.

- Manual control over BUFSIZ. On some systems (*cough* macOS *cough*)
  BUFSIZ is lower than it could be. We could use min(4096, st_blksize).

So, basically, FileReader should be modified to use open()/read()
instead of fopen()/fread() and manage its own buffer :)

*/

#include "common/io/Reader.h"

#include <ghc/filesystem.hpp>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

class FileReader final : public Reader {
  public:
	FileReader(const ghc::filesystem::path&);
	FileReader(const FileReader&) = delete;
	FileReader(FileReader&&);
	~FileReader();

	FileReader& operator=(const FileReader&) = delete;
	FileReader& operator=(FileReader&&) = delete;

	const ghc::filesystem::path& path() const;

	std::vector<uint8_t> read_to_end() override;
	void seek_absolute(size_t) override;
	void seek_relative(int64_t) override;
	size_t tell() const override;
	bool has_data_left() override;
	uint8_t peek_byte() override;

  private:
	void do_read(uint8_t*, size_t) override;

	ghc::filesystem::path path_;
	FILE* ptr_ = nullptr;
	size_t pos_ = 0;
};
