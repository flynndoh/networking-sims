#ifndef FIFOBUFFER_H_
#define FIFOBUFFER_H_

#include <queue>
#include "Utils.h"

class FifoBufferInitialisationException : public std::exception {};
class FifoBufferFullException : public std::exception {};
class FifoBufferEmptyException : public std::exception {};

template <typename T> class FifoBuffer {
  public:
    FifoBuffer(int length);
    void push(T item);
    T pop(void);
    bool isEmpty(void);

  private:
    size_t length;
    std::deque<T> queue;
};

// Throws `FifoBufferInitialisationException` when length is not a positive integer.
template <typename T> FifoBuffer<T>::FifoBuffer(int length)
{
    if (length <= 0)
    {
        throw new FifoBufferInitialisationException;
    }

    this->length = length;
    this->queue = std::deque<T>();
}

// Throws `FifoBufferFullException` when attempting to push to an already full buffer.
template <typename T> void FifoBuffer<T>::push(T item)
{
    if (queue.size() < length)
    {
        queue.push_back(item);
    }
    else
    {
        throw new FifoBufferFullException();
    }
}

// Throws `FifoBufferEmptyException` when attempting to pop from an empty buffer.
template <typename T> T FifoBuffer<T>::pop(void) {
    T result;
    if (queue.empty())
    {
        throw new FifoBufferEmptyException();
    }
    else
    {
        result = queue.front();
        queue.pop_front();
        return result;
    }
}

template <typename T> bool FifoBuffer<T>::isEmpty(void) {
    return queue.empty();
}

#endif /* FIFOBUFFER_H_ */
