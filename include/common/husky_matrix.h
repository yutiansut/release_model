#pragma once

#include <cassert>
#include <fcntl.h>
#include <sys/mman.h>
#include <functional>
#include <exception/fd_not_exist_exception.hpp>
#include "util.h"

namespace husky {

typedef unsigned int PAGE_SIZE;
const PAGE_SIZE _4K = 4 * 1024;
const PAGE_SIZE _2M = 2 * 1024 * 1024;

typedef uint8_t CUSTOM_OPT;
const CUSTOM_OPT MemoryMapped = 1 << 0; // 00000001
const CUSTOM_OPT InHeap       = 1 << 1; // 00000010

struct basic_hdr {
    uint dx;
    uint dy;
    uint x_cur;
    uint y_cur;
};

template<typename T>
class basic_matrix {
public:
    void destroy() {
        if (_opt & MemoryMapped) {
            if (_mem != nullptr) {
                munmap(_mem, _bytes_need);
                _mem = nullptr;
            }
        } else if (_opt & InHeap) {
            if (_mem != nullptr) {
                free(_mem);
                _mem = nullptr;
            }
        }
    }

    basic_matrix() = default;

    basic_matrix& operator=(const basic_matrix & other) {
        shallow_copy(other);
        return *this;
    }

    basic_matrix(const basic_matrix &other) {
        shallow_copy(other);
    }

    void shallow_copy(const basic_matrix & other) {
        this->_dx = other._dx;
        this->_dy = other._dy;
        this->_mem = other._mem;
        this->_buff = other._buff;
        this->_hdr = other._hdr;
        this->_bytes_need = other._bytes_need;
        this->_root_path = other._root_path;
        this->_name = other._name;
    }

    void set_opt(CUSTOM_OPT opt) {
        this->_opt = opt;
    }

    static void create(uint dx, uint dy, std::string name, std::string root_path, basic_matrix<T> *p, CUSTOM_OPT opt = MemoryMapped) {
        *p = basic_matrix<T>(dx, dy, false, name, root_path, _4K, opt);
    }

    static void load(bool readonly, std::string name, std::string root_path, basic_matrix<T> *p) {
        basic_matrix<T> temp(1,1,true,name,root_path);
        *p = basic_matrix<T>(temp._hdr->dx, temp._hdr->dy, readonly, name, root_path);
        temp.destroy();
    }

    static void extend(std::string name, std::string root_path, basic_matrix<T> *p) {
        basic_matrix<T> temp(1,1,true,name,root_path);
        p->destroy();
        *p = basic_matrix<T>(temp._hdr->dx + 1, temp._hdr->dy, false, name, root_path);
        temp.destroy();
    }

    basic_matrix(uint dx, uint dy, bool readonly, std::string name, std::string root_path, PAGE_SIZE pagesize = _4K, CUSTOM_OPT opt = MemoryMapped)
        : _dx(dx), _dy(dy), _readonly(readonly), _name(name), _root_path(root_path), _opt(opt)
    {
        std::string file_full_path = husky::slash_path(_root_path) +  _name;
        if (readonly && !file_exists(file_full_path)) {
            throw FdNotExistException(file_full_path);
        }
        constructHelper(dx, dy, readonly, pagesize);
    }

    T& operator()(uint32_t x, uint32_t y) {
        assert((x < _dx && x >= 0) && "x out of boundary");
        assert((y < _dy && y >= 0) && "y out of boundary");
        return buff()[x * _dy + y];
    }

    T operator()(int minus) {
        int temp = _hdr->y_cur - 1 + minus;
        int x = temp < 0 ? 0 : temp;
        return buff()[_hdr->x_cur * _dy + x];
    }

    T operator[](uint32_t y) {
        return buff()[_hdr->x_cur * _dy + y];
    }

    void set(T v) {
        buff()[_hdr->x_cur * _dy + _hdr->y_cur] = v;
        ++_hdr->y_cur;
    }

