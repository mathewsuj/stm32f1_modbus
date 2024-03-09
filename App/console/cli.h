#pragma once
#include <array>

namespace utility
{
class Cli {
private:
  enum CommandType {
    HELP,
    DUMP_MODEL,
    DUMP_PORTS,
    SET_PORT,
    SET_PROTOCOL,
    SOFT_RESET,
    NUM_COMMANDS
  };
  static const int MAX_CMD_LEN = 20;
  static const int MAX_TOKENS = 10;
  char *tokens[MAX_TOKENS];
  static constexpr std::array<const char *, NUM_COMMANDS> cmdlist = {
      "help", "dumpmodel", "dumpports", "setport", "setprotocol", "reset"};
  int parseInput(const char *input);

public:
  Cli() = default;
  void processCommand(const char *inp);
};
} // namespace utility