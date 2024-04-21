//
// Created by Administrator on 2024/3/20.
//

#ifndef JVM_THREAD_H
#define JVM_THREAD_H

#include "include_header.h"

namespace java_hotspot::thread_local_storage_internal {
    inline uint8_t *_thread_index_address_ = nullptr;
}


namespace java_hotspot {
    class thread_base;
}

namespace java_hotspot {
    class jni_handle_block;

    class thread_local_storage {
    public:
        static auto get_thread_index() -> int;

        static auto get_thread_slow() -> thread_base *;

        static auto set_thread(thread_base *thread) -> void;

        auto get_active_handles() -> jni_handle_block *;
    };
}


#endif //JVM_THREAD_H
