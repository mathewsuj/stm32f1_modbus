
#pragma once

#include "circularbuf.h"

template <typename T, std::size_t Size>
class CommandBuffer : public CircularBuffer<T, Size>
{
    static constexpr char delimiter = '\r';

public:
    using CircularBuffer<T, Size>::CircularBuffer;
    CommandBuffer() : commandIndex(0), commandFound(false)
    {
    }
    bool isCommandFound()
    {
        for (; (this->size != 0) && (commandFound == false);)
        {
            const T c = this->read();
            if (c == delimiter)
            {
                command[commandIndex] = '\0';
                commandFound = true;
            }
            else
            {
                command[commandIndex++] = c;
            }
        }
        return commandFound;
    }
    const char *getCommand()
    {
        commandFound = false;
        commandIndex = 0;
        return command;
    }

private:
    T command[Size];
    int commandIndex;
    bool commandFound;
};
