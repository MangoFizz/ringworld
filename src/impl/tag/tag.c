#include <string.h>

#include <ringworld/console/console.h>
#include <ringworld/tag/tag.h>

#include "../exception/exception.h"

extern bool *map_is_loaded;
extern TagDataHeader **tag_data_header_loaded;

TagHandle lookup_tag(const char *path, TagGroupFourCC group) {
    if(!*map_is_loaded) {
        return NULL_HANDLE;
    }

    TagDataHeader *header = *tag_data_header_loaded;
    uint32_t tag_count = header->tag_count;

    TagEntry *tag = header->tags;
    for(uint32_t i = 0; i < tag_count; i++, tag++) {
        if(tag->primary_group == group && strcmp(path, tag->path) == 0) {
            return tag->handle;
        }
    }

    console_printf_debug_err("lookup_tag failed!");
    console_printf_debug_err("    path: %s.%s", path, group_fourcc_to_name(group));
    return NULL_HANDLE;
}

void *get_tag_data(TagHandle tag_handle) {
    return (*tag_data_header_loaded)->tags[tag_handle.index].data;
}

void *tag_get_data(TagHandle tag_handle, TagGroupFourCC group) {
    ASSERT(tag_handle.index != NULL_HANDLE.index);
    TagEntry *tag = &(*tag_data_header_loaded)->tags[tag_handle.index];
    ASSERT(tag->primary_group == group);
    return tag->data;
}

void *tag_get_block(GenericTagBlock *block, uint32_t index, uint32_t size) {
    ASSERT(block);
    ASSERT(index < block->count);
    return block->elements + index * size;
}

