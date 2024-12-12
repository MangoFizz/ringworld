#include <ringworld/script/hsc.h>

extern uint16_t *structure_bsp_index;

void structure_bsp_index_evaluate(uint32_t param_1, uint32_t param_2) {
    hs_return(*structure_bsp_index, param_2);
}
