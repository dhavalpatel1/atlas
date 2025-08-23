#include "core_alloc.h"

#include "anvil_spec.h"

spec(alloc_scratch) {
    it("respects the requested alignment") {
        alloc_alloc(g_alloc_scratch, 32, 32); 

        u8* startAddr = mem_begin(alloc_alloc(g_alloc_scratch, 1, 1));
        u8* addr      = mem_begin(alloc_alloc(g_alloc_scratch, 6, 2));

        anvil(addr == startAddr + 2);

        addr = mem_begin(alloc_alloc(g_alloc_scratch, 8, 8));

        anvil(addr == startAddr + 8);

        addr = mem_begin(alloc_alloc(g_alloc_scratch, 64, 32));

        anvil(addr == startAddr + 32);
    }
}