    T& get_w(int minus) {
        int temp = _hdr->y_cur + minus;
        int x = temp < 0 ? 0 : temp;
        return buff()[_hdr->x_cur * _dy + x];
    }

    void inc() {
        ++_hdr->y_cur;
    }

    void set(char* ptr, int n) {
        memcpy( (char*)_buff + (_hdr->x_cur * _dy + _hdr->y_cur) * sizeof(T), ptr, n ),
        ++_hdr->y_cur;
    }

    void set_cur(uint x, uint y) {
        _hdr->x_cur = x;
        _hdr->y_cur = y;
    }

    bool nan_check() {
        return _hdr->y_cur > 0;
    }

    uint dx() { return _dx; }
    uint dy() { return _dy; }

    void clear() {
        memset(_mem, 0, _bytes_need);
    }

    void fill(T filled_value) {
        memset( ((char*)_buff), filled_value, sizeof(T) * _dx * _dy );
    }

    void clear(uint x) {
        assert((x < _dx && x >= 0) && "x out of boundary");
        memset( ((char*)_buff) + x * sizeof(T) * _dy, 0, sizeof(T) * _dy );
    }

    void fill(uint x, T filled_value) {
        assert((x < _dx && x >= 0) && "x out of boundary");
        memset( ((char*)_buff) + x * sizeof(T) * _dy, filled_value, sizeof(T) * _dy );
    }

    std::pair<uint32_t,uint32_t> get_offset() {
        return {_hdr->x_cur, _hdr->y_cur};
    }

    uint32_t get_x_offset() {
        return _hdr->x_cur;
    }

    uint32_t get_y_offset() {
        return _hdr->y_cur;
    }

    template<class F>
    void for_each(F p) {
        for (unsigned i = 0; i < _dx; ++i) {
            for (unsigned j = 0; j < _dy; ++j) {
                p(this->operator()(i, j));
            }
        }
    }

    template<class F>
    void for_each_backward(F p) {
        for (int i = _dx - 1; i >= 0; --i) {
            for (int j = _dy - 1; j >= 0; --j) {
                if (!p(this->operator()(i, j))) {
                    return;
                }
            }
        }
    }

    template<class F>
    void for_each(F p, unsigned int i) {
        for (unsigned int j = 0; j < _dy; ++j) {
            p(this->operator()(i, j));
        }
    }

    void extend_x(unsigned int extendN) {
        destroy();
        unsigned int temp = _dx;
        _dx += extendN;
        constructHelper(_dx, _dy, false, _4K);
        for (; temp < _dx; ++temp) {
            clear(temp);
        }
    }

    unsigned int search_x(unsigned int y, const T& target) {
        assert((y < _dy && y >= 0) && "y out of boundary");
        for (unsigned int x = 0; x < _dx; ++x) {
            if (target == this->operator()(x, y)) {
                return x;
            }
        }
        return -1;
    }

    unsigned int search_x(unsigned int y, std::function<bool(const T&)> f) {
        assert((y < _dy && y >= 0) && "y out of boundary");
        for (unsigned int x = 0; x < _dx; ++x) {
            if ( f(this->operator()(x, y)) ) {
                return x;
            }
        }
        return -1;
    }

    unsigned int search_y(unsigned int x, const T& target) {
        assert((x < _dx && x >= 0) && "x out of boundary");
        for (unsigned int y = 0; y < _dy; ++y) {
            if (target == this->operator()(x, y)) {
                return y;
            }
        }
        return -1;
    }

