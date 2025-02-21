#ifndef RINGWORLD__CUTSCENE__CUTSCENE_H
#define RINGWORLD__CUTSCENE__CUTSCENE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct CinematicGlobals {
    float letter_box_size;
    uint32_t letter_box_last_game_time;
    bool showing_letter_box;
    bool cinematic_in_progress;
    bool cinematic_skip_in_progress;
    bool cinematic_suppress_bsp_object_creation;
    struct {
        int16_t index;
        int16_t timer;
    } titles[4];
} CinematicGlobals;

/**
 * Get the global cinematic settings.
 * @return A pointer to the global cinematic settings.
 */
CinematicGlobals *cutscene_get_globals(void);

#ifdef __cplusplus
}
#endif

#endif
