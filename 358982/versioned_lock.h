//
// Created by anton on 25.10.22.
//

#ifndef CONCURRENT_PROJECT_VERSIONED_LOCK_H
#define CONCURRENT_PROJECT_VERSIONED_LOCK_H

#include <utility>
#include "atomic"

class VersionedLock {
private:
    std::atomic_int v{0};
    int form_version_locked(int version, bool lock);
public:
    std::pair<int, bool> get_version_locked();
    void init();
    bool try_lock();
    void unlock();
    void set_version(int version);
    bool is_locked();
};

#endif //CONCURRENT_PROJECT_VERSIONED_LOCK_H
