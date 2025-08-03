// SPDX-License-Identifier: GPL-3.0-only

#ifndef RINGWORLD__TAG__TAG_H
#define RINGWORLD__TAG__TAG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types/types.h"

typedef enum TagGroup {
    TAG_GROUP_NONE = 0,
    TAG_GROUP_ACTOR = 0x61637472,
    TAG_GROUP_ACTOR_VARIANT = 0x61637476,
    TAG_GROUP_ANTENNA = 0x616E7421,
    TAG_GROUP_MODEL_ANIMATIONS = 0x616E7472,
    TAG_GROUP_BIPED = 0x62697064,
    TAG_GROUP_BITMAP = 0x6269746D,
    TAG_GROUP_SPHEROID = 0x626F6F6D,
    TAG_GROUP_CONTINUOUS_DAMAGE_EFFECT = 0x63646D67,
    TAG_GROUP_MODEL_COLLISION_GEOMETRY = 0x636F6C6C,
    TAG_GROUP_COLOR_TABLE = 0x636F6C6F,
    TAG_GROUP_CONTRAIL = 0x636F6E74,
    TAG_GROUP_DEVICE_CONTROL = 0x6374726C,
    TAG_GROUP_DECAL = 0x64656361,
    TAG_GROUP_UI_WIDGET_DEFINITION = 0x44654C61,
    TAG_GROUP_INPUT_DEVICE_DEFAULTS = 0x64657663,
    TAG_GROUP_DEVICE = 0x64657669,
    TAG_GROUP_DETAIL_OBJECT_COLLECTION = 0x646F6263,
    TAG_GROUP_EFFECT = 0x65666665,
    TAG_GROUP_EQUIPMENT = 0x65716970,
    TAG_GROUP_FLAG = 0x666C6167,
    TAG_GROUP_FOG = 0x666F6720,
    TAG_GROUP_FONT = 0x666F6E74,
    TAG_GROUP_MATERIAL_EFFECTS = 0x666F6F74,
    TAG_GROUP_GARBAGE = 0x67617262,
    TAG_GROUP_GLOW = 0x676C7721,
    TAG_GROUP_GRENADE_HUD_INTERFACE = 0x67726869,
    TAG_GROUP_HUD_MESSAGE_TEXT = 0x686D7420,
    TAG_GROUP_HUD_NUMBER = 0x68756423,
    TAG_GROUP_HUD_GLOBALS = 0x68756467,
    TAG_GROUP_ITEM = 0x6974656D,
    TAG_GROUP_ITEM_COLLECTION = 0x69746D63,
    TAG_GROUP_DAMAGE_EFFECT = 0x6A707421,
    TAG_GROUP_LENS_FLARE = 0x6C656E73,
    TAG_GROUP_LIGHTNING = 0x656C6563,
    TAG_GROUP_DEVICE_LIGHT_FIXTURE = 0x6C696669,
    TAG_GROUP_LIGHT = 0x6C696768,
    TAG_GROUP_SOUND_LOOPING = 0x6C736E64,
    TAG_GROUP_DEVICE_MACHINE = 0x6D616368,
    TAG_GROUP_GLOBALS = 0x6D617467,
    TAG_GROUP_METER = 0x6D657472,
    TAG_GROUP_LIGHT_VOLUME = 0x6D677332,
    TAG_GROUP_GBXMODEL = 0x6D6F6432,
    TAG_GROUP_MODEL = 0x6D6F6465,
    TAG_GROUP_MULTIPLAYER_SCENARIO_DESCRIPTION = 0x6D706C79,
    TAG_GROUP_NULL = 0xFFFFFFFF,
    TAG_GROUP_PREFERENCES_NETWORK_GAME = 0x6E677072,
    TAG_GROUP_OBJECT = 0x6F626A65,
    TAG_GROUP_PARTICLE = 0x70617274,
    TAG_GROUP_PARTICLE_SYSTEM = 0x7063746C,
    TAG_GROUP_PHYSICS = 0x70687973,
    TAG_GROUP_PLACEHOLDER = 0x706C6163,
    TAG_GROUP_POINT_PHYSICS = 0x70706879,
    TAG_GROUP_PROJECTILE = 0x70726F6A,
    TAG_GROUP_WEATHER_PARTICLE_SYSTEM = 0x7261696E,
    TAG_GROUP_SCENARIO_STRUCTURE_BSP = 0x73627370,
    TAG_GROUP_SCENERY = 0x7363656E,
    TAG_GROUP_SHADER_TRANSPARENT_CHICAGO_EXTENDED = 0x73636578,
    TAG_GROUP_SHADER_TRANSPARENT_CHICAGO = 0x73636869,
    TAG_GROUP_SCENARIO = 0x73636E72,
    TAG_GROUP_SHADER_ENVIRONMENT = 0x73656E76,
    TAG_GROUP_SHADER_TRANSPARENT_GLASS = 0x73676C61,
    TAG_GROUP_SHADER = 0x73686472,
    TAG_GROUP_SKY = 0x736B7920,
    TAG_GROUP_SHADER_TRANSPARENT_METER = 0x736D6574,
    TAG_GROUP_SOUND = 0x736E6421,
    TAG_GROUP_SOUND_ENVIRONMENT = 0x736E6465,
    TAG_GROUP_SHADER_MODEL = 0x736F736F,
    TAG_GROUP_SHADER_TRANSPARENT_GENERIC = 0x736F7472,
    TAG_GROUP_UI_WIDGET_COLLECTION = 0x536F756C,
    TAG_GROUP_SHADER_TRANSPARENT_PLASMA = 0x73706C61,
    TAG_GROUP_SOUND_SCENERY = 0x73736365,
    TAG_GROUP_STRING_LIST = 0x73747223,
    TAG_GROUP_SHADER_TRANSPARENT_WATER = 0x73776174,
    TAG_GROUP_TAG_COLLECTION = 0x74616763,
    TAG_GROUP_CAMERA_TRACK = 0x7472616B,
    TAG_GROUP_DIALOGUE = 0x75646C67,
    TAG_GROUP_UNIT_HUD_INTERFACE = 0x756E6869,
    TAG_GROUP_UNIT = 0x756E6974,
    TAG_GROUP_UNICODE_STRING_LIST = 0x75737472,
    TAG_GROUP_VIRTUAL_KEYBOARD = 0x76636B79,
    TAG_GROUP_VEHICLE = 0x76656869,
    TAG_GROUP_WEAPON = 0x77656170,
    TAG_GROUP_WIND = 0x77696E64,
    TAG_GROUP_WEAPON_HUD_INTERFACE = 0x77706869,
#ifdef RINGWORLD_ENABLE_ENHANCEMENTS
    TAG_GROUP_VECTOR_FONT = 0x76666E74,
    TAG_GROUP_VECTOR_FONT_DATA = 0x76666E64,
#endif
} TagGroup;

