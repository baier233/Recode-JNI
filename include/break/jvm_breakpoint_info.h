//
// Created by Administrator on 2024/3/19.
//

#ifndef JVM_BREAKPOINT_INFO_H
#define JVM_BREAKPOINT_INFO_H

#include "include_header.h"
#include "method.h"

namespace jvm_internal {
    class breakpoint_info {
    public:
        static auto create(java_hotspot::method *method, int bci) -> breakpoint_info *;

        auto orig_bytecode() -> java_runtime::bytecodes;

        auto bci() -> int;

        auto name_index() -> uint16_t;

        auto signature_index() -> uint16_t;

        auto next() -> breakpoint_info *;

        auto set_orig_bytecode(java_runtime::bytecodes value) -> void;

        auto set_bci(int value) -> void;

        auto set_name_index(uint16_t value) -> void;

        auto set_signature_index(uint16_t value) -> void;

        auto set_next(breakpoint_info *value) -> void;
    };
}


#endif //JVM_BREAKPOINT_INFO_H
