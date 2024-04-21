//
// Created by Administrator on 2024/3/20.
//

#ifndef OOP_H
#define OOP_H

#include "include_header.h"
#include "jvm_internal.h"

namespace java_hotspot {
    class instance_klass;
}

namespace java_hotspot {
    class oop_desc {
    public:
        [[nodiscard]] auto get_compressed_klass() -> narrowKlass;

        [[nodiscard]] auto get_klass() -> instance_klass *;

        [[nodiscard]] auto field_base(int offset) const -> void *;

        template<class T>
        [[nodiscard]] auto obj_field_addr(int offset) const -> T *;

        [[nodiscard]] auto obj_field(int offset) const -> oop;

        static auto load_decode_heap_oop(const narrowOop *p) -> oop;

        static auto load_decode_heap_oop(const oop *p) -> oop;

        static auto decode_heap_oop(narrowOop v) -> oop;

        static auto decode_heap_oop_not_null(narrowOop v) -> oop;

        static bool is_null(narrowOop obj);
    };

    template<class T>
    T *oop_desc::obj_field_addr(const int offset) const {
        return static_cast<T *>(field_base(offset));
    }
}


#endif //OOP_H
