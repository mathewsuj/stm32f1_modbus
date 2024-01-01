// circular_buffer.h
#pragma once
#include <algorithm>

template <typename T, std::size_t Size>
class CircularBuffer
{

public:
    CircularBuffer() : head(0), tail(0), size(0), commandIndex(0), commandFound(false)
    {
    }

    bool write(const T data)
    {

        if (size < Size)
        {
            buffer[head] = data;
            head = (head + 1) % Size;
            size++;
            return true;
        }
        return false;
    }
    bool write(const T *data, const size_t count)
    {

        if (count < (Size - size))
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
            return true;
        }
        return false;
    }
    const T read()
    {

        if (size)
        {
            const T c = buffer[tail];
            tail = (tail + 1) % Size;
            size--;

            return c;
        }

        return T{};
    }
    T *getString(T delimiter)
    {
        for (; (size != 0); tail = (tail + 1) % Size, size--)
        {
            if (buffer[tail] == delimiter)
            {
                tail = (tail + 1) % Size;
                size--;
                if (commandIndex == 0)
                    return nullptr;
                command[commandIndex] = '\0';
                commandIndex = 0;

                return command;
            }
            else
            {
                if (commandIndex < Size)
                {
                    command[commandIndex++] = buffer[tail];
                }
            }
        }
        return nullptr;
    }
    T *getCommand()
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
    std::size_t commandIndex;
    bool commandFound;
};
