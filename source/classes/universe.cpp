//
// Created by Administrator on 2024/3/20.
//

#include "universe.h"

#include "jvm_internal.h"

auto java_hotspot::universe::narrow_oop_base() -> uint8_t * {
    static VMStructEntry *_narrow_klass_base_entry = JVMWrappers::find_type_fields("Universe").value().get()[
        "_narrow_klass._base"];
    if (!_narrow_klass_base_entry) {
        return nullptr;
    }
    return *reinterpret_cast<uint8_t **>(static_cast<uint8_t *>(_narrow_klass_base_entry->address));
}

auto java_hotspot::universe::narrow_oop_shift() -> int {
    static VMStructEntry *_narrow_klass_shift_entry = JVMWrappers::find_type_fields("Universe").value().get()[
        "_narrow_klass._shift"];
    if (!_narrow_klass_shift_entry) {
        return 0;
    }
    return *static_cast<int *>(_narrow_klass_shift_entry->address);
}

auto java_hotspot::universe::narrow_klass_shift() -> int {
    static VMStructEntry *_narrow_klass_shift_entry = JVMWrappers::find_type_fields("Universe").value().get()[
        "_narrow_klass._shift"];
    if (!_narrow_klass_shift_entry) {
        return 0;
    }
    return *static_cast<int *>(_narrow_klass_shift_entry->address);
}

auto java_hotspot::universe::narrow_klass_base() -> uint8_t * {
    static VMStructEntry *_narrow_klass_base_entry = JVMWrappers::find_type_fields("Universe").value().get()[
        "_narrow_klass._base"];
    if (!_narrow_klass_base_entry) {
        return nullptr;
    }
    return *reinterpret_cast<uint8_t **>(static_cast<uint8_t *>(_narrow_klass_base_entry->address));
}
