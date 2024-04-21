//
// Created by Administrator on 2024/3/15.
//

#ifndef KLASS_H
#define KLASS_H

#include "const_pool.h"
#include "symbol.h"
#include "array.h"
#include "field_info.h"
#include "jni_internal.h"
#include "method.h"
#include "jvm_breakpoint_info.h"
#include "oop.h"
#include "universe.h"

namespace java_hotspot {
    class JNIid;
    class instance_klass {
    public:
        auto get_name() -> symbol *;

        auto get_constants() -> const_pool *;

        auto get_methods() -> array<method *> *;

        auto get_fields() -> array<uint16_t> *;

        auto jni_ids() -> JNIid*;

        auto jni_id_for_offset(int offset) -> JNIid*;

        void set_jni_ids(JNIid* ids);

        static auto to_instance_jfieldID(int offset) -> jfieldID ;

        auto to_static_jfieldID(int offset) -> jfieldID ;

        auto get_field_id(const char* name,const char* sign,bool isStatic) -> jfieldID;

        auto get_method_id(const char* name,const char* sign,bool isStatic) -> jmethodID;

        auto set_breakpoints(jvm_internal::breakpoint_info *breakpoints) -> void;

        auto get_breakpoints() -> jvm_internal::breakpoint_info *;

        auto get_super_klass() -> instance_klass *;

        auto get_java_mirror() -> oop_desc *;

        auto find_field_info(
            const std::string &field_name,
            const std::string &field_signature
        ) -> std::tuple<field_info *, instance_klass *>;
    };

    inline instance_klass *decode_klass_not_null(const narrowKlass v) {
        const int shift = universe::narrow_klass_shift();
        auto *result = static_cast<instance_klass *>(reinterpret_cast<void *>(
            reinterpret_cast<uintptr_t>(universe::narrow_klass_base()) + (static_cast<uintptr_t>(v) << shift)));
        return result;
    }

    auto get_static_object_field(
        field_info *field_info,
        instance_klass *current_klass
    ) -> jobject;

    auto set_static_object_field(
        field_info *field_info,
        instance_klass *current_klass,
        jobject obj
    ) -> void;

    /* The pointers obtained by this function are not managed by the JVM, so it will cause a memory leak if not managed properly. */
    auto get_objet_field_detach(
        field_info *field_info,
        jobject obj
    ) -> jobject;

    auto get_objet_field(
        field_info *field_info,
        jobject obj
    ) -> jobject;

    auto set_objet_field(
        field_info *field_info,
        jobject obj,
        jobject value
    ) -> void;
}


#endif //KLASS_H