typedef union TableResourceHandle TagHandle;

typedef struct TagEntry {
    TagGroup primary_group;
    TagGroup secondary_group;
    TagGroup tertiary_group;
    TagHandle handle;
    char *path;
    void *data;
    uint32_t indexed;
    char padding[4];
} TagEntry;
_Static_assert(sizeof(TagEntry) == 0x20);

typedef struct TagDataHeader {
    TagEntry *tags;
    TagHandle scenario_tag_id;
    uint32_t tag_checksum;
    uint32_t tag_count;
    uint32_t model_part_count;
    uint32_t model_data_file_offset;
    uint32_t model_part_count2;
    uint32_t vertex_size;
    uint32_t model_data_size;
    FourCC tags_fourcc;
} TagDataHeader;
_Static_assert(sizeof(TagDataHeader) == 0x28);

typedef struct TagReference {
    TagGroup tag_group;
    char *path; // unused at runtime
    size_t path_size; // unused at runtime
    TagHandle tag_handle;
} TagReference;
_Static_assert(sizeof(TagReference) == 0x10);

typedef struct TagRawData {
    uint32_t size;
    uint32_t flags;
    uint32_t file_offset; // only applies to sounds
    void *pointer;
    char padding[4];
} TagRawData;
_Static_assert(sizeof(TagRawData) == 0x14);

typedef struct TagBlockDefinition {
    const char *name;
    int maximum;
    char padding[4];
    int elements_size;
    void *fields;
} TagBlockDefinition;
_Static_assert(sizeof(TagBlockDefinition) == 0x14);

/**
 * Get the tag data header.
 * @return pointer to tag data header
 */
TagDataHeader *tag_get_data_header(void);

/**
 * Get the tag entry for a tag handle.
 * @param tag_handle tag handle
 * @return pointer to tag entry
 */
TagEntry *tag_get_entry(TagHandle tag_handle);

/**
 * Lookup the tag.
 * @param path  path of the tag (not including file extension)
 * @param group group of the tag
 * @return tag ID if found, NULL_ID if not or if the map is not loaded
 */
TagHandle tag_lookup(const char *path, TagGroup group);

/**
 * Get the data for the tag.
 * @param tag_group group of the tag
 * @param tag_handle tag handle
 * @return pointer to tag data
 */
void *tag_get_data(TagGroup tag_group, TagHandle tag_handle);

/**
 * Get th path for the tag.
 */
char *tag_get_path(TagHandle tag_handle);

/**
 * Get the tag primary group for a tag handle.
 * @param tag_handle tag handle
 * @return tag group
 */
TagGroup tag_get_primary_group(TagHandle tag_handle);

/**
 * Get an element from a reflexive block.
 * @param block reflexive block
 * @param index element index
 * @param size size of each element
 * @return pointer to element
 */
void *tag_get_block(GenericTagBlock *block, uint32_t index, uint32_t size);

/**
 * Get an element from a reflexive block.
 * @param block reflexive block
 * @param index element index
 * @return pointer to element
 */
#define TAG_BLOCK_GET_ELEMENT(block, index) (tag_get_block((GenericTagBlock *)&block, index, sizeof(block.elements[0])))

/**
 * Convert a tag group FourCC to a name.
 * @param group_fourcc fourcc to look up
 * @return tag group or "???" if unknown
 */
const char *group_fourcc_to_name(TagGroup group_fourcc);

#ifdef __cplusplus
}
#endif

#endif
