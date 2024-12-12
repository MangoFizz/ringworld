#include <ringworld/script/hsc.h>

uint16_t *structure_bsp_index = 0x006397d0;

void structure_bsp_index_evaluate(uint32_t param_1, uint32_t param_2) {
    hs_return(*structure_bsp_index, param_2);
}
