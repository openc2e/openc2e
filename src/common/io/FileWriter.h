#pragma once

/*

A Writer implementation for local files.

Couple things to note:

- There is no "empty" FileWriter: the constructor always
  throws if it can't open the file. If you have a FileWriter,
  (and you haven't std::move'd from it), then it is valid
  and points to a file.

  If you really need a way to distinguish between a FileWriter
  that is valid and one which is not, try Optional<FileWriter>.

- FileWriter always keeps a copy of the path it opened for query
  by the user and for reporting better error messages. IMO this
  is a big oversight of iostreams, as it's often useful to know
  the name of the file.

*/

#include "common/io/Writer.h"

#include <ghc/filesystem.hpp>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

class FileWriter final : public Writer {
  private:
	struct option_append_t {};

  public:
	constexpr static option_append_t option_append{};

	// to open a file and truncate: FileWriter(path)
	// to open a file and append: FileWriter(path, FileWriter::option_append)
	FileWriter(const ghc::filesystem::path&);
	FileWriter(const ghc::filesystem::path&, option_append_t);
	FileWriter(const FileWriter&) = delete;
	FileWriter(FileWriter&&);
	~FileWriter();

	FileWriter& operator=(const FileWriter&) = delete;
	FileWriter& operator=(FileWriter&&) = delete;

	const ghc::filesystem::path& path() const;
	void flush() override;

  private:
	void do_write(const uint8_t*, size_t) override;

	FILE* ptr_ = nullptr;
	ghc::filesystem::path path_;
};
