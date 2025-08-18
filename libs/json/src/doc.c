/**
 * @file doc.c
 * @brief JSON document implementation
 *
 * Implements the core JSON document structure for storing and managing JSON values.
 * Provides value creation, relationship management, and property access functions
 * for all JSON types including objects, arrays, and primitives.
 */

#include "core_alloc.h"
#include "core_diag.h"
#include "core_dynarray.h"

#include "json_doc.h"

/**
 * @brief Internal data structure for JSON arrays
 * 
 * Stores array metadata including linked list pointers for efficient
 * element traversal and count tracking for O(1) size queries.
 */
typedef struct {
    JsonVal elemHead;   /**< Handle to first array element */
    JsonVal elemTail;   /**< Handle to last array element */
    u32 elemCount;      /**< Number of elements in the array */
} JsonArrayData;

/**
 * @brief Internal data structure for JSON objects
 * 
 * Stores object metadata including the head of the field linked list
 * and count tracking for O(1) field count queries.
 */
typedef struct {
    JsonVal fieldHead;  /**< Handle to first object field (name-value pair) */
    u32 fieldCount;     /**< Number of fields in the object */
} JsonObjectData;

/**
 * @brief Internal data structure for all JSON values
 * 
 * Unified value representation that stores type information, parent relationship,
 * linking data for collections, and the actual value data in a discriminated union.
 * The typeAndParent field packs both JsonType (lower 16 bits) and JsonParent 
 * (upper 16 bits) for efficient storage.
 */
typedef struct {
    u32 typeAndParent;      /**< Packed type (bits 0-15) and parent type (bits 16-31) */
    JsonVal next;           /**< Next sibling in parent collection (array/object) */

    union {
        JsonArrayData val_array;    /**< Array data (when type is JsonType_Array) */
        JsonObjectData val_object;  /**< Object data (when type is JsonType_Object) */
        String val_string;          /**< String value (when type is JsonType_String) */
        f64 val_number;             /**< Numeric value (when type is JsonType_Number) */
        bool val_bool;              /**< Boolean value (when type is JsonType_Bool) */
    };
} JsonValData;

/**
 * @brief Main JSON document structure
 * 
 * Contains the complete JSON document state including all values stored
 * in a dynamic array for efficient access by handle, and the allocator
 * used for memory management of strings and the document itself.
 */
struct sJsonDoc {
    DynArray values;    /**< Dynamic array storing all JsonValData structures */
    Allocator* alloc;   /**< Allocator for string storage and document memory */
};

/**
 * @brief Get the internal data structure for a JSON value
 * @param doc JSON document containing the value
 * @param val JSON value handle
 * @return Pointer to the internal value data structure
 *
 * Provides access to the underlying value data with bounds checking.
 * Used internally to access value properties and content.
 */
static JsonValData* json_val_data(const JsonDoc* doc, const JsonVal val) {
    diag_assert_msg(val < doc->values.size, "Out of bounds JsonVal");

    return dynarray_at_t(&doc->values, val, JsonValData);
}

/**
 * @brief Add a new value data structure to the document
 * @param doc JSON document to add the value to
 * @param data Value data structure to add
 * @return JSON value handle for the newly added value
 *
 * Allocates a new value slot in the document and returns a handle
 * that can be used to reference the value later.
 */
static JsonVal json_add_data(JsonDoc* doc, JsonValData data) {
    const JsonVal val = (JsonVal)doc->values.size;
    *dynarray_push_t(&doc->values, JsonValData) = data;

    return val;
}

JsonDoc* json_create(Allocator* alloc, usize valueCapacity) {
    JsonDoc* doc = alloc_alloc_t(alloc, JsonDoc);
    *doc = (JsonDoc) {
        .values = dynarray_create_t(alloc, JsonValData, valueCapacity),
        .alloc = alloc,
    };

    return doc;
}

void json_destroy(JsonDoc* doc) {
    dynarray_for_t(&doc->values, JsonValData, data, {
        switch (data->typeAndParent & 0xFFFF) {
            case JsonType_String: {
                if (data->val_string.ptr) {
                    string_free(doc->alloc, data->val_string);
                }
            } break;

            default: {
            
            } break;
        }
    });

    dynarray_destroy(&doc->values);
    alloc_free_t(doc->alloc, doc);
}

