#include "cli.h"
#include "console.h"
#include "manager.h"
#include <cstdint>
#include <cstring>
#include <string>

using namespace utility;

void Cli::processCommand(const char *inp)
{
  int token_count = parseInput(inp);
  if (token_count <= 0)
    return;
  for (int idx = 0; idx < NUM_COMMANDS; ++idx)
  {

    if (std::strcmp(tokens[0], cmdlist[idx]) == 0)
    {
      switch (static_cast<CommandType>(idx))
      {
      case HELP:
        debugLog("\n\rcommands:\n\r\thelp\n\r");
        return;

      case DUMP_MODEL:
        consoleDumpmodel();
        return;

      case DUMP_PORTS:
        consoleDumpports();
        return;
      case SET_PORT:

        if (token_count > 3)
        {
          int port_number = std::stoi(tokens[1]);
          if (port_number > 3 || port_number <= 0)
          {

            debugLog("\n\rInvalid port id in set port command!\n\r");
            return;
          }

          db::Communication::ParityType parity_type =
              static_cast<db::Communication::ParityType>(std::stoi(tokens[3]));
          if (parity_type != db::Communication::ParityType::Even &&
              parity_type != db::Communication::ParityType::Odd &&
              parity_type != db::Communication::ParityType::None)
          {
            debugLog("\n\rInvalid Parity for set port command!\n\r");
            return;
          }
          uint16_t baud_rate = std::stoi(tokens[2]);
          auto &device_data = db::TvgDatabase::getInstance();
          device_data.setPortSettings(port_number, baud_rate);

          debugLog("\n\rport setting changed.\n\r", false);
        } else
        {
          debugLog("\n\rInvalid arguments in set port command!\n\r");
        }

        return;
      case SOFT_RESET:
        debugLog("\n\rResetting...\n\r");
        __disable_irq();
        NVIC_SystemReset();
        break;
      default:
        debugLog("\n\rcommand not found!\n\r");
      }
    }
  }
  debugLog("\n\rcommand not found!\n\r");
}

int Cli::parseInput(const char *input)
{
  const char *delimiters = " ,";
  char *token = std::strtok(const_cast<char *>(input), delimiters);
  int tokenIndex = 0;

  while (token != nullptr && tokenIndex < MAX_TOKENS)
  {
    tokens[tokenIndex++] = token;
    token = std::strtok(nullptr, delimiters);
  }

  return tokenIndex;
}
