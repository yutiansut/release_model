#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cstdint>
#include <string>
#include <common/util.h>

namespace husky {

namespace mp1c {
constexpr long KILO_BYTES = 1024 * 1;
constexpr long MEGA_BYTES = 1024 * 1024;

typedef uint8_t FS_TYPE;
constexpr FS_TYPE normalfs = 1 << 1;
constexpr FS_TYPE tlbfs    = 1 << 2;

struct hdr {
    volatile uint16_t w_cursor;
    volatile uint16_t r_cursor;
};
template <typename D> struct payload {
    volatile char w_flag;
    D data;
};
const char W_UNREADY = 0;
template<typename T> class queue {
public:
    ~queue() {
    }

    void release() {
        if (_mem != nullptr) {
            munmap(_mem, _bytes_need);
            _mem = nullptr;
        }
    }

    queue() = default;

    queue& operator=(const queue & other) {
        shallow_copy(other);
        return *this;
    }

    queue(const queue &other) {
        shallow_copy(other);
    }

    void shallow_copy(const queue &other) {
        _mem = other._mem;
        _buff = other._buff;
        _hdr = other._hdr;
        _bytes_need = other._bytes_need;
    }

    queue(std::string name, std::string cache_path, long page_size = 2*MEGA_BYTES, FS_TYPE fsType = tlbfs) {
        _bytes_need = sizeof(mp1c::hdr) + sizeof(mp1c::payload<T>) * UINT16_MAX;
        _bytes_need = (_bytes_need / page_size) * page_size + (_bytes_need % page_size > 0 ? page_size : 0);
        std::string path = husky::slash_path(cache_path) + name;
        bool is_create = !husky::file_exists(path);
        int fd;
        husky::checker::fd_check(fd = open(path.c_str(), (O_CREAT | O_RDWR), (S_IRUSR|S_IWUSR|S_IRGRP)));
        husky::checker::fd_check(lseek(fd, _bytes_need - 1, SEEK_SET));
        if (fsType == normalfs) {
            husky::checker::fd_check(write(fd, "", 1));
        }
        husky::checker::mmap_check(_mem = mmap(nullptr, _bytes_need, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_SHARED, fd, 0));
        _hdr = (mp1c::hdr*) _mem;
        _buff = ((char*) _mem) + sizeof(mp1c::hdr);
        if (is_create) {
            clear();
        }
        close(fd);
    }

    void clear() {
        memset(_mem, 0, _bytes_need);
    }

    T* dequeue() {
        if (!is_empty()) {
            while (__atomic_load_n(&(buff() + _hdr->r_cursor)->w_flag, __ATOMIC_ACQUIRE) == W_UNREADY) {
            }
            __atomic_store_n(&(buff() + _hdr->r_cursor)->w_flag, W_UNREADY, __ATOMIC_RELEASE);
            return &(buff() + (_hdr->r_cursor++))->data;
        }
        return nullptr;
    }

    void enqueue(const T&& data) {
        auto* ptr = this->w_ptr();
        memcpy(&ptr->data, &data, sizeof(T));
        this->finish_write(ptr);
    }

    void enqueue(const T& data) {
        auto* ptr = this->w_ptr();
        memcpy(&ptr->data, &data, sizeof(T));
        this->finish_write(ptr);
    }

    std::pair<mp1c::payload<T>*, uint16_t> w_pair() {
        uint16_t w = w_inc(&_hdr->w_cursor);
        return std::make_pair(buff() + w, w);
    }

    mp1c::payload<T>* w_ptr() {
        return buff() +w_inc(&_hdr->w_cursor);
    }

    T& operator[](uint16_t w_idx) {
        return (buff() + w_idx)->data;
    }

    void finish_write(mp1c::payload<T>* w_t_ptr) {
        __atomic_store_n(&w_t_ptr->w_flag, true, __ATOMIC_RELEASE);
    }

    bool is_empty() {
        return _hdr->w_cursor == _hdr->r_cursor;
    }

    inline mp1c::payload<T>* buff() {
        return (mp1c::payload<T>*) _buff;
    }

    template<class F, class Predicator>
    void forEach(F p, Predicator predicator) {
        for (uint16_t i = 0; i < UINT16_MAX; ++i) {
            if (predicator(this->operator[](i))) {
                p(this->operator[](i));
            } else {
                break;
            }
        }
    }

private:
    uint16_t w_inc(volatile uint16_t *c) {
        uint16_t old = *c;
        while(!__atomic_compare_exchange_n(c, &old, old+1, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
            old = *c;
        }
        return old;
    }
private:
    void* _mem;
    void* _buff;
    mp1c::hdr* _hdr;
    size_t _bytes_need;
};
}


namespace fixed {
    template<typename T> class queue {
    public:
        ~queue();
        queue(unsigned int capacity = 10);
        void push(T val);
        void pop();
        T peek();
        bool exist(const T& val);
        std::list<T> getQueue();
    private:
        std::list<T> _queue;
        unsigned int _capacity;

    };

    template<typename T>
    inline queue<T>::~queue() {
    }

    template<typename T>
    inline queue<T>::queue(unsigned int capacity):_capacity(capacity) {
    }

    template<typename T>
    inline void queue<T>::push(T val) {
        if (_capacity <= _queue.size()) {
            pop();
        }
        _queue.push_back(val);
    }

    template<typename T>
    inline void queue<T>::pop() {
        _queue.pop_front();
    }

    template<typename T>
    inline T queue<T>::peek() {
        return _queue.back();
    }

    template<typename T>
    bool queue<T>::exist(const T &val) {
        for (const T& t: _queue) {
            if (val == t) {
                return true;
            }
        }
        return false;
    }

    template<typename T>
    std::list<T> queue<T>::getQueue() {
        return _queue;
    }
}

}