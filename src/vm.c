#include "vm.h"
#include "bh_common.h"
#include "wasm_export.h"

bool vm_init(void) {
    return wasm_runtime_init();
}

void vm_destroy(void) {
    wasm_runtime_destroy();
}
