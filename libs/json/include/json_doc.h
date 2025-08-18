/**
 * @file json_doc.h
 * @brief JSON document management and manipulation API
 *
 * This header provides the core JSON document interface for creating, manipulating,
 * and accessing JSON data structures. The implementation is compatible with RFC 7159
 * JSON specification and supports all standard JSON types: objects, arrays, strings,
 * numbers, booleans, and null values.
 */

#pragma once

#include "core_string.h"

/**
 * @brief Opaque JSON document structure
 *
 * Contains all JSON values and their relationships. Compatible with RFC 7159 JSON specification.
 * @see https://datatracker.ietf.org/doc/html/rfc7159
 */
typedef struct sJsonDoc JsonDoc;

/**
 * @brief JSON value types enumeration
 */
typedef enum {
    JsonType_Array,     /**< JSON array type */
    JsonType_Object,    /**< JSON object type */
    JsonType_String,    /**< JSON string type */
    JsonType_Number,    /**< JSON number type (IEEE 754 double precision) */
    JsonType_Bool,      /**< JSON boolean type */
    JsonType_Null,      /**< JSON null type */
} JsonType;

/**
 * @brief JSON value parent type enumeration
 */
typedef enum {
    JsonParent_None,    /**< Value has no parent (root value) */
    JsonParent_Array,   /**< Value is contained in an array */
    JsonParent_Object,  /**< Value is contained in an object */
} JsonParent;

/**
 * @brief Handle representing a JSON value within a document
 *
 * This is an opaque handle used to reference JSON values. Handles are only
 * valid within the document they were created in.
 */
typedef u32 JsonVal;

/**
 * @brief Iterator structure for JSON object fields
 */
typedef struct {
    String name;        /**< Field name */
    JsonVal value;      /**< Field value handle */
} JsonFieldItr;

/**
 * @brief Iterate over all elements in a JSON array
 * @param _DOC_ JSON document pointer
 * @param _ARRAY_ JSON array value handle
 * @param _VAR_ Variable name for the iteration (will be of type JsonVal)
 * @param ... Code block to execute for each element
 *
 * Example usage:
 * @code
 * json_for_elems(doc, arrayVal, elem, {
 *     printf("Element type: %d\n", json_type(doc, elem));
 * });
 * @endcode
 */
#define json_for_elems(_DOC_, _ARRAY_, _VAR_, ...)                                            \
    {                                                                                         \
        for (JsonVal _VAR_ = json_elem_begin(_DOC_, _ARRAY_); !sentinel_check(_VAR_); _VAR_ = \
        json_elem_next(_DOC_, _VAR_)) {                                                       \
            __VA_ARGS__                                                                       \
        }                                                                                     \
    }

/**
 * @brief Iterate over all fields in a JSON object
 * @param _DOC_ JSON document pointer
 * @param _OBJECT_ JSON object value handle
 * @param _VAR_ Variable name for the iteration (will be of type JsonFieldItr)
 * @param ... Code block to execute for each field
 *
 * Example usage:
 * @code
 * json_for_fields(doc, objectVal, field, {
 *     printf("Field: %.*s\n", fmt_text(field.name));
 * });
 * @endcode
 */
#define json_for_fields(_DOC_, _OBJECT_, _VAR_, ...)                                               \
    {                                                                                              \
        for (JsonFieldItr _VAR_ = json_field_begin(_DOC_, _OBJECT_); !sentinel_check(_VAR_.value); \
        _VAR_ = json_field_next(_DOC_, _VAR_.value)) {                                             \
            __VA_ARGS__                                                                            \
        }                                                                                          \
    }

/**
 * @brief Create a new JSON document
 * @param allocator Allocator to use for document memory
 * @param valueCapacity Initial capacity for JSON values (0 for default)
 * @return Pointer to the created JSON document, or null on failure
 */
JsonDoc* json_create(Allocator* allocator, usize valueCapacity);

/**
 * @brief Destroy a JSON document and free its resources
 * @param doc JSON document to destroy
 */
void json_destroy(JsonDoc* doc);

/**
 * @brief Add a new empty array to the document
 * @param doc JSON document
 * @return Handle to the created array
 */
JsonVal json_add_array(JsonDoc* doc);

/**
 * @brief Add a new empty object to the document
 * @param doc JSON document
 * @return Handle to the created object
 */
JsonVal json_add_object(JsonDoc* doc);

/**
 * @brief Add a string value to the document
 * @param doc JSON document
 * @param str String value to add
 * @return Handle to the created string value
 */
JsonVal json_add_string(JsonDoc* doc, String str);

/**
 * @brief Add a number value to the document
 * @param doc JSON document
 * @param num Number value to add (IEEE 754 double precision)
 * @return Handle to the created number value
 */
