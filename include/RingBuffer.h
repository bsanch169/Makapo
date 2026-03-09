#pragma once
#include <stddef.h> 

//The type we will use is a PaddlerData struct
template <typename T, size_t capacity>
class RingBuffer {
  static_assert(capacity > 0, "RingBuffer capacity must be > 0");

public:
  bool pop(T& out) {
    if (isEmpty()) return false;

    out = buffer[head];
    head = (head + 1) % capacity;
    count--;
    return true;
  }

  void push(const T& value) {
    buffer[tail] = value;

    if (count == capacity) {
      head = (head + 1) % capacity;
    } else {
      count++;
    }

    tail = (tail + 1) % capacity;
  }

  bool peek(T& out) const {
    if (isEmpty()) return false;
    out = buffer[head];
    return true;
  }

  size_t size() const { return count; }
  bool isEmpty() const { return count == 0; }
  bool isFull() const { return count == capacity; }

  void clear() {
    head = 0;
    tail = 0;
    count = 0;
    buffer = T[capacity];
  }

private:
  T buffer[capacity];
  size_t head = 0;
  size_t tail = 0;  
  size_t count = 0;
};