#include "circularbuffer.h"
#include <cstring>

CircularBuffer::CircularBuffer(uint32_t size)
    : _buffer(new char[size]), _begin(0), _end(0), _capacity(0), _remain(size), _size(size)
{
}

CircularBuffer::~CircularBuffer()
{
    delete[] _buffer;
}

bool CircularBuffer::addBuffer(const char *buffer, uint32_t size)
{
    if (size > _remain)
    {
        return false;
    }

    uint32_t end_pos = (_end + size) % _size;
    if (end_pos > _end)
    {
        memcpy(_buffer + _end, buffer, size);
    }
    else
    {
        uint32_t tail_size = _size - _end;
        memcpy(_buffer + _end, buffer, tail_size);
        memcpy(_buffer, buffer + tail_size, size - tail_size);
    }

    _end = end_pos;
    _capacity += size;
    _remain -= size;

    return true;
}

bool CircularBuffer::popBuffer(uint32_t size)
{
    if (size > _capacity)
    {
        return false;
    }

    _begin = (_begin + size) % _size;

    _capacity -= size;
    _remain += size;
    return true;
}

void CircularBuffer::getBuffer(char *buf, uint32_t len)
{
    if (len > _capacity)
    {
        return;
    }

    uint32_t start_pos = _begin;
    if (start_pos + len <= _size)
    {
        memcpy(buf, _buffer + start_pos, len);
    }
    else
    {
        uint32_t tail_size = _size - start_pos;
        memcpy(buf, _buffer + start_pos, tail_size);
        memcpy(buf + tail_size, _buffer, len - tail_size);
    }

    return;
}

uint32_t CircularBuffer::getRemain()
{
    return _remain;
}

uint32_t CircularBuffer::getCapacity()
{
    return _capacity;
}

char CircularBuffer::operator[](size_t id)
{
    if (id < 0 || id >= _size)
        return 0;
    return _buffer[id];
}
