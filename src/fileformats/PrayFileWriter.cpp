#include "PrayFileWriter.h"

#include "common/encoding.h"
#include "common/endianlove.h"
#include "common/vectorstream.h"

#include <cassert>
#include <vector>
#include <zlib.h>

PrayFileWriter::PrayFileWriter(std::ostream& stream_)
	: stream(stream_) {
	stream.write("PRAY", 4);
}

void PrayFileWriter::writeBlockRawData(const std::string& type,
	const std::string& name,
	const unsigned char* data, size_t data_size,
	PrayFileWriter::Compression compression) {
	assert(type.size() == 4);
	stream.write(type.c_str(), 4);

	std::string cp1252_name = ensure_cp1252(name);
	assert(cp1252_name.size() < 128);
	stream.write(cp1252_name.c_str(), cp1252_name.size());
	for (size_t i = 0; i < 128 - cp1252_name.size(); ++i) {
		stream.write("\0", 1);
	}

	if (compression == PRAY_COMPRESS_ON) {
		uLongf compressed_size = compressBound(data_size);
		std::vector<char> compressed_data(compressed_size);
		int status = compress((Bytef*)compressed_data.data(), &compressed_size,
			(const Bytef*)data, data_size);
		if (status != Z_OK) {
			abort(); // TODO
		}

		if (compressed_size < data_size) {
			write32le(stream, compressed_size);
			write32le(stream, data_size);
			write32le(stream, 0x1);

			stream.write(compressed_data.data(), compressed_size);

			return;
		}
	}

	write32le(stream, data_size);
	write32le(stream, data_size);
	write32le(stream, 0);

	stream.write((char*)data, data_size);
}

void PrayFileWriter::writeBlockRawData(const std::string& type,
	const std::string& name,
	span<const unsigned char> data,
	PrayFileWriter::Compression compress) {
	writeBlockRawData(type, name, data.data(), data.size(), compress);
}

void PrayFileWriter::writeBlockTags(
	const std::string& type, const std::string& name,
	const std::map<std::string, unsigned int>& integer_tags,
	const std::map<std::string, std::string>& string_tags,
	PrayFileWriter::Compression compression) {
	vectorstream os;

	write32le(os, integer_tags.size());
	for (auto kv : integer_tags) {
		std::string cp1252_key = ensure_cp1252(kv.first);
		write32le(os, cp1252_key.size());
		os.write(cp1252_key.c_str(), cp1252_key.size());
		write32le(os, kv.second);
	}

	write32le(os, string_tags.size());
	for (auto kv : string_tags) {
		std::string cp1252_key = ensure_cp1252(kv.first);
		write32le(os, cp1252_key.size());
		os.write(cp1252_key.c_str(), cp1252_key.size());
		std::string cp1252_value = ensure_cp1252(kv.second);
		write32le(os, cp1252_value.size());
		os.write(cp1252_value.c_str(), cp1252_value.size());
	}

	writeBlockRawData(type, name, os.vector(), compression);
}
