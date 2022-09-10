#ifndef FIFOBUFFER_H_
#define FIFOBUFFER_H_

#include <queue>
#include "Utils.h"

class FifoBufferFullException : public std::exception {};
class FifoBufferEmptyException : public std::exception {};

template <typename T> class FifoBuffer
        {
public:
    FifoBuffer(int length);
    void push(T item);
    T pop(void);
    bool empty(void);

private:
    size_t length;
    std::deque<T> queue;
};

template <typename T> FifoBuffer<T>::FifoBuffer(int length)
{
    if (length <= 0)
    {
        throw "length must be a positive integer";
    }

    this->length = length;
    this->queue = std::deque<T>();
}

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

template <typename T> bool FifoBuffer<T>::empty(void) {
    return queue.empty();
}

#endif /* FIFOBUFFER_H_ */
