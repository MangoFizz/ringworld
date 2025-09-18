#include <string.h>
#include "event/event.h"
#include "../terminal/terminal.h"
#include "hsc.h"

extern uint16_t *structure_bsp_index;
extern HscFunction **hsc_functions;

enum {
    HSC_FUNCTIONS_COUNT = 529
};

HscFunction *hsc_get_function(int16_t index) {
    if(index < 0 || index >= HSC_FUNCTIONS_COUNT) {
        return NULL;
    }
    return hsc_functions[index];
}

int16_t hsc_get_function_by_name(const char *name) {
    // aliases
    if(stricmp(name, "clear") == 0) {
        name = "cls";
    }
    for(int16_t i = 0; i < HSC_FUNCTIONS_COUNT; i++) {
        HscFunction *func = hsc_get_function(i);
        if(func && func->name && stricmp(func->name, name) == 0) {
            return i;
        }
    }
    return -1;
}


void hsc_help_command(char *command) {
    int16_t function_index = hsc_get_function_by_name(command);
    if(function_index == -1) {
        char command_input[256];
        strncpy(command_input, command, sizeof(command_input));
        bool handled = RINGWORLD_EVENT_DISPATCH(RW_EVENT_HSC_FUNCTION_HELP_NOT_FOUND, command_input);
        if(!handled) {
            terminal_info_printf("Function \"%s\" cannot be found.", command);
        }
        return;
    }

    char buffer[2048];

    memset(buffer, 0, sizeof(buffer));
    hsc_get_function_parameters_string(buffer, function_index);
    terminal_info_printf("%s", buffer);

    HscFunction *function = hsc_get_function(function_index);
    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, function->help, sizeof(buffer) - 1);

    /* Print first line up to the first newline; if no newline, print whole help string. */
    char *start = buffer;
    char *nl = strchr(start, '\n');
    if(nl == NULL) {
        terminal_info_printf("%s", start);
        return;
    }

    while(start != NULL && *start != '\0') {
        if(nl != NULL) {
            /* temporarily terminate this line, print it, then restore */
            *nl = '\0';
            terminal_info_printf("%s", start);
            *nl = '\n';
            start = nl + 1;
            nl = strchr(start, '\n');
        } 
        else {
            /* last line (no trailing newline) */
            if(*start != '\0') {
                terminal_info_printf("%s", start);
            }
            break;
        }
    }
}

void structure_bsp_index_evaluate(uint32_t param_1, uint32_t param_2) {
    hs_return(*structure_bsp_index, param_2);
}

const char *hsc_type_name(HscDataType type) {
    switch(type) {
        case HSC_DATA_TYPE_UNPARSED:
            return "unparsed";
        case HSC_DATA_TYPE_SPECIAL_FORM:
            return "special form";
        case HSC_DATA_TYPE_FUNCTION_NAME:
            return "function name";
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
        case HSC_DATA_TYPE_SCRIPT:
            return "script";
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
        case HSC_DATA_TYPE_DAMAGE_EFFECT:
            return "damage_effect";
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
        case HSC_DATA_TYPE_WEAPON:
            return "weapon";
        case HSC_DATA_TYPE_DEVICE:
            return "device";
        case HSC_DATA_TYPE_SCENERY:
            return "scenery";
        case HSC_DATA_TYPE_OBJECT_NAME:
            return "object_name";
        case HSC_DATA_TYPE_UNIT_NAME:
            return "unit_name";
        case HSC_DATA_TYPE_VEHICLE_NAME:
            return "vehicle_name";
        case HSC_DATA_TYPE_WEAPON_NAME:
            return "weapon_name";
        case HSC_DATA_TYPE_DEVICE_NAME:
            return "device_name";
        case HSC_DATA_TYPE_SCENERY_NAME:
            return "scenery_name";
        default:
            return "unknown";
    }
}
