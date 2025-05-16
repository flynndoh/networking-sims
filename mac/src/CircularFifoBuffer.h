#ifndef CIRCULARFIFOBUFFER_H_
#define CIRCULARFIFOBUFFER_H_

#include <memory>

template <typename T> class CircularFifoBuffer {
public:
    CircularFifoBuffer(int length);
    void push(const T& item);
    T pop(void);
    bool isEmpty(void);
    bool isFull(void);

private:
    size_t head;
    size_t tail;
    size_t capacity;
    std::unique_ptr<T[]> buff;
};

template <typename T> CircularFifoBuffer<T>::CircularFifoBuffer(int capacity)
{
    this->head = 0;
    this->tail = 0;
    this->capacity = capacity;
    this->buff = std::unique_ptr<T[]>(new T[capacity]);
}

// If the buffer is full, we overwrite the oldest thing in the buffer!
template <typename T> void CircularFifoBuffer<T>::push(const T& item)
{
    buff[tail] = item;
    tail = (tail + 1) % capacity;
}

// This should only ever be called after the caller has verified that the buffer has > 0 items.
template <typename T> T CircularFifoBuffer<T>::pop(void) {
    T item = buff[head];
    buff[head] = nullptr;
    head = (head + 1) % capacity;
    return item;
}

template <typename T> bool CircularFifoBuffer<T>::isEmpty(void) {
    return head == tail;
}

template <typename T> bool CircularFifoBuffer<T>::isFull(void) {
    return tail == ((head - 1) & capacity);
}
#endif /* CIRCULARFIFOBUFFER_H_ */
