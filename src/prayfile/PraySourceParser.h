#pragma once

#include <mpark/variant.hpp>
#include <string>

class PraySourceParser {
public:
  struct Error {
    std::string message;
  };
  struct EndOfInput {};
  struct GroupBlockStart {
    std::string type;
    std::string name;
  };
  struct GroupBlockEnd {
    std::string type;
    std::string name;
  };
  struct InlineBlock {
    std::string type;
    std::string name;
    std::string filename;
  };
  struct StringTag {
    std::string key;
    std::string value;
  };
  struct StringTagFromFile {
    std::string key;
    std::string filename;
  };
  struct IntegerTag {
    std::string key;
    int value;
  };
  using Event =
      mpark::variant<Error, EndOfInput, GroupBlockStart, GroupBlockEnd,
                      InlineBlock, StringTag, StringTagFromFile, IntegerTag>;

  PraySourceParser(const char *buf);
  Event next();

private:
  enum TokenType {
    PS_EOI,
    PS_ERROR,
    PS_NEWLINE,
    PS_WS,
    PS_BAREWORD,
    PS_STRING,
    PS_INTEGER,
    PS_AT,
    PS_COMMENT,
    PS_EN_GB,
  };

  struct Token {
    TokenType type;
    std::string stringval;
    int intval;
  };

  static std::string TokenToString(Token &token);
  void next_token();
  Event parse_line();
  bool parse_some_ws();

  const char *pray_source_parse_p = nullptr;
  int yylineno = -1;
  Token token;
  bool in_group_block = false;
  std::string last_group_type;
  std::string last_group_name;
  bool seen_en_gb = false;
};
