// circular_buffer.h
#pragma once
#include <cstddef>

template <typename T, std::size_t Size>
class CircularBuffer
{
    static constexpr char delimiter = '\r';

public:
    CircularBuffer() : head(0), tail(0), size(0) {}

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

protected:
    T buffer[Size];
    int head;
    int tail;
    int size;
};
