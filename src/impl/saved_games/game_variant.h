#ifndef RINGWORLD__SAVED_GAMES__GAME_VARIANT_H
#define RINGWORLD__SAVED_GAMES__GAME_VARIANT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * Get whether a game variant is not a stock variant.
 * @todo Verify if the name of this function is correct.
 */
bool game_variant_is_not_stock(const wchar_t *variant_name);

#ifdef __cplusplus
}
#endif

#endif
