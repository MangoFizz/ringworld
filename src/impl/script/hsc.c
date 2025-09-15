#include "hsc.h"

extern uint16_t *structure_bsp_index;

void structure_bsp_index_evaluate(uint32_t param_1, uint32_t param_2) {
    hs_return(*structure_bsp_index, param_2);
}

const char *hsc_type_name(HscDataType type) {
    switch(type) {
        case HSC_DATA_TYPE_PASSTHROUGH:
            return "passthrough";
        case HSC_DATA_TYPE_VOID:
            return "void";
        case HSC_DATA_TYPE_BOOLEAN:
            return "boolean";
        case HSC_DATA_TYPE_REAL:
            return "real";
        case HSC_DATA_TYPE_SHORT:
            return "short";
        case HSC_DATA_TYPE_LONG:
            return "long";
        case HSC_DATA_TYPE_STRING:
            return "string";
        case HSC_DATA_TYPE_TRIGGER_VOLUME:
            return "trigger_volume";
        case HSC_DATA_TYPE_CUTSCENE_FLAG:
            return "cutscene_flag";
        case HSC_DATA_TYPE_CUTSCENE_CAMERA_POINT:
            return "cutscene_camera_point";
        case HSC_DATA_TYPE_CUTSCENE_TITLE:
            return "cutscene_title";
        case HSC_DATA_TYPE_CUTSCENE_RECORDING:
            return "cutscene_recording";
        case HSC_DATA_TYPE_DEVICE_GROUP:
            return "device_group";
        case HSC_DATA_TYPE_AI:
            return "ai";
        case HSC_DATA_TYPE_AI_COMMAND_LIST:
            return "ai_command_list";
        case HSC_DATA_TYPE_STARTING_PROFILE:
            return "starting_profile";
        case HSC_DATA_TYPE_CONVERSATION:
            return "conversation";
        case HSC_DATA_TYPE_NAVPOINT:
            return "navpoint";
        case HSC_DATA_TYPE_HUD_MESSAGE:
            return "hud_message";
        case HSC_DATA_TYPE_OBJECT_LIST:
            return "object_list";
        case HSC_DATA_TYPE_SOUND:
            return "sound";
        case HSC_DATA_TYPE_EFFECT:
            return "effect";
        case HSC_DATA_TYPE_DAMAGE:
            return "damage";
        case HSC_DATA_TYPE_LOOPING_SOUND:
            return "looping_sound";
        case HSC_DATA_TYPE_ANIMATION_GRAPH:
            return "animation_graph";
        case HSC_DATA_TYPE_ACTOR_VARIANT:
            return "actor_variant";
        case HSC_DATA_TYPE_OBJECT_DEFINITION:
            return "object_definition";
        case HSC_DATA_TYPE_GAME_DIFFICULTY:
            return "game_difficulty";
        case HSC_DATA_TYPE_TEAM:
            return "team";
        case HSC_DATA_TYPE_AI_DEFAULT_STATE:
            return "ai_default_state";
        case HSC_DATA_TYPE_ACTOR_TYPE:
            return "actor_type";
        case HSC_DATA_TYPE_HUD_CORNER:
            return "hud_corner";
        case HSC_DATA_TYPE_OBJECT:
            return "object";
        case HSC_DATA_TYPE_UNIT:
            return "unit";
        case HSC_DATA_TYPE_VEHICLE:
            return "vehicle";
        case HSC_DATA_TYPE_DEVICE:
            return "device";
        case HSC_DATA_TYPE_SCENERY:
            return "scenery";
        case HSC_DATA_TYPE_OBJECT_NAME:
            return "object_name";
        default:
            return "unknown";
    }
}
