//
// Created by Administrator on 2024/3/15.
//

#include "klass.h"

#include "java_interop.h"
#include "jvm_internal.h"
#include "JNIId.h"

auto java_hotspot::instance_klass::get_name() -> symbol * {
    static VMStructEntry *_name_entry = JVMWrappers::find_type_fields("Klass").value().get()["_name"];
    if (!_name_entry)
        return nullptr;
    return *reinterpret_cast<symbol **>(reinterpret_cast<uint8_t *>(this) + _name_entry->offset);
}

auto java_hotspot::instance_klass::get_constants() -> const_pool * {
    static VMStructEntry *_constants_entry = JVMWrappers::find_type_fields("InstanceKlass").value().get()["_constants"];
    if (!_constants_entry) return nullptr;
    return *reinterpret_cast<const_pool **>(reinterpret_cast<uint8_t *>(this) + _constants_entry->offset);
}

auto java_hotspot::instance_klass::get_methods() -> array<method *> * {
    static VMStructEntry *_methods_entry = JVMWrappers::find_type_fields("InstanceKlass").value().get()["_methods"];
    if (!_methods_entry) return nullptr;
    return *reinterpret_cast<array<method *> **>(reinterpret_cast<uint8_t *>(this) + _methods_entry->offset);
}

auto java_hotspot::instance_klass::get_fields() -> array<uint16_t> * {
    static VMStructEntry *_fields_entry = JVMWrappers::find_type_fields("InstanceKlass").value().get()["_fields"];
    if (!_fields_entry) return nullptr;
    return *reinterpret_cast<array<uint16_t> **>(reinterpret_cast<uint8_t *>(this) + _fields_entry->offset);
}

auto java_hotspot::instance_klass::set_breakpoints(jvm_internal::breakpoint_info *breakpoints) -> void {
    static VMStructEntry *_breakpoints_entry = JVMWrappers::find_type_fields("InstanceKlass").value().get()[
        "_breakpoints"];
    if (!_breakpoints_entry) return;
    *reinterpret_cast<jvm_internal::breakpoint_info **>(reinterpret_cast<uint8_t *>(this) + _breakpoints_entry->offset)
            = breakpoints;
}

auto java_hotspot::instance_klass::get_breakpoints() -> jvm_internal::breakpoint_info * {
    static VMStructEntry *_breakpoints_entry = JVMWrappers::find_type_fields("InstanceKlass").value().get()[
        "_breakpoints"];
    if (!_breakpoints_entry) return nullptr;
    return *reinterpret_cast<jvm_internal::breakpoint_info **>(
        reinterpret_cast<uint8_t *>(this) + _breakpoints_entry->offset);
}

auto java_hotspot::instance_klass::get_super_klass() -> instance_klass * {
    static VMStructEntry *_super_entry = JVMWrappers::find_type_fields("Klass").value().get()["_super"];
    if (!_super_entry) return nullptr;
    return *reinterpret_cast<instance_klass **>(reinterpret_cast<uint8_t *>(this) + _super_entry->offset);
}

auto java_hotspot::instance_klass::get_java_mirror() -> oop_desc * {
    static VMStructEntry *_java_mirror_entry = JVMWrappers::find_type_fields("Klass").value().get()["_java_mirror"];
    if (!_java_mirror_entry) return nullptr;
    return *reinterpret_cast<oop_desc **>(reinterpret_cast<uint8_t *>(this) + _java_mirror_entry->offset);
}

auto java_hotspot::instance_klass::find_field_info(
    const std::string &field_name,
    const std::string &field_signature
) -> std::tuple<field_info *, instance_klass *> {
    auto current_klass = this;
    while (current_klass) {
        const auto fields = current_klass->get_fields();
        const auto fields_length = fields->get_length() / field_slots;
        const auto fields_data = fields->get_data();
        const auto name_and_signature_param = field_name + field_signature;
        const auto constants = current_klass->get_constants();
        for (auto i = 0; i < fields_length; i++) {
            const auto field = field_info::from_field_array(fields_data, i);
            if (!field) {
                continue;
            }
            if (
                const auto name_and_signature_field = field->get_name(constants)->to_string() +
                                                      field->get_signature(constants)->to_string();
                !name_and_signature_param._Equal(name_and_signature_field)
            ) {
                continue;
            }
            return {field, current_klass};
        }
        current_klass = current_klass->get_super_klass();
    }
    throw std::runtime_error("find_field_error");
}



std::unordered_map<java_hotspot::instance_klass*, std::map<std::string, std::shared_ptr<std::vector<uint8_t>>>> methodsMap;
std::unordered_map<std::string, uint8_t*> methodIdMap;

