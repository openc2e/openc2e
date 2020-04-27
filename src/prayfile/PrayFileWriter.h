#pragma once

#include <iosfwd>
#include <map>
#include <string>

class PrayFileWriter {
public:
  PrayFileWriter(std::ostream &stream, bool enable_compression = true);

  void writeBlockRawData(const std::string &type, const std::string &name,
                         const char *data, size_t data_size);
  void writeBlockTags(const std::string &type, const std::string &name,
                      const std::map<std::string, int> &integer_tags,
                      const std::map<std::string, std::string> &string_tags);

private:
  std::ostream& stream;
  bool enable_compression;
};
