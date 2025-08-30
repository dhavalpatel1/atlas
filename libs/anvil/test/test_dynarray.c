#include "core_array.h"

#include "anvil_spec.h"

spec(dynarray) {
    DynArray array;

    setup() {
        array = dynarray_create_t(g_alloc_heap, u64, 0);
    }

    it("Is empty when created") {
        anvil_eq_u64(array.size, 0);
    }

    it("Increases in size when new elements are pushed") {
        *dynarray_push_t(&array, u64) = 42;
        anvil_eq_u64(array.size, 1);
    }

    it("Can be sorted") {
        const u64 data[] = { 6, 3, 1, 42, 7, 8 };
        const u64 expected[] = { 1, 3, 6, 7, 8, 42 };

        array_for_t(data, u64, itr, {
            *dynarray_push_t(&array, u64) = *itr;
        });

        dynarray_sort(&array, compare_u64);

        dynarray_for_t(&array, u64, itr, {
            anvil_eq_u64(*itr, expected[itr_i]);
        });
    }

    teardown() {
        dynarray_destroy(&array);
    }
}
