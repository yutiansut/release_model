#pragma once

#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdexcept>
#include <iostream>

namespace husky {
    inline static void CHECK(void *cursor) {
        if (cursor == (void *) -1) {
            throw std::runtime_error("init cursor failed!!!");
        }
    }

    template<typename S>
    class share_mem {
    public:
        ~share_mem();
        share_mem() = default;
        share_mem(const std::string &path, size_t item_capacity, bool readonly = true);
        S &get(size_t offset = 0);
        S *cursor();
        void persist(size_t offset, const S &val);
        void persist(size_t offset, S *val_ptr, size_t n);
        void destroy();
        void clear();
    private:
        size_t _capacity;
        size_t _size;
        S *_cursor;
        int _fd;
    };
    
    template<typename S>
    inline share_mem<S>::~share_mem() {
        void *buffer = reinterpret_cast<void *>(_cursor);
        munlock(buffer, _size);
        munmap(buffer, _size);
    }

    template<typename S>
    inline share_mem<S>::share_mem(const std::string &path, size_t item_capacity, bool readonly):   _capacity(item_capacity) {
        if ((sizeof(S) * item_capacity) % 4096 > 0) {
            _size = (sizeof(S) * item_capacity / 4096) * 4096 + 4096;
        } else {
            _size = (sizeof(S) * item_capacity / 4096) * 4096;
        }
        void *cursor;
        if (!readonly) {
            _fd = open(path.c_str(),
                      (O_CREAT | O_RDWR),
                      0666);
            if (_fd < 0) {
                perror("open file");
                throw std::runtime_error("open file");
            }
            if (lseek(_fd, 0, SEEK_END) == -1) {
                close(_fd);
                throw std::runtime_error("failed to seek end file ");
            } else if (lseek(_fd, 0, SEEK_END) < (__off_t)_size) {
                if (lseek(_fd, _size - 1, SEEK_SET) == -1) {
                    close(_fd);
                    throw std::runtime_error("failed to seek set file ");
                }
                if (write(_fd, "", 1) == -1) {
                    close(_fd);
                    throw std::runtime_error("unable to write for page ");
                }
            }
            cursor = mmap(nullptr, _size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
            CHECK(cursor);
            if (madvise(cursor, _size, MADV_SEQUENTIAL) != 0 && mlock(cursor, _size) != 0) {
                munmap(cursor, _size);
                close(_fd);
                throw std::runtime_error("failed to lock memory for page ");
            }
        } else {
            _fd = open(path.c_str(), O_RDONLY, (S_IRUSR | S_IRGRP | S_IROTH));
            cursor = mmap(nullptr, _size, PROT_READ, MAP_PRIVATE, _fd, 0);
            CHECK(cursor);
        }
        close(_fd);
        this->_cursor = (S *) cursor;
    }

    template<typename S>
    inline S& share_mem<S>::get(size_t offset) {
        return *(_cursor + offset);
    }

    template<typename S>
    inline void share_mem<S>::persist(size_t offset, const S &val) {
        *(_cursor + offset) = val;
    }

    template<typename S>
    inline void share_mem<S>::persist(size_t offset, S *val_ptr, size_t n) {
        memcpy(_cursor + offset, val_ptr, sizeof(S) * n);
    }

    template<typename S>
    inline S *share_mem<S>::cursor() {
        return _cursor;
    }

    template<typename S>
    void share_mem<S>::destroy() {
        this->~share_mem<S>();
    }

    template<typename S>
    void share_mem<S>::clear() {
        memset(_cursor, 0, _size);
    }
}