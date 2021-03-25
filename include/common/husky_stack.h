#pragma once

#include <cstring>

namespace husky {

template<typename T>
class stack {
public:
    ~stack();
    stack(size_t init_capacity = 10);
    bool empty();
    T* pop();
    T* peek();
    bool offer(T value);
    bool full();
    void clear();

private:
    char* _buff;
    size_t _capacity;
    T* _cursor;
};

template<typename T>
inline stack<T>::~stack() {
    delete _buff;
}

template<typename T>
inline stack<T>::stack(size_t init_capacity): _capacity(init_capacity) {
    _buff = new char[_capacity];
    _cursor = nullptr;
}

template<typename T>
inline T *stack<T>::peek() {
    return _cursor;
}

template<typename T>
inline T *stack<T>::pop() {
    if (empty()) return nullptr;
    T* temp = _cursor;
    if (_cursor == (T*) _buff) {
        _cursor = nullptr;
    } else {
        --_cursor;
    }
    return temp;
}

template<typename T>
inline bool stack<T>::offer(T value) {
    if (full()) return false;
    if (empty()) {
        _cursor = (T*)_buff;
    } else {
        ++_cursor;
    }
    *_cursor = value;
    return true;
}

template<typename T>
inline bool stack<T>::empty() {
    return _cursor == nullptr;
}

template<typename T>
inline bool stack<T>::full() {
    return (T*)_buff + _capacity - 1 == _cursor;
}

template<typename T>
void stack<T>::clear() {
    memset(_buff, 0, sizeof(T) * _capacity);
    _cursor = nullptr;
}

}
