// circular_buffer.h
#pragma once

template <int Size>
class CircularBuffer
{
    static constexpr char delimiter = '\r';

public:
    CircularBuffer() : head(0), tail(0), size(0), commandIndex(0), commandFound(false) {}

    void write(const char data)
    {
        if (size == Size)
            return; // ignore input, buffer is full
        buffer[head] = data;
        head = (head + 1) % Size;
        size++;
    }
    bool isCommandFound()
    {
        for (; (size != 0) && (commandFound == false); tail = (tail + 1) % Size, size--)
        {
            if (buffer[tail] == delimiter)
            {
                command[commandIndex] = '\0';
                commandFound = true;
            }
            else
            {
                command[commandIndex++] = buffer[tail];
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
    char buffer[Size];
    char command[Size];
    int head;
    int tail;
    int size;
    int commandIndex;
    bool commandFound;
};