JsonVal json_add_number(JsonDoc* doc, f64 num);

/**
 * @brief Add a boolean value to the document
 * @param doc JSON document
 * @param value Boolean value to add
 * @return Handle to the created boolean value
 */
JsonVal json_add_bool(JsonDoc* doc, bool value);

/**
 * @brief Add a null value to the document
 * @param doc JSON document
 * @return Handle to the created null value
 */
JsonVal json_add_null(JsonDoc* doc);

/**
 * @brief Add an element to a JSON array
 * @param doc JSON document
 * @param array Array to add the element to
 * @param elem Element to add to the array
 */
void json_add_elem(JsonDoc* doc, JsonVal array, JsonVal elem);

/**
 * @brief Add a field to a JSON object using a JSON string as the key
 * @param doc JSON document
 * @param object Object to add the field to
 * @param name JSON string value to use as the field name
 * @param val Value to associate with the field name
 * @return true if field was added successfully, false if field already exists
 */
bool json_add_field(JsonDoc* doc, JsonVal object, JsonVal name, JsonVal val);

/**
 * @brief Add a field to a JSON object using a string literal as the key
 * @param doc JSON document
 * @param object Object to add the field to
 * @param name String literal to use as the field name
 * @param val Value to associate with the field name
 * @return true if field was added successfully, false if field already exists
 */
bool json_add_field_str(JsonDoc* doc, JsonVal object, String name, JsonVal val);

/**
 * @brief Get the type of a JSON value
 * @param doc JSON document
 * @param val JSON value handle
 * @return Type of the JSON value
 */
JsonType json_type(const JsonDoc* doc, JsonVal val);

/**
 * @brief Get the parent type of a JSON value
 * @param doc JSON document
 * @param val JSON value handle
 * @return Parent type of the JSON value
 */
JsonParent json_parent(const JsonDoc* doc, JsonVal val);

/**
 * @brief Get an element from a JSON array by index
 * @param doc JSON document
 * @param array Array to get element from
 * @param idx Zero-based index of the element
 * @return Handle to the element, or sentinel value if index is out of bounds
 */
JsonVal json_elem(const JsonDoc* doc, JsonVal array, u32 idx);

/**
 * @brief Get the number of elements in a JSON array
 * @param doc JSON document
 * @param array Array to count elements in
 * @return Number of elements in the array
 */
u32 json_elem_count(const JsonDoc* doc, JsonVal array);

/**
 * @brief Get an iterator to the first element of a JSON array
 * @param doc JSON document
 * @param array Array to iterate over
 * @return Handle to the first element, or sentinel value if array is empty
 */
JsonVal json_elem_begin(const JsonDoc* doc, JsonVal array);

/**
 * @brief Get the next element in a JSON array iteration
 * @param doc JSON document
 * @param elem Current element handle
 * @return Handle to the next element, or sentinel value if at end
 */
JsonVal json_elem_next(const JsonDoc* doc, JsonVal elem);

/**
 * @brief Get a field value from a JSON object by name
 * @param doc JSON document
 * @param object Object to search in
 * @param name Field name to look for
 * @return Handle to the field value, or sentinel value if field not found
 */
JsonVal json_field(const JsonDoc* doc, JsonVal object, String name);

/**
 * @brief Get the number of fields in a JSON object
 * @param doc JSON document
 * @param object Object to count fields in
 * @return Number of fields in the object
 */
u32 json_field_count(const JsonDoc* doc, JsonVal object);

/**
 * @brief Get an iterator to the first field of a JSON object
 * @param doc JSON document
 * @param object Object to iterate over
 * @return Iterator to the first field, or sentinel iterator if object is empty
 */
JsonFieldItr json_field_begin(const JsonDoc* doc, JsonVal object);

/**
 * @brief Get the next field in a JSON object iteration
 * @param doc JSON document
 * @param fieldVal Current field value handle
 * @return Iterator to the next field, or sentinel iterator if at end
 */
JsonFieldItr json_field_next(const JsonDoc* doc, JsonVal fieldVal);

/**
 * @brief Get the string value from a JSON string
 * @param doc JSON document
 * @param val JSON string value handle
 * @return String content of the JSON string value
 */
String json_string(const JsonDoc* doc, JsonVal val);

/**
 * @brief Get the numeric value from a JSON number
 * @param doc JSON document
 * @param val JSON number value handle
 * @return Numeric value as IEEE 754 double precision floating point
 */
f64 json_number(const JsonDoc* doc, JsonVal val);

/**
 * @brief Get the boolean value from a JSON boolean
 * @param doc JSON document
 * @param val JSON boolean value handle
 * @return Boolean value (true or false)
 */
bool json_bool(const JsonDoc* doc, JsonVal val);