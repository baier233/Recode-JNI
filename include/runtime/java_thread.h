//
// Created by Administrator on 2024/3/15.
//

#ifndef THREAD_H
#define THREAD_H

#include "oop.h"
#include "../include_header.h"

namespace java_hotspot {
    class thread_base {
    };

    class java_thread : public thread_base {
    public:
        auto get_next() -> java_thread *;

        auto get_jni_envoriment() -> JNIEnv *;

        auto get_thread_object() -> oop;

        auto set_thread_object(oop obj) -> void;

        auto get_thread_state() -> int;

        auto set_thread_state(int state) -> void;
    };

    namespace threads {
        auto first() -> java_thread *;
    }
}


#endif //THREAD_H