    unsigned int search_y(unsigned int x, std::function<bool(const T&)> f) {
        assert((x < _dx && x >= 0) && "x out of boundary");
        for (unsigned int y = 0; y < _dy; ++y) {
            if ( f(this->operator()(x, y)) ) {
                return y;
            }
        }
        return -1;
    }

private:
    void constructHelper(unsigned int dx, unsigned int dy, bool readonly, PAGE_SIZE pagesize = _4K) {
        _bytes_need = sizeof(T) * dx * dy + hdr_reserve;
        _bytes_need = (_bytes_need / pagesize) * pagesize + (_bytes_need % pagesize > 0 ? pagesize : 0);
        std::string path = husky::slash_path(_root_path) + _name;
        if (_opt & MemoryMapped) {
            int _fd;
            if (!readonly) {
                husky::checker::fd_check(_fd = open(path.c_str(), (O_CREAT | O_RDWR), (S_IRUSR|S_IWUSR|S_IRGRP)));
                husky::checker::fd_check(lseek(_fd, _bytes_need - 1, SEEK_SET));
                husky::checker::fd_check(write(_fd, "", 1));
            } else {
                husky::checker::fd_check(_fd = open(path.c_str(), O_RDONLY));
            }
            husky::checker::mmap_check(_mem = mmap(nullptr, _bytes_need, PROT_READ | PROT_WRITE, readonly ? MAP_PRIVATE : MAP_SHARED, _fd, 0));
            husky::checker::fd_check(madvise(_mem, _bytes_need, MADV_SEQUENTIAL));
            close(_fd);
        } else if (_opt & InHeap) {
            _mem = malloc(_bytes_need);
            memset(_mem, 0, _bytes_need);
        }
        husky::checker::cmp_check(sizeof(basic_hdr), hdr_reserve, husky::checker::LTE);
        _hdr = (basic_hdr*) _mem;
        if (!readonly) {
            _hdr->dx = dx;
            _hdr->dy = dy;
            _hdr->x_cur = dx - 1;
            _hdr->y_cur = 0;
        }
        _buff = (char*) _mem + hdr_reserve;
    }

    inline T* buff() {
        return (T*) _buff;
    }
private:
    void* _mem;
    void* _buff;
    basic_hdr* _hdr;
    uint _dx, _dy;
    const int hdr_reserve = 52;
    size_t _bytes_need;

    bool _readonly;
    std::string _name;
    std::string _root_path;

    CUSTOM_OPT _opt = MemoryMapped;
};



/*==========cube impl============*/
struct basic_cube_hdr {
    uint dx;
    uint dy;
    uint dz;
    uint x_cur;
    uint y_cur;
    uint z_cur;
};

template<typename T>
class basic_cube {
public:
    void destroy() {
        if (_opt & MemoryMapped) {
            munmap(_mem, _bytes_need);
        } else if (_opt & InHeap) {
            if (_mem != nullptr) {
                free(_mem);
            }
        }
    };

    basic_cube() = default;

    basic_cube(const basic_cube &other) {
        shallow_copy(other);
    }

    basic_cube& operator = (const basic_cube& other) {
        shallow_copy(other);
        return *this;
    }

    void set_opt(CUSTOM_OPT opt) {
        this->_opt = opt;
    }

    static void create(uint dx, uint dy, uint dz, std::string name, std::string root_path, basic_cube<T> *p, CUSTOM_OPT opt = MemoryMapped) {
        *p = basic_cube<T>(dx, dy, dz, false, name, root_path, _4K, opt);
    }

    static void load(bool readonly, std::string name, std::string root_path, basic_cube<T> *p) {
        basic_cube<T> temp(1,1,1,true,name,root_path);
        *p = basic_cube<T>(temp._hdr->dx, temp._hdr->dy, temp._hdr->dz, readonly, name, root_path);
        temp.destroy();
    }

    basic_cube(uint dx, uint dy, uint dz, bool readonly, std::string name, std::string root_path, PAGE_SIZE pagesize = _4K,  CUSTOM_OPT opt = MemoryMapped)
            : _dx(dx), _dy(dy), _dz(dz), _readonly(readonly), _name(name), _root_path(root_path), _opt(opt) {
        constructor_helper(dx, dy, dz, readonly, pagesize);
    }

    T& operator()(uint x, uint y, uint z) {
        assert((x < _dx && x >= 0) && "x out of boundary");
        assert((y < _dy && y >= 0) && "y out of boundary");
        assert((z < _dz && z >= 0) && "z out of boundary");
        return buff()[x * _dy * _dz + y * _dz + z];
    }

    uint dx() { return _dx; }
    uint dy() { return _dy; }
    uint dz() { return _dz; }

