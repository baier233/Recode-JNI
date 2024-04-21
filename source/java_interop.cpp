//
// Created by Administrator on 2024/3/15.
//

#include "../include/java_interop.h"
#include "../include/utility/jvm_internal.h"

#include "jvm_thread.h"

static java_hotspot::thread_base *last_temp_thread;


java_interop::debug_accessor::debug_accessor() {
    if (javaVM){
        this->vm = javaVM;
    }else{
        jint vm_count = 0;
        /* Try to get the first created jvm */
        jint error = JNI_GetCreatedJavaVMs(&vm, 1, &vm_count);

        /* Check error */
        if (error != JNI_OK || vm_count == 0)
            throw std::runtime_error("Failed to get created Java VMs");
    }

    if (!this->env)
    {
        jint res = this->vm->GetEnv((void**)&this->env, JNI_VERSION_1_8);

        if (res == JNI_EDETACHED)
            res = this->vm->AttachCurrentThreadAsDaemon((void**)&this->env, nullptr);

        if (res != JNI_OK)
            return;
    }

    if (!this->jvmti)
    {
        this->vm->GetEnv((void**)&this->jvmti, JVMTI_VERSION_1_2);
    }
    if (this->env == nullptr) {
        this->vm->DetachCurrentThread();
        //this->vm->DestroyJavaVM();
    }
}

java_interop::debug_accessor::~debug_accessor() {
    if (!vm)
        return;
    vm->DetachCurrentThread();
}

auto java_interop::debug_accessor::find_class(const char *name) const -> jclass {
    /* Find class */
    const auto clazz = env->FindClass(name);

    /* Check error */
    if (!clazz)
        throw std::runtime_error("Failed to find class");

    return clazz;
}

auto java_interop::debug_accessor::find_method(const jclass clazz, const char *name,
                                               const char *sig) const -> jmethodID {
    /* Find method */
    const auto method = env->GetMethodID(clazz, name, sig);

    /* Check error */
    if (!method)
        throw std::runtime_error("Failed to find method");

    return method;
}

auto java_interop::debug_accessor::find_static_method(const jclass clazz, const char *name,
                                                      const char *sig) const -> jmethodID {
    /* Find method */
    const auto method = env->GetStaticMethodID(clazz, name, sig);

    /* Check error */
    if (!method)
        throw std::runtime_error("Failed to find static method");

    return method;
}

auto java_interop::debug_accessor::find_field(_jclass *const clazz, const char *name,
                                              const char *sig) const -> jfieldID {
    /* Find field */
    const auto field = env->GetFieldID(clazz, name, sig);

    /* Check error */
    if (!field)
        throw std::runtime_error("Failed to find field");

    return field;
}

auto java_interop::debug_accessor::get_env() const -> JNIEnv * {
    return env;
}

auto java_interop::debug_accessor::get_vm() const -> JavaVM * {
    return vm;
}

auto java_interop::debug_accessor::get_jvmti() const -> jvmtiEnv * {
    return jvmti;
}

auto java_interop::attach_temp_thread() -> void {
    detach_temp_thread();
    last_temp_thread = java_hotspot::thread_local_storage::get_thread_slow();
    java_hotspot::thread_local_storage::set_thread(java_hotspot::threads::first());
}

auto java_interop::detach_temp_thread() -> void {
    if (last_temp_thread) {
        java_hotspot::thread_local_storage::set_thread(last_temp_thread);
        last_temp_thread = nullptr;
    }
}

auto java_interop::steal_jni() -> JNIEnv * {
    return java_hotspot::threads::first()->get_jni_envoriment();
}

auto java_interop::execute_jni(const std::function<bool(JNIEnv *)> &func) -> void {
    auto thread = java_hotspot::threads::first();
    while (thread) {
        const auto original_thread = java_hotspot::thread_local_storage::get_thread_slow();
        java_hotspot::thread_local_storage::set_thread(thread);
        if (!thread->get_jni_envoriment()) {
            continue;
        }
        const auto jni_envoriment = thread->get_jni_envoriment();
        jni_envoriment->ExceptionClear();
        jni_envoriment->EnsureLocalCapacity(1);
        if (!jni_envoriment->PushLocalFrame(1)) {
            if (const bool result = func(jni_envoriment); !result) {
                jni_envoriment->ExceptionClear();
                jni_envoriment->PopLocalFrame(nullptr);
                java_hotspot::thread_local_storage::set_thread(original_thread);
                thread = thread->get_next();
                continue;
            }
            jni_envoriment->ExceptionClear();
            jni_envoriment->PopLocalFrame(nullptr);
            java_hotspot::thread_local_storage::set_thread(original_thread);
            return;
        }
        thread = thread->get_next();
    }
}

auto java_interop::find_class(const char *name) -> jclass {
    jclass clazz = nullptr;
    execute_jni([&](JNIEnv *env) {
        clazz = env->FindClass(name);
        return clazz ? true : false;
    });
    return clazz;
}

auto java_interop::find_method(const jclass clazz, const char *name, const char *sig) -> jmethodID {
    jmethodID method = nullptr;
    execute_jni([&](JNIEnv *env) {
        method = env->GetMethodID(clazz, name, sig);
        return clazz ? true : false;
    });
    return method;
}

auto java_interop::find_static_method(const jclass clazz, const char *name, const char *sig) -> jmethodID {
    jmethodID method = nullptr;
    execute_jni([&](JNIEnv *env) {
        method = env->GetStaticMethodID(clazz, name, sig);
        return method ? true : false;
    });
    return method;
}

auto java_interop::find_field(_jclass *const clazz, const char *name, const char *sig) -> jfieldID {
    jfieldID field = nullptr;
    execute_jni([&](JNIEnv *env) {
        field = env->GetFieldID(clazz, name, sig);
        return field ? true : false;
    });
    return field;
}

java_hotspot::instance_klass *java_interop::get_instance_class(_jclass *const klasas) {
    /* Check if class is null */
    if (klasas == nullptr)
        return nullptr;

    /* Dereference class */
    void *klass_ptr = *reinterpret_cast<void **>(klasas);
    if (klass_ptr == nullptr)
        return nullptr;

    // Get the instance klass
    klass_ptr = *reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(klass_ptr) + global_offsets::klass_offset);
    return static_cast<java_hotspot::instance_klass *>(klass_ptr);
}
