//
// Created by Administrator on 2024/3/20.
//

#include "oop.h"

#include "universe.h"
#include "klass.h"


auto java_hotspot::oop_desc::get_klass() -> instance_klass * {
    return decode_klass_not_null(get_compressed_klass());
}

auto java_hotspot::oop_desc::get_compressed_klass() -> narrowKlass {
    static VMStructEntry *_compressed_klass_entry = JVMWrappers::find_type_fields("oopDesc").value().get()[
        "_metadata._compressed_klass"];
    if (!_compressed_klass_entry) return 0;
    return *reinterpret_cast<narrowKlass *>(reinterpret_cast<uint8_t *>(this) + _compressed_klass_entry->offset);
}

void *java_hotspot::oop_desc::field_base(const int offset) const {
    return (void *) &((char *) this)[offset];
}

auto java_hotspot::oop_desc::obj_field(const int offset) const -> oop {
    return load_decode_heap_oop(obj_field_addr<narrowOop>(offset));
}

auto java_hotspot::oop_desc::load_decode_heap_oop(const narrowOop *p) -> oop {
    return decode_heap_oop(*p);
}

auto java_hotspot::oop_desc::load_decode_heap_oop(const oop *p) -> oop {
    return *p;
}

auto java_hotspot::oop_desc::decode_heap_oop(const narrowOop v) -> oop {
    return is_null(v) ? static_cast<oop>(nullptr) : decode_heap_oop_not_null(v);
}

auto java_hotspot::oop_desc::decode_heap_oop_not_null(const narrowOop v) -> oop {
    uint8_t *base = universe::narrow_oop_base();
    const int shift = universe::narrow_oop_shift();
    const auto result = static_cast<oop>(reinterpret_cast<void *>(
        reinterpret_cast<uintptr_t>(base) + (static_cast<uintptr_t>(v) << shift)));
    return result;
}

bool java_hotspot::oop_desc::is_null(const narrowOop obj) {
    return obj == 0;
}
