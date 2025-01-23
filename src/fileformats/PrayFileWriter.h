#pragma once

#include "common/io/Writer.h"
#include "common/span.h"

#include <map>
#include <string>

class PrayFileWriter {
  public:
	enum Compression {
		PRAY_COMPRESS_ON,
		PRAY_COMPRESS_OFF
	};

	PrayFileWriter(Writer&);

	void writeBlockRawData(const std::string& type, const std::string& name,
		const unsigned char* data, size_t data_size,
		Compression compress = PRAY_COMPRESS_ON);
	void writeBlockRawData(const std::string& type, const std::string& name,
		span<const unsigned char> data, Compression compress = PRAY_COMPRESS_ON);

	void writeBlockTags(const std::string& type, const std::string& name,
		const std::map<std::string, unsigned int>& integer_tags,
		const std::map<std::string, std::string>& string_tags,
		Compression compress = PRAY_COMPRESS_ON);

  private:
	Writer& stream;
};