/// \fixme @baieroops: shitcode and bad performance
auto java_hotspot::instance_klass::get_method_id(const char* name, const char* sign, bool isStatic) -> jmethodID {
    if (!this) throw std::runtime_error("klass instance is null");

    std::string method_name_and_sign = std::string(name) + std::string(sign);
    if (auto it = methodsMap.find(this); it != methodsMap.end()){
        if (auto methodDataIt = it->second.find(method_name_and_sign); methodDataIt != it->second.end()){
            auto it = methodIdMap.find(this->get_name()->to_string()+"_" + method_name_and_sign);
            if(it != methodIdMap.end()){
                return reinterpret_cast<jmethodID>(&it->second);
            }
        }
    }

    const auto methods = this->get_methods();
    if(!methods) return nullptr;

    const auto data = methods->get_data();
    const auto length = methods->get_length();
    for (auto i = 0; i < length; i++) {
        const auto method = data[i];
        if (!method) continue;

        auto target_method_name_and_sign = (method->get_name() + method->get_signature());
        if (target_method_name_and_sign._Equal(method_name_and_sign)) {
            auto methodType = JVMWrapper::from_instance("Method", method).value();
            uint64_t method_size = methodType.size();
            auto cachedMethod = std::make_shared<std::vector<uint8_t>>((uint8_t *)methodType.get_instance(), &((uint8_t *)methodType.get_instance())[method_size]);
            //specially resolve for non-static method
            if (!isStatic){
                const auto& callableMethod = cachedMethod;
                auto callable = JVMWrapper::from_instance("Method", (void*)callableMethod->data()).value();
                int *callable_vtable_index = callable.get_field<int>("_vtable_index").value();
                //
                *callable_vtable_index = nonvirtual_vtable_index;
            }


            methodsMap[this][method_name_and_sign] = cachedMethod;
            methodIdMap[this->get_name()->to_string()+"_" + method_name_and_sign] = cachedMethod->data();

            return reinterpret_cast<jmethodID>(&methodIdMap[this->get_name()->to_string()+"_" + method_name_and_sign]);
        }
    }

    return nullptr;
}




auto
java_hotspot::instance_klass::get_field_id(const char *name, const char *sign, bool isStatic) -> jfieldID {
    if (!this || !name || !sign) return nullptr;
    auto info = this->find_field_info(name, sign);

    auto field = std::get<0>(info);
    auto holder = std::get<1>(info);
    if (!field || !holder) return nullptr;
    if (!isStatic) {
        return java_hotspot::instance_klass::to_instance_jfieldID(field->get_offset());
    }
    return holder->to_static_jfieldID(field->get_offset());
}

auto java_hotspot::instance_klass::to_static_jfieldID(int offset) -> jfieldID {
    JNIid *id = this->jni_id_for_offset(offset);
    return (jfieldID) id;;
}

auto java_hotspot::instance_klass::jni_ids() -> java_hotspot::JNIid * {
    static VMStructEntry *_jni_ids_entry = JVMWrappers::find_type_fields("InstanceKlass").value().get()["_jni_ids"];
    if (!_jni_ids_entry) return nullptr;
    return *reinterpret_cast<JNIid **>(reinterpret_cast<uint8_t *>(this) + _jni_ids_entry->offset);
}

void java_hotspot::instance_klass::set_jni_ids(java_hotspot::JNIid *ids) {
    static VMStructEntry *_jni_ids_entry = JVMWrappers::find_type_fields("InstanceKlass").value().get()["_jni_ids"];
    if (!_jni_ids_entry) return;
    *reinterpret_cast<JNIid **>(reinterpret_cast<uint8_t *>(this) + _jni_ids_entry->offset) = ids;

}

auto java_hotspot::instance_klass::jni_id_for_offset(int offset) -> java_hotspot::JNIid * {
    JNIid *probe = jni_ids() == nullptr ? nullptr : jni_ids()->find(offset);
    if (probe == nullptr) {
        probe = new JNIid(this, offset, this->jni_ids());
        this->set_jni_ids(probe);
    }
    return probe;
}

auto java_hotspot::instance_klass::to_instance_jfieldID(int offset) -> jfieldID {
    intptr_t as_uint = ((offset & large_offset_mask) << offset_shift) | instance_mask_in_place;
    auto result = (jfieldID) as_uint;
    return result;
}

auto java_hotspot::get_static_object_field(
    field_info *field_info,
    instance_klass *current_klass
) -> jobject {
    const auto java_mirror = current_klass->get_java_mirror();
    const auto address = java_mirror->obj_field(static_cast<int>(field_info->get_offset()));
    java_interop::attach_temp_thread();
    /* this is a local reference, maybe can delete it */
    const auto make_local = jni_handles_internal::make_local(address);
    java_interop::detach_temp_thread();
    return make_local;
}

auto java_hotspot::get_objet_field(
    field_info *field_info,
    jobject obj
) -> jobject {
    const auto result = jni_handles::resolve_non_null(obj);
    java_interop::attach_temp_thread();
    const auto ret = jni_handles_internal::make_local(result->obj_field(static_cast<int>(field_info->get_offset())));
    java_interop::detach_temp_thread();
    std::cout << result->obj_field(static_cast<int>(field_info->get_offset())) << std::endl;
    return ret;
}

auto java_hotspot::set_objet_field(field_info *field_info, jobject obj, jobject value) -> void {
    const auto result = jni_handles::resolve_non_null(obj);
    jni_handles_internal::oop_store(
        result->obj_field_addr<narrowOop>(static_cast<int>(field_info->get_offset())),
        jni_handles::resolve(value)
    );
}

auto java_hotspot::set_static_object_field(
    field_info *field_info,
    instance_klass *current_klass,
    jobject obj
) -> void {
    const auto java_mirror = current_klass->get_java_mirror();
    jni_handles_internal::oop_store(
        java_mirror->obj_field_addr<narrowOop>(static_cast<int>(field_info->get_offset())),
        jni_handles::resolve(obj)
    );
}

auto java_hotspot::get_objet_field_detach(field_info *field_info, jobject obj) -> jobject {
    const auto result = jni_handles::resolve_non_null(obj);
    const auto ret = result->obj_field(static_cast<int>(field_info->get_offset()));
    /* Maybe need to delete it properly */
    const auto oop_pointer = new oop(ret);
    return reinterpret_cast<jobject>(oop_pointer);
}
