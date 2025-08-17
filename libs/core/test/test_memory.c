#include "core_array.h"
#include "core_memory.h"

#include "anvil_spec.h"

spec(memory) {

    it("can check if it contains a specific byte") {
        Mem mem = array_mem(((u8[]){
            42,
            137,
            255,
            99,
        }));

        anvil(mem_contains(mem, 42));
        anvil(mem_contains(mem, 99));
        anvil(mem_contains(mem, 255));

        anvil(!mem_contains(mem, 7));
        anvil(!mem_contains(mem, 0));
    }
}