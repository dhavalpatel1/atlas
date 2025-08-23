#include "core_alloc.h"

#include "anvil_spec.h"

spec(alloc_bump) {

    it("tracks the maximum remaining size") {
        Allocator* alloc = alloc_bump_create_stack(128);

        const usize startingSize = alloc_max_size(alloc);

        alloc_alloc(alloc, 32, sizeof(void*));

        anvil_eq_int(alloc_max_size(alloc), startingSize - 32);

        alloc_alloc(alloc, alloc_max_size(alloc), sizeof(void*));
        anvil_eq_int(alloc_max_size(alloc), 0);
    }

    it("respects the requested alignment") {
        Allocator* alloc = alloc_bump_create_stack(256);
        alloc_alloc(alloc, 32, 32);

        const usize startingSize = alloc_max_size(alloc);

        alloc_alloc(alloc, 6, 1);

        anvil_eq_int(alloc_max_size(alloc), startingSize - 6);

        alloc_alloc(alloc, 8, 8);

        anvil_eq_int(alloc_max_size(alloc), startingSize - 16);

        alloc_alloc(alloc, 64, 32);

        anvil_eq_int(alloc_max_size(alloc), startingSize - 96);
    }
}