const char *group_fourcc_to_name(TagGroupFourCC group_fourcc) {
    switch(group_fourcc) {
        case TAG_GROUP_ACTOR:
            return "actor";
        case TAG_GROUP_ACTOR_VARIANT:
            return "actor_variant";
        case TAG_GROUP_ANTENNA:
            return "antenna";
        case TAG_GROUP_MODEL_ANIMATIONS:
            return "model_animations";
        case TAG_GROUP_BIPED:
            return "biped";
        case TAG_GROUP_BITMAP:
            return "bitmap";
        case TAG_GROUP_SPHEROID:
            return "spheroid";
        case TAG_GROUP_CONTINUOUS_DAMAGE_EFFECT:
            return "continuous_damage_effect";
        case TAG_GROUP_MODEL_COLLISION_GEOMETRY:
            return "model_collision_geometry";
        case TAG_GROUP_COLOR_TABLE:
            return "color_table";
        case TAG_GROUP_CONTRAIL:
            return "contrail";
        case TAG_GROUP_DEVICE_CONTROL:
            return "device_control";
        case TAG_GROUP_DECAL:
            return "decal";
        case TAG_GROUP_UI_WIDGET_DEFINITION:
            return "ui_widget_definition";
        case TAG_GROUP_INPUT_DEVICE_DEFAULTS:
            return "input_device_defaults";
        case TAG_GROUP_DEVICE:
            return "device";
        case TAG_GROUP_DETAIL_OBJECT_COLLECTION:
            return "detail_object_collection";
        case TAG_GROUP_EFFECT:
            return "effect";
        case TAG_GROUP_EQUIPMENT:
            return "equipment";
        case TAG_GROUP_FLAG:
            return "flag";
        case TAG_GROUP_FOG:
            return "fog";
        case TAG_GROUP_FONT:
            return "font";
        case TAG_GROUP_MATERIAL_EFFECTS:
            return "material_effects";
        case TAG_GROUP_GARBAGE:
            return "garbage";
        case TAG_GROUP_GLOW:
            return "glow";
        case TAG_GROUP_GRENADE_HUD_INTERFACE:
            return "grenade_hud_interface";
        case TAG_GROUP_HUD_MESSAGE_TEXT:
            return "hud_message_text";
        case TAG_GROUP_HUD_NUMBER:
            return "hud_number";
        case TAG_GROUP_HUD_GLOBALS:
            return "hud_globals";
        case TAG_GROUP_ITEM:
            return "item";
        case TAG_GROUP_ITEM_COLLECTION:
            return "item_collection";
        case TAG_GROUP_DAMAGE_EFFECT:
            return "damage_effect";
        case TAG_GROUP_LENS_FLARE:
            return "lens_flare";
        case TAG_GROUP_LIGHTNING:
            return "lightning";
        case TAG_GROUP_DEVICE_LIGHT_FIXTURE:
            return "device_light_fixture";
        case TAG_GROUP_LIGHT:
            return "light";
        case TAG_GROUP_SOUND_LOOPING:
            return "sound_looping";
        case TAG_GROUP_DEVICE_MACHINE:
            return "device_machine";
        case TAG_GROUP_GLOBALS:
            return "globals";
        case TAG_GROUP_METER:
            return "meter";
        case TAG_GROUP_LIGHT_VOLUME:
            return "light_volume";
        case TAG_GROUP_GBXMODEL:
            return "gbxmodel";
        case TAG_GROUP_MODEL:
            return "model";
        case TAG_GROUP_MULTIPLAYER_SCENARIO_DESCRIPTION:
            return "multiplayer_scenario_description";
        case TAG_GROUP_PREFERENCES_NETWORK_GAME:
            return "preferences_network_game";
        case TAG_GROUP_OBJECT:
            return "object";
        case TAG_GROUP_PARTICLE:
            return "particle";
        case TAG_GROUP_PARTICLE_SYSTEM:
            return "particle_system";
        case TAG_GROUP_PHYSICS:
            return "physics";
        case TAG_GROUP_PLACEHOLDER:
            return "placeholder";
        case TAG_GROUP_POINT_PHYSICS:
            return "point_physics";
        case TAG_GROUP_PROJECTILE:
            return "projectile";
        case TAG_GROUP_WEATHER_PARTICLE_SYSTEM:
            return "weather_particle_system";
        case TAG_GROUP_SCENARIO_STRUCTURE_BSP:
            return "scenario_structure_bsp";
        case TAG_GROUP_SCENERY:
            return "scenery";
        case TAG_GROUP_SHADER_TRANSPARENT_CHICAGO_EXTENDED:
            return "shader_transparent_chicago_extended";
        case TAG_GROUP_SHADER_TRANSPARENT_CHICAGO:
            return "shader_transparent_chicago";
        case TAG_GROUP_SCENARIO:
            return "scenario";
        case TAG_GROUP_SHADER_ENVIRONMENT:
            return "shader_environment";
        case TAG_GROUP_SHADER_TRANSPARENT_GLASS:
            return "shader_transparent_glass";
        case TAG_GROUP_SHADER:
            return "shader";
        case TAG_GROUP_SKY:
            return "sky";
        case TAG_GROUP_SHADER_TRANSPARENT_METER:
            return "shader_transparent_meter";
        case TAG_GROUP_SOUND:
            return "sound";
        case TAG_GROUP_SOUND_ENVIRONMENT:
            return "sound_environment";
        case TAG_GROUP_SHADER_MODEL:
            return "shader_model";
        case TAG_GROUP_SHADER_TRANSPARENT_GENERIC:
            return "shader_transparent_generic";
        case TAG_GROUP_UI_WIDGET_COLLECTION:
            return "ui_widget_collection";
        case TAG_GROUP_SHADER_TRANSPARENT_PLASMA:
            return "shader_transparent_plasma";
        case TAG_GROUP_SOUND_SCENERY:
            return "sound_scenery";
        case TAG_GROUP_STRING_LIST:
            return "string_list";
        case TAG_GROUP_SHADER_TRANSPARENT_WATER:
            return "shader_transparent_water";
        case TAG_GROUP_TAG_COLLECTION:
            return "tag_collection";
        case TAG_GROUP_CAMERA_TRACK:
            return "camera_track";
        case TAG_GROUP_DIALOGUE:
            return "dialogue";
        case TAG_GROUP_UNIT_HUD_INTERFACE:
            return "unit_hud_interface";
        case TAG_GROUP_UNIT:
            return "unit";
        case TAG_GROUP_UNICODE_STRING_LIST:
            return "unicode_string_list";
        case TAG_GROUP_VIRTUAL_KEYBOARD:
            return "virtual_keyboard";
        case TAG_GROUP_VEHICLE:
            return "vehicle";
        case TAG_GROUP_WEAPON:
            return "weapon";
        case TAG_GROUP_WIND:
            return "wind";
        case TAG_GROUP_WEAPON_HUD_INTERFACE:
            return "weapon_hud_interface";
        default:
            return "???";
    }
}

