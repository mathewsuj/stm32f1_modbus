// circular_buffer.h
#pragma once
#include <algorithm>

inline bool validateFrame(const uint8_t &data)
{

    return (data > 0);
}

template <typename T, int Size>
class CircularBuffer
{
    // static constexpr char delimiter = '\r';

public:
    CircularBuffer() : head(0), tail(0), size(0), commandIndex(0), commandFound(false) {}

    void write(const T data)
    {
        if (size == Size)
            return; // ignore input, buffer is full
        if (head > Size - 1)
        {
            asm("bkpt 32");
        }
        buffer[head] = data;
        head = (head + 1) % Size;
        size++;
    }
    void write(const T *data, const size_t count)
    {
        if (count > Size)
            return; // too long input
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
    template <typename Validator>
    bool isValidFrame(Validator &&validateFunction) const
    {

        for (const auto &data : buffer)
        {
            if (!validateFunction(data))
            {
                return false;
            }
        }
        return true;
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
