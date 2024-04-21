//
// Created by Administrator on 2024/3/15.
//

#ifndef JAVAINTEROP_H
#define JAVAINTEROP_H

#include "include_header.h"
#include "../include/classes/klass.h"

namespace java_interop {

    inline JavaVM* javaVM = nullptr;

    class debug_accessor {
    public:
        debug_accessor();

        ~debug_accessor();

        auto find_class(const char *name) const -> jclass;

        auto find_method(jclass clazz, const char *name, const char *sig) const -> jmethodID;

        auto find_static_method(jclass clazz, const char *name, const char *sig) const -> jmethodID;

        auto find_field(jclass clazz, const char *name, const char *sig) const -> jfieldID;

        [[nodiscard]] auto get_env() const -> JNIEnv *;

        [[nodiscard]] auto get_vm() const -> JavaVM *;

        [[nodiscard]] auto get_jvmti() const -> jvmtiEnv *;

    private:
        JavaVM *vm = nullptr;
        JNIEnv *env = nullptr;
        jvmtiEnv *jvmti = nullptr;
    };

    auto attach_temp_thread() -> void;

    auto detach_temp_thread() -> void;

    auto steal_jni() -> JNIEnv *;

    auto execute_jni(const std::function<bool(JNIEnv *)> &func) -> void;

    auto find_class(const char *name) -> jclass;

    auto find_method(jclass clazz, const char *name, const char *sig) -> jmethodID;

    auto find_static_method(jclass clazz, const char *name, const char *sig) -> jmethodID;

    auto find_field(jclass clazz, const char *name, const char *sig) -> jfieldID;

    java_hotspot::instance_klass *get_instance_class(jclass klasas);
};


#endif //JAVAINTEROP_H
