#ifndef RINGWORLD__SCRIPT__HSC_H
#define RINGWORLD__SCRIPT__HSC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../memory/memory.h"

typedef enum PACKED_ENUM HscDataType {
    HSC_DATA_TYPE_PASSTHROUGH = 3,
    HSC_DATA_TYPE_VOID,
    HSC_DATA_TYPE_BOOLEAN,
    HSC_DATA_TYPE_REAL,
    HSC_DATA_TYPE_SHORT,
    HSC_DATA_TYPE_LONG,
    HSC_DATA_TYPE_STRING,
    HSC_DATA_TYPE_TRIGGER_VOLUME = 11,
    HSC_DATA_TYPE_CUTSCENE_FLAG,
    HSC_DATA_TYPE_CUTSCENE_CAMERA_POINT,
    HSC_DATA_TYPE_CUTSCENE_TITLE,
    HSC_DATA_TYPE_CUTSCENE_RECORDING,
    HSC_DATA_TYPE_DEVICE_GROUP,
    HSC_DATA_TYPE_AI,
    HSC_DATA_TYPE_AI_COMMAND_LIST,
    HSC_DATA_TYPE_STARTING_PROFILE,
    HSC_DATA_TYPE_CONVERSATION,
    HSC_DATA_TYPE_NAVPOINT,
    HSC_DATA_TYPE_HUD_MESSAGE,
    HSC_DATA_TYPE_OBJECT_LIST,
    HSC_DATA_TYPE_SOUND,
    HSC_DATA_TYPE_EFFECT,
    HSC_DATA_TYPE_DAMAGE,
    HSC_DATA_TYPE_LOOPING_SOUND,
    HSC_DATA_TYPE_ANIMATION_GRAPH,
    HSC_DATA_TYPE_ACTOR_VARIANT,
    HSC_DATA_TYPE_OBJECT_DEFINITION = 31,
    HSC_DATA_TYPE_GAME_DIFFICULTY,
    HSC_DATA_TYPE_TEAM,
    HSC_DATA_TYPE_AI_DEFAULT_STATE,
    HSC_DATA_TYPE_ACTOR_TYPE,
    HSC_DATA_TYPE_HUD_CORNER,
    HSC_DATA_TYPE_OBJECT,
    HSC_DATA_TYPE_UNIT,
    HSC_DATA_TYPE_VEHICLE,
    HSC_DATA_TYPE_DEVICE,
    HSC_DATA_TYPE_SCENERY,
    HSC_DATA_TYPE_OBJECT_NAME,
    HSC_DATA_TYPE_MAX,
    HSC_DATA_TYPE_SIZE = 0xFFFF
} HscDataType;

/**
 * Get the name of an HSC data type.
 * @param type the HSC data type
 * @return the name of the HSC data type
 */
const char *hsc_type_name(HscDataType type);

/**
 * Find an HSC function by name and type flags.
 * @param type_flags the type flags of the function to find
 * @param buffer the buffer to store the search results
 * @param buffer_size the size of the buffer
 * @param name the name of the function to find
 * @param flags additional flags; connection mode related?
 * @return the number of functions found
 * @todo Find out what the flags parameter actually does
 * @todo Reorder parameters to be more logical when reimplementing
 */
uint16_t hsc_find_functions(uint32_t type_flags, char **buffer, uint32_t buffer_size, const char *name, uint32_t flags);

/**
 * Execute a script.
 * @param script the script to execute
 * @return true if the script executed successfully, false otherwise
 */
bool hsc_execute_script(const char *script);

void hs_return(uint32_t return_value, uint32_t);

void structure_bsp_index_evaluate(uint32_t, uint32_t);

#ifdef __cplusplus
}
#endif

#endif
