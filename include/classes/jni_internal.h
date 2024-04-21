//
// Created by Administrator on 2024/3/20.
//

#ifndef JNI_H
#define JNI_H
#include <jni/jni.h>

#include "oop.h"

using make_local_t = jobject(*)(oop);

using oop_store_t = void(*)(narrowOop *, oop);

namespace java_hotspot {
    namespace jni_handles_internal {
        inline make_local_t make_local = nullptr;
        inline oop_store_t oop_store = nullptr;
    }

    class jni_id {
    public:
        auto get_offset() -> int;
    };

    namespace jni_handles {
        auto resolve(jobject handle) -> oop;

        auto resolve_non_null(jobject handle) -> oop;;
    }
}


#endif //JNI_H
