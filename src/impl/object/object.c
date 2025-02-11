#include <string.h>
#include <stdio.h>
#include "object.h"

extern ObjectTable **object_table;

static const VectorIJK object_default_forward = {1.0f, 0.0f, 0.0f};
static const VectorIJK object_default_up = {0.0f, 0.0f, 1.0f};
static const ColorRGB object_default_change_color = {1.0f, 1.0f, 1.0f};

ObjectTable *object_table_get() {
    return *object_table;
}

DynamicObjectBase *object_get_and_verify_type(ObjectHandle object_handle, uint32_t types) {
    ObjectTable *object_table = object_table_get();
    DynamicObjectHeader *object_header = table_get_element((GenericTable *)object_table, object_handle);
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
    for(int i = 0; i < NUMBER_OF_OBJECT_CHANGE_COLORS; i++) {
        data->change_colors[i] = object_default_change_color;
    }
}

