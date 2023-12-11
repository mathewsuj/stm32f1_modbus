#pragma once
namespace utility
{
    class Cli
    {
    private:
        enum CommandType
        {
            HELP,
            DUMP_MODEL,
            NUM_COMMANDS
        };
        static const int max_cmd_len = 20;
        static constexpr char cmdlist[][max_cmd_len] = {"help", "dump model"};

    public:
        Cli() = default;
        static void processCommand(const char *inp);
    };
}