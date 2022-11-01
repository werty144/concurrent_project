//
// Created by anton on 25.10.22.
//

#include "versioned_lock.h"
#include "log.h"


int VersionedLock::form_version_locked(int version, bool lock ) {
    return (version << 1) | ((int) lock);
}

bool VersionedLock::try_lock() {
    int cur_version = get_version_locked().first;
    int expected = form_version_locked(cur_version, false);
    int desired = form_version_locked(cur_version, true);
    return v.compare_exchange_strong(expected, desired);
}

void VersionedLock::unlock() {
    auto version_locked = get_version_locked();
    if (!version_locked.second) {
        log("Unlocking unlocked!");
    }
    v.store(form_version_locked(version_locked.first, false));
}

void VersionedLock::set_version(int version) {
    auto version_locked = get_version_locked();
    if (!version_locked.second) {
        log("Versioning unlocked!");
    }
    if (version_locked.first > version) {
        log("Past version!");
    }
    v.store(form_version_locked(version, version_locked.second));
}

std::pair<int, bool> VersionedLock::get_version_locked() {
    int cur_v = v.load();
    auto ret = std::pair(cur_v >> 1, (bool) (cur_v & 1));
    if (ret.first < 0) {
        log("Bad version!");
    }
    return ret;
}

void VersionedLock::init() {
    int ini = form_version_locked(0, false);
    v.store(ini);
}

bool VersionedLock::is_locked() {
    return get_version_locked().second;
}

