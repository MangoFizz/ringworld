#ifndef RINGWORLD__OBJECT__OBJECT_TYPE_H
#define RINGWORLD__OBJECT__OBJECT_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "object_base.h"

typedef struct ObjectTypeInterface {
    const char *name;
    TagGroup tag_group;
    int16_t data_size;
    int16_t placement_tag_block_offset;
    int16_t palette_tag_block_offset;
    int16_t placement_tag_block_size;
    int32_t update_message_type;
    void (*initialize)();
    void (*dispose)();
    void (*initialize_for_new_map)();
    void (*dispose_from_old_map)();
    void (*adjust_placement)();
    void (*new_object)();
    void (*place_object)();
    void (*delete_object)();
    void (*update_object)();
    void (*export_function_values)();
    void (*handle_deleted_object)();
    void (*handle_region_destroyed)();
    void (*handle_parent_destroyed)();
    void (*preprocess_node_orientations)();
    void (*postprocess_node_matrices)();
    void (*reset)();
    void (*disconnect_from_structure_bsp)();
    void (*notify_impulse_sound)();
    void (*render_debug)();
    void (*render_message_debug)();
    void (*create_to_network)();
    void (*update_baseline)();
    void (*build_update_delta)();
    void (*process_update_delta)();
    void (*is_network_time_valid)();
    void (*unknown78)();
    void (*update_network_time)();
    struct ObjectTypeInterface *inheritance_list[16];
    struct ObjectTypeInterface *next;
} ObjectTypeInterface;

/**
 * Get the object type interface for a given object type.
 * @param type The object type.
 * @return A pointer to the object type interface.
 */
ObjectTypeInterface *object_type_interface_get(ObjectType type);

/**
 * Initialize the object type interfaces.
 */
void object_types_initialize(void);

/**
 * Initialize the object type interfaces for a new map.
 */
void object_types_initialize_for_new_map(void);

#ifdef __cplusplus
}
#endif

#endif
