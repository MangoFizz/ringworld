#include "object_type.h"

extern ObjectTypeInterface **object_type_interface_list;

ObjectTypeInterface *object_type_interface_get(ObjectType type) {
    if (type >= OBJECT_TYPE_MAX_VALUE) {
        return NULL;
    }
    return object_type_interface_list[type];
}

void object_types_initialize_for_new_map(void) {
    if(object_type_interface_list == NULL) {
        return;
    }
    ObjectTypeInterface *curent_interface = *object_type_interface_list;
    while(curent_interface != NULL) {
        if(curent_interface->initialize_for_new_map) {
            curent_interface->initialize_for_new_map();
        }
        curent_interface = curent_interface->next;
    }
}
