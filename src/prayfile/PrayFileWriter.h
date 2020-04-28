#pragma once

#include <iosfwd>
#include <map>
#include <string>

class PrayFileWriter {
public:
  enum Compression {
      PRAY_COMPRESS_ON,
      PRAY_COMPRESS_OFF
  };

  PrayFileWriter(std::ostream &stream);

  void writeBlockRawData(const std::string &type, const std::string &name,
                         const char *data, size_t data_size,
                         Compression compress = PRAY_COMPRESS_ON);
  void writeBlockTags(const std::string &type, const std::string &name,
                      const std::map<std::string, int> &integer_tags,
                      const std::map<std::string, std::string> &string_tags,
                      Compression compress = PRAY_COMPRESS_ON);

private:
  std::ostream& stream;
};
