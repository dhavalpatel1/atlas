/**
 * @file json_eq.h
 * @brief JSON value equality comparison functionality
 *
 * This header provides utilities for deep equality comparison between JSON values.
 * The comparison follows JSON semantics for determining when two values are
 * considered equal, including deep comparison of nested structures.
 */

#pragma once

#include "json_doc.h"

/**
 * @brief Compare two JSON values for deep equality
 * @param doc JSON document containing both values
 * @param x First JSON value to compare
 * @param y Second JSON value to compare
 * @return true if the values are equal, false otherwise
 *
 * Performs deep equality comparison following JSON semantics:
 * - Numbers: Equal if they represent the same numeric value
 * - Strings: Equal if they contain the same sequence of characters
 * - Booleans: Equal if they have the same truth value
 * - Null: All null values are considered equal
 * - Arrays: Equal if they have the same length and all elements are equal
 * - Objects: Equal if they have the same set of fields with equal values
 * - Different types: Never equal (e.g., number 0 is not equal to boolean false)
 */
bool json_eq(JsonDoc* doc, JsonVal x, JsonVal y);