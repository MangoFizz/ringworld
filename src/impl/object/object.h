#ifndef RINGWORLD__OBJECT__OBJECT_H
#define RINGWORLD__OBJECT__OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../memory/table.h"
#include "../types/types.h"
#include "object_base.h"

enum {
    OBJECT_MAXIMUM_COUNT = 0x1000,
    OBJECT_MEMORY_POOL_SIZE = 0xA00000, // 10 MiB
    OBJECT_NUMBER_OF_CHANGE_COLORS = 4
};

typedef struct ObjectGlobals {
    bool object_is_being_placed;
    bool object_marker_initialized;
    bool garbage_collect_now;
    int16_t unk1; 
    ObjectHandle unk_object_handle;
    uint32_t pvs[16];
    uint32_t pvs_2[16];
    int32_t last_garbage_collection_tick; 
    int16_t pvs_activator_type; 
    ObjectHandle unk_object_handle_2;
} ObjectGlobals; 

typedef struct ObjectPlacementData {
    TagHandle tag_handle;
    uint32_t flags;
    PlayerHandle player;
    ObjectHandle parent_object_handle;
    TagHandle parent_tag_handle; 
    int16_t team_index; 
    int16_t region_permutation;
    VectorXYZ position; 
    float unk1; 
    VectorIJK translational_velocity; 
    VectorIJK forward; 
    VectorIJK up; 
    VectorIJK angular_velocity; 
    ColorRGB change_colors[OBJECT_NUMBER_OF_CHANGE_COLORS]; 
} ObjectCreationData;
_Static_assert(sizeof(ObjectCreationData) == 0x88);

typedef struct DynamicObjectHeader {
    int16_t id;
    uint8_t flags;
    uint8_t object_type;
    int16_t cluster_index;
    int16_t data_size;
    void *address;
} DynamicObjectHeader;
_Static_assert(sizeof(DynamicObjectHeader) == 0xC);

MAKE_TABLE_STRUCT(ObjectTable, DynamicObjectHeader);

typedef struct ObjectClusterPartition {
    int8_t *cluster_first_data_references;
    GenericTable *data_references_table;
    GenericTable *cluster_references_table;
} ObjectClusterPartition;


/**
 * Retrieves the global object table instance.
 * 
 * @return Pointer to the global objects table.
 */
ObjectTable *object_table_get();

/**
 * Retrieves and verifies the type of a dynamic object.
 *
 * @param object_handle The handle to the object to be retrieved and verified.
 * @param types A bitmask or set of expected types to verify against.
 * @return A pointer to the dynamic object if the type matches; otherwise,
 *         returns NULL.
 */
DynamicObjectBase *object_get_and_verify_type(ObjectHandle object_handle, uint32_t types);

/**
 * Initialize object placement data.
 * @param data The object placement data to initialize.
 * @param tag_handle The tag handle of the object.
 * @param parent_handle The parent object handle.
 */ 
void object_placement_data_new(ObjectCreationData *data, TagHandle tag_handle, ObjectHandle parent_handle);

/**
 * Attach an object to another object by its markers.
 * @param object The object to attach.
 * @param object_marker The marker on the object to attach.
 * @param attachment The object to attach to.
 * @param attachment_marker The marker on the attachment to attach to.
 */
void object_attach_to_marker(ObjectHandle object, const char *object_marker, ObjectHandle attachment, const char *attachment_marker);

#ifdef __cplusplus
}
#endif

#endif
