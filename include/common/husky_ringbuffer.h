#pragma once

#include <cstring>

namespace husky {

template<typename T>
class ringbuffer {
public:
    ~ringbuffer();
    ringbuffer(size_t capacity = 10);
    void write(T value);
    T* read();
    void clear();
    bool empty();

private:
    T* buff_front();
    T* buff_end();
private:
    char* _buff;
    size_t _capacity;
    T* _w_cursor;
    T* _r_cursor;
    size_t _unread;
};

template<typename T>
inline ringbuffer<T>::~ringbuffer() {
    delete _buff;
}

template<typename T>
inline ringbuffer<T>::ringbuffer(size_t capacity): _capacity(capacity), _unread(0) {
    _buff = new char[capacity * sizeof(T)];
    _w_cursor = (T*)_buff;
    _r_cursor = (T*)_buff;
}

template<typename T>
inline void ringbuffer<T>::write(T value) {
    *_w_cursor = value;
    if (_w_cursor == buff_end()) {
        _w_cursor = buff_front();
    } else {
        ++_w_cursor;
    }
    // writer has written a loop, lose one element
    if (_w_cursor == _r_cursor) {
        if (_r_cursor == buff_end()) {
            _r_cursor = buff_front();
        } else {
            ++_r_cursor;
        }
    }
}

template<typename T>
inline T* ringbuffer<T>::read() {
    if (empty()) return nullptr;
    if (_r_cursor == buff_end()) {
        _r_cursor = buff_front();
        return buff_end();
    } else {
        ++_r_cursor;
        return _r_cursor - 1;
    }
}

template<typename T>
inline void ringbuffer<T>::clear() {
    memset(_buff, 0, sizeof(T) * _capacity);
}

template<typename T>
inline bool ringbuffer<T>::empty() {
    return _w_cursor == _r_cursor;
}

template<typename T>
inline T *ringbuffer<T>::buff_front() {
    return (T*)_buff;
}

template<typename T>
inline T *ringbuffer<T>::buff_end() {
    return (T*)_buff + _capacity - 1;
}

}