JsonVal json_add_array(JsonDoc* doc) {
    return json_add_data(doc, (JsonValData) {
        .typeAndParent = JsonType_Array,
        .next = sentinel_u32,
        .val_array = {
            .elemHead = sentinel_u32,
            .elemTail = sentinel_u32,
            .elemCount = 0,
        }
    });
}

JsonVal json_add_object(JsonDoc* doc) {
    return json_add_data(doc, (JsonValData) {
        .typeAndParent = JsonType_Object,
        .next = sentinel_u32,
        .val_object = {
            .fieldHead = sentinel_u32,
            .fieldCount = 0,
        }
    });
}

JsonVal json_add_string(JsonDoc* doc, const String str) {
    return json_add_data(doc, (JsonValData) {
        .typeAndParent = JsonType_String,
        .next = sentinel_u32,
        .val_string = string_is_empty(str) ? string_empty : string_dup(doc->alloc, str),
    });
}

JsonVal json_add_number(JsonDoc* doc, f64 number) {
    return json_add_data(doc, (JsonValData) {
        .typeAndParent = JsonType_Number,
        .next = sentinel_u32,
        .val_number = number,
    });
}

JsonVal json_add_bool(JsonDoc* doc, bool boolean) {
    return json_add_data(doc, (JsonValData) {
        .typeAndParent = JsonType_Bool,
        .next = sentinel_u32,
        .val_bool = boolean,
    });
}

JsonVal json_add_null(JsonDoc* doc) {
    return json_add_data(doc, (JsonValData) {
        .typeAndParent = JsonType_Null,
        .next = sentinel_u32,
    });
}

void json_add_elem(JsonDoc* doc, JsonVal array, JsonVal elem) {
    diag_assert_msg(json_parent(doc, elem) == JsonParent_None, "Given values is already parented");
    diag_assert_msg(json_type(doc, array) == JsonType_Array, "Invalid array value");
    // TODO: Check for indirect cycles
    diag_assert_msg(array != elem, "Arrays cannot contain cycles");

    JsonValData* arrayData = json_val_data(doc, array);
    JsonValData* elemData = json_val_data(doc, elem);

    if (sentinel_check(arrayData->val_array.elemTail)) {
        arrayData->val_array.elemHead = elem;
        arrayData->val_array.elemTail = elem;
    } else {
        json_val_data(doc, arrayData->val_array.elemTail)->next = elem;
        arrayData->val_array.elemTail = elem;
    }

    elemData->typeAndParent |= (u32)JsonParent_Array << 16;
    ++arrayData->val_array.elemCount;
}

bool json_add_field(JsonDoc* doc, JsonVal object, JsonVal name, JsonVal val) {
    diag_assert_msg(json_type(doc, object) == JsonType_Object, "Invalid object value");
    // TODO: Check for indirect cycles
    diag_assert_msg(object != name && object != val, "Objects cannot contain cycles");
    diag_assert_msg(json_parent(doc, name) == JsonParent_None, "Given name is already parented");
    diag_assert_msg(!string_is_empty(json_string(doc, name)), "Field name cannot be empty");
    diag_assert_msg(json_parent(doc, val) == JsonParent_None, "Given value is already parented");

    const String nameStr = json_string(doc, name);
    JsonValData* objectData = json_val_data(doc, object);

    JsonVal* link = &objectData->val_object.fieldHead;
    while (!sentinel_check(*link)) {
        const JsonValData* nameValData = json_val_data(doc, *link);
        if (string_eq(nameValData->val_string, nameStr)) {
            return false;
        }

        link = &json_val_data(doc, nameValData->next)->next;
    }

    *link = name;
    ++objectData->val_object.fieldCount;
    json_val_data(doc, name)->next = val;
    json_val_data(doc, name)->typeAndParent |= (u32)JsonParent_Object << 16;
    json_val_data(doc, val)->typeAndParent |= (u32)JsonParent_Object << 16;

    return true;
}

bool json_add_field_str(JsonDoc* doc, JsonVal object, String name, JsonVal val) {
    return json_add_field(doc, object, json_add_string(doc, name), val);
}

