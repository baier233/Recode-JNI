//
// Created by JKot on 2024/1/25.
//


#ifndef DETOUR_HOOK_H
#define DETOUR_HOOK_H

#include <mutex>
#include <windows.h>
#include "detours.h"

#pragma comment(lib,"detours.lib")

template<typename T>
class detour_hook final {
public:
    void init_hook(void *target, void *function);

    detour_hook() = default;

    ~detour_hook();

    detour_hook(const detour_hook &other) = delete;

    detour_hook(detour_hook &&other) noexcept = delete;

    detour_hook &operator=(const detour_hook &other) = delete;

    detour_hook &operator=(detour_hook &&other) = delete;

    void start_hook();

    void stop_hook();

    T get_original_method();

    std::once_flag &get_flag() {
        return flag_;
    }

private:
    std::once_flag flag_;
    void *target_ = nullptr;
    void *function_ = nullptr;
    bool is_hooked_ = false;
};

template<typename T>
void detour_hook<T>::init_hook(void *target, void *function) {
    target_ = target;
    function_ = function;
}

template<typename T>
detour_hook<T>::~detour_hook() {
    stop_hook();
}

template<typename T>
void detour_hook<T>::start_hook() {
    if (!is_hooked_) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&static_cast<LPVOID &>(target_), function_);
        DetourTransactionCommit();
        is_hooked_ = true;
    }
}

template<typename T>
void detour_hook<T>::stop_hook() {
    if (is_hooked_) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&static_cast<LPVOID &>(target_), function_);
        DetourTransactionCommit();
        is_hooked_ = false;
    }
}

template<typename T>
T detour_hook<T>::get_original_method() {
    return static_cast<T>(target_);
}

#endif
