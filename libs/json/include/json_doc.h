#pragma once

#include "core_string.h"

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

#define json_add_string_lit(_DOC_, _STRING_LIT_) json_add_string(_DOC_, string_lit(_STRING_LIT_))

JsonDoc* json_create(Allocator* allocator, usize valueCapacity);

void json_destroy(JsonDoc* doc);

JsonVal json_add_array(JsonDoc* doc);

JsonVal json_add_object(JsonDoc* doc);

JsonVal json_add_string(JsonDoc* doc, String str);

JsonVal json_add_number(JsonDoc* doc, f64 num);

JsonVal json_add_bool(JsonDoc* doc, bool value);

JsonVal json_add_null(JsonDoc* doc);

void json_add_elem(JsonDoc* doc, JsonVal array, JsonVal elem);

bool json_add_field(JsonDoc* doc, JsonVal object, JsonVal name, JsonVal val);

bool json_add_field_str(JsonDoc* doc, JsonVal object, String name, JsonVal val);

JsonType json_type(const JsonDoc* doc, JsonVal val);

JsonParent json_parent(const JsonDoc* doc, JsonVal val);

JsonVal json_elem(const JsonDoc* doc, JsonVal array, u32 idx);

u32 json_elem_count(const JsonDoc* doc, JsonVal array);

JsonVal json_elem_begin(const JsonDoc* doc, JsonVal array);

JsonVal json_elem_next(const JsonDoc* doc, JsonVal elem);

JsonVal json_field(const JsonDoc* doc, JsonVal object, String name);

u32 json_field_count(const JsonDoc* doc, JsonVal object);

JsonFieldItr json_field_begin(const JsonDoc* doc, JsonVal object);

JsonFieldItr json_field_next(const JsonDoc* doc, JsonVal fieldVal);

String json_string(const JsonDoc* doc, JsonVal val);

f64 json_number(const JsonDoc* doc, JsonVal val);

bool json_bool(const JsonDoc* doc, JsonVal val);
