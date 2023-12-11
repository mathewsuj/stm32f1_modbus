#include <cstring>
#include "cli.h"
#include "console.h"
#include "manager.h"

using namespace utility;

void Cli::processCommand(const char *inp)
{
    int idx = 0;
    for (const auto &cmd : cmdlist)
    {
        if (std::strcmp(inp, cmdlist[idx]) == 0)
        {
            switch (idx)
            {
            case HELP:
                logMessage("\n\rcommands:\n\r\thelp\n\r", false);
                break;
            case DUMP_MODEL:
                console_dumpmodel();
                break;
            default:
                logMessage("\n\r command not found!\n\r", false);
            }
        }
        idx++;
    }
}