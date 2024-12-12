#ifndef RINGWORLD__SCRIPT__HSC_H
#define RINGWORLD__SCRIPT__HSC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void hs_return(uint32_t return_value, uint32_t);

void structure_bsp_index_evaluate(uint32_t, uint32_t);

#ifdef __cplusplus
}
#endif

#endif
