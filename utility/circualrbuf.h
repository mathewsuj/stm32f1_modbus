// circular_buffer.h
#pragma once

template <typename T, int Size>
class CircularBuffer
{
    static constexpr char delimiter = '\r';

public:
    CircularBuffer() : head(0), tail(0), size(0), commandIndex(0), commandFound(false) {}

    void write(const T data)
    {
        if (size == Size)
            return; // ignore input, buffer is full
        buffer[head] = data;
        head = (head + 1) % Size;
        size++;
    }
    void write(const T *data, const size_t count)
    {
        const size_t size_front = Size - head;
        size_t part_count = (count < size_front) ? count : size_front;
        if (size_front)
        {
            std::copy(data, data + part_count, buffer + head);
        }
        if (count - part_count)
        {
            std::copy(data + part_count, data + count, buffer);
        }
        head = (head + count) % Size;
        size += count;
    }
    const char read()
    {
        if (size)
        {
            const char c = buffer[tail];
            tail = (tail + 1) % Size;
            size--;
            return c;
        }
        return '\0';
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
    T buffer[Size];
    T command[Size];
    int head;
    int tail;
    int size;
    int commandIndex;
    bool commandFound;
};
