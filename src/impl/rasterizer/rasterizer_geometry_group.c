#include "../exception/exception.h"
#include "rasterizer_geometry_group.h"
#include "rasterizer_dx9.h"


extern uint16_t *dynamic_vertices;

uint16_t rasterizer_get_dynamic_vertex_buffer_type(uint16_t dynamic_vertex_index) {
    IDirect3DDevice9 *device = rasterizer_dx9_device();
    ASSERT(device != NULL);
    ASSERT(dynamic_vertex_index != -1);
    return dynamic_vertices[dynamic_vertex_index * 8];
}

uint16_t rasterizer_get_vertex_buffer_type(TransparentGeometryGroup *group) {
    ASSERT(group != NULL);
    if(group->vertex_buffers == NULL) {
        ASSERT(group->dynamic_vertex_buffer_index == -1);
        return rasterizer_get_dynamic_vertex_buffer_type(group->dynamic_vertex_buffer_index);
    }
    return group->vertex_buffers->type;
}
