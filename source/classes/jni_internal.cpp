//
// Created by Administrator on 2024/3/20.
//

#include "jni_internal.h"

#include "jvm_internal.h"
#include "jvm_thread.h"

auto java_hotspot::jni_id::get_offset() -> int {
    static VMStructEntry *_offset_entry = JVMWrappers::find_type_fields("JNIid").value().get()["_offset"];
    if (!_offset_entry) return 0;
    return *reinterpret_cast<int *>(reinterpret_cast<uint8_t *>(this) + _offset_entry->offset);
}

auto java_hotspot::jni_handles::resolve(const jobject handle) -> oop {
    oop result = (handle == nullptr ? static_cast<oop>(nullptr) : *reinterpret_cast<oop *>(handle));
    return result;
}

oop java_hotspot::jni_handles::resolve_non_null(jobject handle) {
    oop result = *reinterpret_cast<oop *>(handle);
    return result;
}

