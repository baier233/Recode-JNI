//
// Created by Administrator on 2024/3/17.
//

#ifndef JVM_BREAK_POINTS_H
#define JVM_BREAK_POINTS_H

#include <functional>
#include <map>

#include "include_header.h"
#include "detour_hook.h"
#include "break_point_info.h"


extern "C" {
uintptr_t __forceinline jhook_get_r14_address();
uintptr_t __forceinline jhook_get_r13_address();
void __forceinline jhook_set_r13_address(void* value);
uintptr_t _forceinline jhook_get_rbp_address();
}


namespace jvm_break_points {
    inline detour_hook<breakpoint_callback_t> breakpoint_hook;
    inline detour_hook<original_bytecode_handler_t> original_bytecode_hook;
    inline std::map<uintptr_t, unsigned int> original_bytecodes;
    inline std::map<uintptr_t, native_callback_t> breakpoint_callbacks;

    auto breakpoint_callback(
        java_hotspot::java_thread *java_thread,
        java_hotspot::method *method,
        uintptr_t bytecode_address
    ) -> void;

    auto breakpoint_handler(
        java_hotspot::java_thread *java_thread,
        java_hotspot::method *method,
        uintptr_t bytecode_address,
        uintptr_t parameters
    ) -> void;

    auto original_bytecode_handler(
        java_hotspot::java_thread *java_thread,
        java_hotspot::method *method,
        uintptr_t bytecode_address
    ) -> unsigned int;

    auto set_breakpoint(
        java_hotspot::method *method,
        uintptr_t offset,
        const native_callback_t &callback
    ) -> bool;


    auto set_breakpoint_with_original_code(
        java_hotspot::method *method,
        uintptr_t offset,
        uint8_t original,
        const native_callback_t &callback
    ) -> bool;

    auto remove_breakpoint(
        java_hotspot::method *method,
        uintptr_t offset
    ) -> void;

    auto remove_all_breakpoints(
        java_hotspot::method *method
    ) -> void;
}


#endif //JVM_BREAK_POINTS_H
