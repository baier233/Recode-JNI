//
// Created by Administrator on 2024/3/20.
//

#include "jvm_thread.h"

#include "java_thread.h"
#include "jni_internal.h"

auto java_hotspot::thread_local_storage::get_thread_index() -> int {
    return *reinterpret_cast<int *>(thread_local_storage_internal::_thread_index_address_);
}

auto java_hotspot::thread_local_storage::get_thread_slow() -> thread_base * {
    return static_cast<thread_base *>(TlsGetValue(get_thread_index()));
}

auto java_hotspot::thread_local_storage::set_thread(thread_base *thread) -> void {
    TlsSetValue(get_thread_index(), thread);
}

auto java_hotspot::thread_local_storage::get_active_handles() -> jni_handle_block * {
    static VMStructEntry *_active_handles_entry = JVMWrappers::find_type_fields("Thread").value().get()[
        "_active_handles"];
    if (!_active_handles_entry) return nullptr;
    return *reinterpret_cast<jni_handle_block **>(reinterpret_cast<uint8_t *>(this) + _active_handles_entry->offset);
}
