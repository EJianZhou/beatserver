#pragma once

#include <cstdint>
#include <memory>

class CircularBuffer {
public:
    CircularBuffer(uint32_t size);
    ~CircularBuffer();

    bool addBuffer(const char *buffer, uint32_t size);
    bool popBuffer(uint32_t size);
    void getBuffer(char *buf, uint32_t len);
    uint32_t getRemain();
    uint32_t getCapacity();
    char operator[](size_t id);

private:
    char *_buffer;
    uint32_t _begin;
    uint32_t _end;
    uint32_t _capacity;
    uint32_t _remain;
    uint32_t _size;
};