JsonType json_type(const JsonDoc* doc, JsonVal val) {
    return (JsonType)(json_val_data(doc, val)->typeAndParent & 0xFFFF);
}

JsonParent json_parent(const JsonDoc* doc, JsonVal val) {
    return (JsonParent)(json_val_data(doc, val)->typeAndParent >> 16);
}

JsonVal json_elem(const JsonDoc* doc, JsonVal array, u32 idx) {
    diag_assert_msg(json_type(doc, array) == JsonType_Array, "Invalid array value");

    JsonVal link = json_val_data(doc, array)->val_array.elemHead;
    for (u32 i = 0; !sentinel_check(link); ++i) {
        if (i == idx) {
            return link;
        }

        link = json_val_data(doc, link)->next;
    }

    return sentinel_u32;
}

u32 json_elem_count(const JsonDoc* doc, JsonVal array) {
    diag_assert_msg(json_type(doc, array) == JsonType_Array, "Invalid array value");

    return json_val_data(doc, array)->val_array.elemCount;
}

JsonVal json_elem_begin(const JsonDoc* doc, JsonVal array) {
    diag_assert_msg(json_type(doc, array) == JsonType_Array, "Invalid array value");

    return json_val_data(doc, array)->val_array.elemHead;
}

JsonVal json_elem_next(const JsonDoc* doc, JsonVal elem) {
    diag_assert_msg(json_parent(doc, elem) == JsonParent_Array, "Invalid array value");

    return json_val_data(doc, elem)->next;
}

JsonVal json_field(const JsonDoc* doc, JsonVal object, String name) {
    diag_assert_msg(json_type(doc, object) == JsonType_Object, "Invalud object value");

    JsonValData* objectData = json_val_data(doc, object);

    JsonVal link = objectData->val_object.fieldHead;
    while (!sentinel_check(link)) {
        const JsonValData* nameValData = json_val_data(doc, link);
        if (string_eq(nameValData->val_string, name)) {
            return nameValData->next;
        }

        link = json_val_data(doc, nameValData->next)->next;
    }

    return sentinel_u32;
}

u32 json_field_count(const JsonDoc* doc, JsonVal object) {
    diag_assert_msg(json_type(doc, object) == JsonType_Object, "Invalid object value");

    return json_val_data(doc, object)->val_object.fieldCount;
}

JsonFieldItr json_field_begin(const JsonDoc* doc, JsonVal object) {
    diag_assert_msg(json_type(doc, object) == JsonType_Object, "Invalid object value");

    JsonValData* objectData = json_val_data(doc, object);
    if (sentinel_check(objectData->val_object.fieldHead)) {
        return (JsonFieldItr){
            .name = string_empty,
            .value = sentinel_u32,
        };
    }

    JsonValData* nameData = json_val_data(doc, objectData->val_object.fieldHead);

    return (JsonFieldItr) {
        .name = nameData->val_string,
        .value = nameData->next,
    };
}

JsonFieldItr json_field_next(const JsonDoc* doc, JsonVal fieldVal) {
    diag_assert_msg(json_parent(doc, fieldVal) == JsonParent_Object, "Invalid field value");

    JsonValData* itrValData = json_val_data(doc, fieldVal);
    if (sentinel_check(itrValData->next)) {
        return (JsonFieldItr) {
            .name = string_empty,
            .value = sentinel_u32,
        };
    }

    JsonValData* nameData = json_val_data(doc, itrValData->next);
    return (JsonFieldItr) {
        .name = nameData->val_string,
        .value = nameData->next,
    };
}

String json_string(const JsonDoc* doc, JsonVal val) {
    diag_assert_msg(json_type(doc, val) == JsonType_String, "Given JsonVal is not a string");

    return json_val_data(doc, val)->val_string;
}

f64 json_number(const JsonDoc* doc, JsonVal val) {
    diag_assert_msg(json_type(doc, val) == JsonType_Number, "Given JsonVal is not a number");

    return json_val_data(doc, val)->val_number;
}

bool json_bool(const JsonDoc* doc, JsonVal val) {
    diag_assert_msg(json_type(doc, val) == JsonType_Bool, "Given JsonVal is not a boolean");

    return json_val_data(doc, val)->val_bool;
}