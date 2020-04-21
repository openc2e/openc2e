#include "prayfile/PrayFileWriter.h"
#include "endianlove.h"

#include <cassert>
#include <sstream>
#include <vector>
#include <miniz.h>

PrayFileWriter::PrayFileWriter(const std::string &filename,
                               bool enable_compression_)
    : stream(filename), enable_compression(enable_compression_) {
  stream.write("PRAY", 4);
}

void PrayFileWriter::writeBlockRawData(const std::string &type,
                                       const std::string &name,
                                       const char *data, size_t data_size) {
  assert(type.size() == 4);
  stream.write(type.c_str(), 4);

  assert(name.size() < 128);
  stream.write(name.c_str(), name.size());
  for (size_t i = 0; i < 128 - name.size(); ++i) {
    stream.write("\0", 1);
  }

  if (enable_compression) {
    uLongf compressed_size = compressBound(data_size);
    std::vector<char> compressed_data(compressed_size);
    int status = compress((Bytef *)compressed_data.data(), &compressed_size,
                          (const Bytef *)data, data_size);
    if (status != Z_OK) {
      abort(); // TODO
    }
    write32le(stream, compressed_size);
    write32le(stream, data_size);
    write32le(stream, 0x1);

    stream.write(compressed_data.data(), compressed_size);
  } else {
    write32le(stream, data_size);
    write32le(stream, data_size);
    write32le(stream, 0);

    stream.write(data, data_size);
  }
}

void PrayFileWriter::writeBlockTags(
    const std::string &type, const std::string &name,
    const std::map<std::string, int> &integer_tags,
    const std::map<std::string, std::string> &string_tags) {
  std::ostringstream os;

  write32le(os, integer_tags.size());
  for (auto kv : integer_tags) {
    write32le(os, kv.first.size());
    os.write(kv.first.c_str(), kv.first.size());
    write32le(os, kv.second);
  }

  write32le(os, string_tags.size());
  for (auto kv : string_tags) {
    write32le(os, kv.first.size());
    os.write(kv.first.c_str(), kv.first.size());
    write32le(os, kv.second.size());
    os.write(kv.second.c_str(), kv.second.size());
  }

  writeBlockRawData(type, name, os.str().c_str(), os.str().size());
}
