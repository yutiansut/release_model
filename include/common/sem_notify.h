#pragma once
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include "util.h"

static const size_t one_page = 4 * 1024;
static const char* SEM_POST_ERROR = "sem_post error";
static const char* SEM_WAIT_ERROR = "sem_wait error";
static inline void mem_load(const char* sem_id, void** mem) {
    int fd;
    char* mem_loc = husky::concat("/dev/shm/", sem_id);
    husky::checker::fd_check(fd = open(mem_loc, (O_CREAT | O_RDWR), 0666));
    delete mem_loc;
    husky::checker::fd_check(lseek(fd, one_page - 1, SEEK_SET));
    husky::checker::fd_check(write(fd, "", 1));
    husky::checker::mmap_check(*mem = mmap(nullptr, one_page, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
}

namespace husky {

class sem_notifier {
public:
    ~sem_notifier() {
        sem_destroy((sem_t*) _mem);
        munmap(_mem, one_page);
        remove(_sem_id.c_str());
    }

    sem_notifier(const std::string &sem_id): _sem_id(sem_id.c_str()) {
        mem_load(_sem_id.c_str(), &_mem);
        husky::checker::sys_check(sem_init((sem_t*) _mem, 1, 0), "sem_init error");
    }

    void notify() {
        husky::checker::sys_check(sem_post((sem_t*) _mem), SEM_POST_ERROR);
    }

private:
    std::string _sem_id;
    void* _mem;
};

class sem_listener {
public:
    ~sem_listener() {
        munmap(_mem, one_page);
    }

    sem_listener(const std::string &sem_id): _sem_id(sem_id.c_str()) {
        mem_load(_sem_id.c_str(), &_mem);
    }

    void wait() {
        husky::checker::sys_check(sem_wait((sem_t*) _mem), SEM_WAIT_ERROR);
    }

private:
    std::string _sem_id;
    void *_mem;
};

}