    void clear() {
        memset(_mem, 0, _bytes_need);
    }

    void fill(T filled_value) {
        memset( ((char*)_buff), filled_value, sizeof(T) * _dx * _dy * _dz );
    }

    void fill_x(unsigned int x, T filled_value) {
        assert((x < _dx && x >= 0) && "x out of boundary");
        memset( ((char*)_buff) + x * sizeof(T) * _dy * _dz, filled_value, sizeof(T) * _dy * _dz );
    }

    void clear_x(uint x) {
        assert((x < _dx && x >= 0) && "x out of boundary");
        memset( ((char*)_buff) + x * sizeof(T) * _dy * _dz, 0, sizeof(T) * _dy * _dz );
    }

    void clear_y(uint x, uint y) {
        assert((x < _dx && x >= 0) && "x out of boundary");
        assert((y < _dy && y >= 0) && "y out of boundary");
        memset( ((char*)_buff) + x * sizeof(T) * _dy * _dz + y * sizeof(T) * _dz, 0, sizeof(T) * _dy );
    }

    void extend_x(unsigned int extend_n) {
        destroy();
        unsigned int temp = _dx;
        _dx += extend_n;
        constructor_helper(_dx, _dy, _dz, false, _4K);
        for (; temp < _dx; ++temp) {
            clear_x(temp);
        }
    }

private:
    inline T* buff() {
        return (T*) _buff;
    }

    void constructor_helper(uint dx, uint dy, uint dz, bool readonly, PAGE_SIZE pagesize = _4K) {
        _bytes_need = sizeof(T) * dx * dy * dz + hdr_reserve;
        _bytes_need = (_bytes_need / pagesize) * pagesize + (_bytes_need % pagesize > 0 ? pagesize : 0);
        if (_opt & MemoryMapped) {
            std::string path = husky::slash_path(_root_path) + _name;
            int _fd;
            if (!readonly) {
                husky::checker::fd_check(_fd = open(path.c_str(), (O_CREAT | O_RDWR), (S_IRUSR|S_IWUSR|S_IRGRP)));
                husky::checker::fd_check(lseek(_fd, _bytes_need - 1, SEEK_SET));
                husky::checker::fd_check(write(_fd, "", 1));
            } else {
                husky::checker::fd_check(_fd = open(path.c_str(), O_RDONLY));
            }
            husky::checker::mmap_check(_mem = mmap(nullptr, _bytes_need, PROT_READ | PROT_WRITE, readonly ? MAP_PRIVATE : MAP_SHARED, _fd, 0));
            husky::checker::fd_check(madvise(_mem, _bytes_need, MADV_SEQUENTIAL));
            close(_fd);
        } else if (_opt & InHeap) {
            _mem = malloc(_bytes_need);
            memset(_mem, 0, _bytes_need);
        }
        husky::checker::cmp_check(sizeof(basic_hdr), hdr_reserve, husky::checker::LTE);
        _hdr = (basic_cube_hdr*) _mem;
        if (!readonly) {
            _hdr->dx = dx;
            _hdr->dy = dy;
            _hdr->dz = dz;
            _hdr->x_cur = 0;
            _hdr->y_cur = 0;
            _hdr->z_cur = 0;
        }
        _buff = (char*) _mem + hdr_reserve;
    }

    void shallow_copy(const basic_cube& other) {
        this->_dx = other._dx;
        this->_dy = other._dy;
        this->_dz = other._dz;
        this->_mem = other._mem;
        this->_buff = other._buff;
        this->_hdr = other._hdr;
        this->_bytes_need = other._bytes_need;
        this->_root_path = other._root_path;
        this->_name = other._name;
    }

private:
    uint _dx, _dy, _dz;
    void* _mem;
    void* _buff;
    basic_cube_hdr* _hdr;
    const int hdr_reserve = 52;
    size_t _bytes_need;

    bool _readonly;
    std::string _name;
    std::string _root_path;

    CUSTOM_OPT _opt = MemoryMapped;
};

}