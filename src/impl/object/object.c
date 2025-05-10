#include <string.h>
#include <stdio.h>
#include "../exception/exception.h"
#include "../game/game_state.h"
#include "../memory/pool.h"
#include "../light/light.h"
#include "../widget/widget.h"
#include "object_type.h"
#include "object.h"

extern ObjectTable **object_table;
extern MemoryPool **object_memory_pool;
extern ObjectGlobals **object_globals;
extern void **object_name_list;
extern ObjectClusterPartition *collideable_object;
extern ObjectClusterPartition *noncollideable_object;
extern ObjectHandle *object_unk_handle_1;
extern uint32_t *object_unk1;
extern uint16_t *object_unk2;
extern uint32_t *object_unk3;

static const VectorIJK object_default_forward = {1.0f, 0.0f, 0.0f};
static const VectorIJK object_default_up = {0.0f, 0.0f, 1.0f};
static const ColorRGB object_default_change_color = {1.0f, 1.0f, 1.0f};

ObjectTable *object_table_get() {
    return *object_table;
}

ObjectGlobals *object_globals_get() {
    return *object_globals;
}

void object_new_cluster_partition_globals(ObjectClusterPartition *cluster_partition, const char *name) {
    cluster_partition->cluster_first_data_references = game_state_allocate_heap(OBJECT_MAXIMUM_COUNT);
    ASSERT(cluster_partition->cluster_first_data_references != NULL);
    
    char cluster_references_name[64];
    snprintf(cluster_references_name, sizeof(cluster_references_name), "cluster %s reference", name);
    cluster_partition->data_references_table = game_state_table_new(cluster_references_name, OBJECT_MAXIMUM_COUNT, 12);
    ASSERT(cluster_partition->data_references_table != NULL);
    
    char cluster_reference_name[64];
    snprintf(cluster_reference_name, sizeof(cluster_reference_name), "%s cluster reference", name);
    cluster_partition->cluster_references_table = game_state_table_new(cluster_reference_name, OBJECT_MAXIMUM_COUNT, 12);
    ASSERT(cluster_partition->cluster_references_table != NULL);
}

void objects_initialize(void) {
    widgets_initialize();
    object_types_initialize();
    lights_initialize();
    *object_table = game_state_table_new("object", OBJECT_MAXIMUM_COUNT, sizeof(DynamicObjectHeader));
    *object_memory_pool = game_state_memory_pool_new("objects", OBJECT_MEMORY_POOL_SIZE);
    if(*object_table != NULL && *object_memory_pool != NULL) {
        *object_globals = game_state_allocate_heap(0x98);
        *object_name_list = game_state_allocate_heap(OBJECT_MAXIMUM_COUNT);
    }
    object_new_cluster_partition_globals(collideable_object, "collideable object");
    object_new_cluster_partition_globals(noncollideable_object, "noncollideable object");
}

void objects_initialize_cluster_partition_for_new_map(ObjectClusterPartition *partition) {
    memset(partition->cluster_first_data_references, 0xFF, OBJECT_MAXIMUM_COUNT);
    table_make_valid(partition->data_references_table);
    table_make_valid(partition->cluster_references_table);
}

void object_globals_initialize_for_new_map(void) {
    ObjectGlobals *globals = object_globals_get();
    for(size_t i = 0; i < 16; i++) {
        globals->pvs[i] = 0;
        globals->pvs_2[i] = 0;
    }
    *object_unk3 = 0; 
    globals->pvs_activator_type = 0;
    globals->object_marker_initialized = false;
    globals->unk1 = 0;
    globals->last_garbage_collection_tick = 0;
    globals->unk_object_handle = NULL_HANDLE;
}

void objects_initialize_for_new_map(void) {
    *object_unk_handle_1 = NULL_HANDLE;
    *object_unk1 = 0;
    *object_unk2 = 0;
    
    object_types_initialize_for_new_map();
    widgets_initialize_for_new_map();
    lights_initialize_for_new_map();

    ObjectTable *object_table = object_table_get();
    table_make_valid(object_table);

    memset(*object_name_list, 0xFF, OBJECT_MAXIMUM_COUNT);
    objects_initialize_cluster_partition_for_new_map(collideable_object);
    objects_initialize_cluster_partition_for_new_map(noncollideable_object);

    object_globals_initialize_for_new_map();
}

DynamicObjectBase *object_get_and_verify_type(ObjectHandle object_handle, uint32_t types) {
    ObjectTable *object_table = object_table_get();
    DynamicObjectHeader *object_header = table_get_element(object_table, object_handle);
    if(object_header == NULL || !TEST_BIT(types, object_header->object_type)) {
        return NULL;
    }
    return object_header->address;
}

void object_placement_data_new(ObjectCreationData *data, TagHandle tag_handle, ObjectHandle parent_object_handle) {
    memset(data, 0, sizeof(ObjectCreationData));
    data->tag_handle = tag_handle;
    data->flags = 0;
    data->forward = object_default_forward;
    data->up = object_default_up;
    data->region_permutation = 0;
    DynamicObjectBase *parent = object_get_and_verify_type(parent_object_handle, 0xFFFFFFFF);
    if(parent != NULL) {
        data->parent_object_handle = parent_object_handle;
        data->player = parent->player;
        data->team_index = parent->team_owner;
    }
    else {
        data->parent_object_handle = NULL_HANDLE;
        data->player = NULL_HANDLE;
        data->team_index = -1;
    }
    for(int i = 0; i < OBJECT_NUMBER_OF_CHANGE_COLORS; i++) {
        data->change_colors[i] = object_default_change_color;
    }
}
