//
// Created by Baier on 2024/4/21.
//

#ifndef RECODE_JNI_JNIID_H
#define RECODE_JNI_JNIID_H
#include "include_header.h"
#include "klass.h"


namespace java_hotspot {
    class JNIid{
        JNIid* get_next();
        int get_offset();
        void set_holder(java_hotspot::instance_klass* klass);
        void set_offset(int offset);
        void set_next(JNIid* next);

    public:
        JNIid* find(int offset) {
            JNIid* current = this;
            while (current != nullptr) {
                if (current->get_offset() == offset) return current;
                current = current->get_next();
            }
            return nullptr;
        }

        JNIid(java_hotspot::instance_klass *pKlass, int i, JNIid *pIid);
    };
}



#endif //RECODE_JNI_JNIID_H
