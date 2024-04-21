//
// Created by Administrator on 2024/3/15.
//

#ifndef MEMORY_UTILITY_H
#define MEMORY_UTILITY_H

#include "../include_header.h"

inline std::string hex_to_bytes(std::string hex_string) {
    std::string bytes;
    std::erase_if(hex_string, isspace);
    for (uint32_t i = 0; i < hex_string.length(); i += 2) {
        if (static_cast<uint8_t>(hex_string[i]) == '?') {
            bytes += '?';
            i -= 1;

            continue;
        }
        const uint8_t byte = static_cast<uint8_t>(std::strtol(hex_string.substr(i, 2).c_str(), nullptr, 16));
        bytes += byte;
    }
    return bytes;
}

inline uintptr_t scan(const char *pattern, const uintptr_t start, const uintptr_t end) {
    const std::string signature = hex_to_bytes(pattern);

    auto base = reinterpret_cast<uint8_t *>(start);
    const auto last = reinterpret_cast<uint8_t *>(end);
    const uint8_t first = signature.at(0);

    for (; base < last; ++base) {
        if (*base != first)
            continue;

        uint8_t *bytes = base;

        for (auto sig = (uint8_t *) signature.c_str(); *sig; ++sig, ++bytes) {
            if (*sig == '?')
                continue;

            if (*bytes != *sig)
                goto end;
        }

        return reinterpret_cast<uintptr_t>(base);

    end:;
    }

    return 0;
}

inline uint8_t *pattern_scan(HINSTANCE module, const std::vector<std::uint8_t> &pattern, const char *mask,
                             std::int32_t offset) {
    const auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
    const auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(module + dos->e_lfanew);
    const auto module_size = nt->OptionalHeader.SizeOfImage;
    const auto in_scan = [&]()-> uint8_t *{
        const std::uint32_t pattern_length = strlen(mask);
        const auto based = reinterpret_cast<std::uint8_t *>(module);
        for (int i = 0; i < module_size - pattern_length; ++i) {
            bool found = true;
            for (int j = 0; j < pattern_length; j++) {
                if (mask[j] != '?' && based[i + j] != pattern[j]) {
                    found = false;
                    break;
                }
            }
            if (found)
                return &based[i];
        }
        return nullptr;
    };

    const auto address = in_scan();
    return address ? address : nullptr;
}

inline auto get_absolute_address(const uintptr_t call_address) -> void * {
    const auto call_offset_address = reinterpret_cast<uint8_t *>(call_address);
    const auto offset_ptr = reinterpret_cast<int32_t *>(static_cast<uint8_t *>(call_offset_address) + 1);
    const auto offset = *offset_ptr;
    const auto absolute_address = static_cast<PVOID>(
        static_cast<uint8_t *>(call_offset_address) + offset + 5);
    return absolute_address;
}

#endif //MEMORY_UTILITY_H
