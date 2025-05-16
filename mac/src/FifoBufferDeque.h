#ifndef FIFOBUFFERDEQUE_H_
#define FIFOBUFFERDEQUE_H_

#include <queue>

class FifoBufferInitialisationException : public std::exception {};
class FifoBufferFullException : public std::exception {};
class FifoBufferEmptyException : public std::exception {};

template <typename T> class FifoBufferDeque {
  public:
    FifoBufferDeque(int length);
    void push(const T& item);
    T pop(void);
    bool isEmpty(void);

  private:
    size_t length;
    std::deque<T> queue;
};

// Throws `FifoBufferInitialisationException` when length is not a positive integer.
template <typename T> FifoBufferDeque<T>::FifoBufferDeque(int length)
{
    if (length <= 0)
    {
        throw new FifoBufferInitialisationException;
    }

    this->length = length;
    this->queue = std::deque<T>();
}

// Throws `FifoBufferFullException` when attempting to push to an already full buffer.
template <typename T> void FifoBufferDeque<T>::push(const T& item)
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
template <typename T> T FifoBufferDeque<T>::pop(void) {
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

template <typename T> bool FifoBufferDeque<T>::isEmpty(void) {
    return queue.empty();
}

#endif /* FIFOBUFFERDEQUE_H_ */
