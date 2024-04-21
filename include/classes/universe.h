//
// Created by Administrator on 2024/3/20.
//

#ifndef UNIVERSE_H
#define UNIVERSE_H
#include "include_header.h"


namespace java_hotspot::universe {

    auto narrow_oop_base() -> uint8_t*;

    auto narrow_oop_shift() -> int;

    auto narrow_klass_shift() -> int;

    auto narrow_klass_base() -> uint8_t*;


}


#endif //UNIVERSE_H
