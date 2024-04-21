//
// Created by Baier on 2024/4/21.
//

#include "JNIId.h"

java_hotspot::JNIid *java_hotspot::JNIid::get_next() {
    static VMStructEntry *_next_entry = JVMWrappers::find_type_fields("JNIid").value().get()[
            "_next"];
    if (!_next_entry) return nullptr;
    return *reinterpret_cast<JNIid **>(reinterpret_cast<uint8_t *>(this) + _next_entry->offset);
}

int java_hotspot::JNIid::get_offset() {
    static VMStructEntry *_offset_entry = JVMWrappers::find_type_fields("JNIid").value().get()[
            "_offset"];
    if (!_offset_entry) return 0;
    return *reinterpret_cast<int *>(reinterpret_cast<uint8_t *>(this) + _offset_entry->offset);
}

java_hotspot::JNIid::JNIid(java_hotspot::instance_klass *pKlass, int i, java_hotspot::JNIid *pIid) {
    set_holder(pKlass);
    set_offset(i);
    set_next(pIid);
}

void java_hotspot::JNIid::set_offset(int offset) {
    static VMStructEntry *_offset_entry = JVMWrappers::find_type_fields("JNIid").value().get()[
            "_offset"];
    if (!_offset_entry) return;
    *reinterpret_cast<int *>(reinterpret_cast<uint8_t *>(this) + _offset_entry->offset) = offset;
}

void java_hotspot::JNIid::set_holder(java_hotspot::instance_klass *klass) {
    static VMStructEntry *_offset_entry = JVMWrappers::find_type_fields("JNIid").value().get()[
            "_holder"];
    if (!_offset_entry) return;
    *reinterpret_cast<instance_klass **>(reinterpret_cast<uint8_t *>(this) + _offset_entry->offset) = klass;
}

void java_hotspot::JNIid::set_next(java_hotspot::JNIid *next) {
    static VMStructEntry *_next_entry = JVMWrappers::find_type_fields("JNIid").value().get()[
            "_next"];
    if (!_next_entry) return;
    *reinterpret_cast<JNIid **>(reinterpret_cast<uint8_t *>(this) + _next_entry->offset) = next;
}