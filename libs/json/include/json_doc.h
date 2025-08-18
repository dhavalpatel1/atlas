#pragma once

#include "core_string.h"

// Compatible with RFC 7159 json (https://datatracker.ietf.org/doc/html/rfc7159)
typedef struct sJsonDoc JsonDoc;

typedef enum {
    JsonType_Array,
    JsonType_Object,
    JsonType_String,
    JsonType_Number,
    JsonType_Bool,
    JsonType_Null,
} JsonType;

typedef enum {
    JsonParent_None,
    JsonParent_Array,
    JsonParent_Object,
} JsonParent;

typedef u32 JsonVal;

typedef struct {
    String name;
    JsonVal value;
} JsonFieldItr;

#define json_for_elems(_DOC_, _ARRAY_, _VAR_, ...)                                            \
    {                                                                                         \
        for (JsonVal _VAR_ = json_elem_begin(_DOC_, _ARRAY_); !sentinel_check(_VAR_); _VAR_ = \
        json_elem_next(_DOC_, _VAR_)) {                                                       \
            __VA_ARGS__                                                                       \
        }                                                                                     \
    }

#define json_for_fields(_DOC_, _OBJECT_, _VAR_, ...)                                               \
    {                                                                                              \
        for (JsonFieldItr _VAR_ = json_field_begin(_DOC_, _OBJECT_); !sentinel_check(_VAR_.value); \
        _VAR_ = json_field_next(_DOC_, _VAR_.value)) {                                             \
            __VA_ARGS__                                                                            \
        }                                                                                          \
    }

JsonDoc* json_create(Allocator*, usize valueCapacity);

void json_destroy(JsonDoc*);

JsonVal json_add_array(JsonDoc*);

JsonVal json_add_object(JsonDoc*);

JsonVal json_add_string(JsonDoc*, String);

JsonVal json_add_number(JsonDoc*, f64);

JsonVal json_add_bool(JsonDoc*, bool);

JsonVal json_add_null(JsonDoc*);

void json_add_elem(JsonDoc*, JsonVal array, JsonVal elem);

bool json_add_field(JsonDoc*, JsonVal object, JsonVal name, JsonVal val);

bool json_add_field_str(JsonDoc*, JsonVal object, String name, JsonVal val);

JsonType json_type(const JsonDoc*, JsonVal);

JsonParent json_parent(const JsonDoc*, JsonVal);

JsonVal json_elem(const JsonDoc*, JsonVal array, u32 idx);

u32 json_elem_count(const JsonDoc*, JsonVal array);

JsonVal json_elem_begin(const JsonDoc*, JsonVal array);

JsonVal json_elem_next(const JsonDoc*, JsonVal elem);

JsonVal json_field(const JsonDoc*, JsonVal object, String name);

u32 json_field_count(const JsonDoc*, JsonVal object);

JsonFieldItr json_field_begin(const JsonDoc*, JsonVal object);

JsonFieldItr json_field_next(const JsonDoc*, JsonVal fieldVal);

String json_string(const JsonDoc*, JsonVal);

f64 json_number(const JsonDoc*, JsonVal);

bool json_bool(const JsonDoc*, JsonVal);