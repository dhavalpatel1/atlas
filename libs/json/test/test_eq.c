#include "json_eq.h"

#include "anvil_spec.h"

spec(eq) {

    JsonDoc* doc = null;

    setup() { doc = json_create(g_alloc_heap, 0); }

    it("returns false for values of different types") {
        const JsonVal x = json_add_bool(doc, true);
        const JsonVal y = json_add_number(doc, 1.0);

        anvil(!json_eq(doc, x, y));
    }

    it("returns true for empty arrays") {
        const JsonVal x = json_add_array(doc);
        const JsonVal y = json_add_array(doc);

        anvil(json_eq(doc, x, y));
    }

    it("returns true for arrays with equal elements") {
        const JsonVal x = json_add_array(doc);
        json_add_elem(doc, x, json_add_number(doc, 1.0));
        json_add_elem(doc, x, json_add_number(doc, 2.0));
        json_add_elem(doc, x, json_add_number(doc, 3.0));

        const JsonVal y = json_add_array(doc);
        json_add_elem(doc, y, json_add_number(doc, 1.0));
        json_add_elem(doc, y, json_add_number(doc, 2.0));
        json_add_elem(doc, y, json_add_number(doc, 3.0));

        anvil(json_eq(doc, x, y));
    }

    it("returns false for arrays with different element counts") {
        const JsonVal x = json_add_array(doc);
        json_add_elem(doc, x, json_add_number(doc, 1.0));
        json_add_elem(doc, x, json_add_number(doc, 2.0));
        json_add_elem(doc, x, json_add_number(doc, 3.0));

        const JsonVal y = json_add_array(doc);
        json_add_elem(doc, y, json_add_number(doc, 1.0));
        json_add_elem(doc, y, json_add_number(doc, 2.0));

        anvil(!json_eq(doc, x, y));
    }

    it("returns false for arrays with inequal elements") {
        const JsonVal x = json_add_array(doc);
        json_add_elem(doc, x, json_add_number(doc, 1.0));
        json_add_elem(doc, x, json_add_number(doc, 2.0));
        json_add_elem(doc, x, json_add_number(doc, 3.0));

        const JsonVal y = json_add_array(doc);
        json_add_elem(doc, y, json_add_number(doc, 1.0));
        json_add_elem(doc, y, json_add_number(doc, 2.1));
        json_add_elem(doc, y, json_add_number(doc, 3.0));

        anvil(!json_eq(doc, x, y));
    }

    it("returns true for empty objects") {
        const JsonVal x = json_add_object(doc);
        const JsonVal y = json_add_object(doc);

        anvil(json_eq(doc, x, y));
    }

    it("returns true for objects with equal fields") {
        const JsonVal x = json_add_object(doc);
        json_add_field_str(doc, x, string_lit("a"), json_add_number(doc, 1.0));
        json_add_field_str(doc, x, string_lit("b"), json_add_number(doc, 2.0));
        json_add_field_str(doc, x, string_lit("c"), json_add_number(doc, 3.0));

        const JsonVal y = json_add_object(doc);
        json_add_field_str(doc, y, string_lit("a"), json_add_number(doc, 1.0));
        json_add_field_str(doc, y, string_lit("b"), json_add_number(doc, 2.0));
        json_add_field_str(doc, y, string_lit("c"), json_add_number(doc, 3.0));

        anvil(json_eq(doc, x, y));
    }

    it("returns false for objects with different field counts") {
        const JsonVal x = json_add_object(doc);
        json_add_field_str(doc, x, string_lit("a"), json_add_number(doc, 1.0));
        json_add_field_str(doc, x, string_lit("b"), json_add_number(doc, 2.0));
        json_add_field_str(doc, x, string_lit("c"), json_add_number(doc, 3.0));

        const JsonVal y = json_add_object(doc);
        json_add_field_str(doc, y, string_lit("a"), json_add_number(doc, 1.0));
        json_add_field_str(doc, y, string_lit("b"), json_add_number(doc, 2.0));

        anvil(!json_eq(doc, x, y));
    }

    it("returns false for objects with inequal field values") {
        const JsonVal x = json_add_object(doc);
        json_add_field_str(doc, x, string_lit("a"), json_add_number(doc, 1.0));
        json_add_field_str(doc, x, string_lit("b"), json_add_number(doc, 2.0));
        json_add_field_str(doc, x, string_lit("c"), json_add_number(doc, 3.0));

        const JsonVal y = json_add_object(doc);
        json_add_field_str(doc, y, string_lit("a"), json_add_number(doc, 1.0));
        json_add_field_str(doc, y, string_lit("b"), json_add_number(doc, 2.1));
        json_add_field_str(doc, y, string_lit("c"), json_add_number(doc, 3.0));

        anvil(!json_eq(doc, x, y));
    }

    it("returns false for objects with inequal field names") {
        const JsonVal x = json_add_object(doc);
        json_add_field_str(doc, x, string_lit("a"), json_add_number(doc, 1.0));
        json_add_field_str(doc, x, string_lit("b"), json_add_number(doc, 2.0));
        json_add_field_str(doc, x, string_lit("c"), json_add_number(doc, 3.0));

        const JsonVal y = json_add_object(doc);
        json_add_field_str(doc, y, string_lit("a"), json_add_number(doc, 1.0));
        json_add_field_str(doc, y, string_lit("b"), json_add_number(doc, 2.0));
        json_add_field_str(doc, y, string_lit("d"), json_add_number(doc, 3.0));

        anvil(!json_eq(doc, x, y));
    }

    it("returns true for equal strings") {
        const JsonVal x = json_add_string_lit(doc, "Hello World");
        const JsonVal y = json_add_string_lit(doc, "Hello World");

        anvil(json_eq(doc, x, y));
    }

    it("returns false for inequal strings") {
        const JsonVal x = json_add_string_lit(doc, "Hello");
        const JsonVal y = json_add_string_lit(doc, "World");

        anvil(!json_eq(doc, x, y));
    }

    it("returns true for equal numbers") {
        const JsonVal x = json_add_number(doc, 42.1337);
        const JsonVal y = json_add_number(doc, 42.1337);

        anvil(json_eq(doc, x, y));
    }

    it("returns false for inequal numbers") {
        const JsonVal x = json_add_number(doc, 42.1337);
        const JsonVal y = json_add_number(doc, 42.1336);

        anvil(!json_eq(doc, x, y));
    }

    it("returns true for equal booleans") {
        const JsonVal x = json_add_bool(doc, true);
        const JsonVal y = json_add_bool(doc, true);

        anvil(json_eq(doc, x, y));
    }

    it("returns false for inequal booleans") {
        const JsonVal x = json_add_bool(doc, true);
        const JsonVal y = json_add_bool(doc, false);

        anvil(!json_eq(doc, x, y));
    }

    it("returns true for null values") {
        const JsonVal x = json_add_null(doc);
        const JsonVal y = json_add_null(doc);

        anvil(json_eq(doc, x, y));
    }

    teardown() { json_destroy(doc); }
}
