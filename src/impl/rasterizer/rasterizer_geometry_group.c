#include "../exception/exception.h"
#include "rasterizer_geometry_group.h"
#include "rasterizer_dx9_vertex.h"

VertexBufferType rasterizer_geometry_group_get_vertex_buffer_type(TransparentGeometryGroup *group) {
    ASSERT(group != NULL);
    if(group->vertex_buffers == NULL) {
        if(group->dynamic_vertex_buffer_index == -1) {
            crashf("Transparent geometry group has no vertex buffers or dynamic vertex buffer");
        }
        return rasterizer_dx9_vertex_get_dynamic_buffer_type(group->dynamic_vertex_buffer_index);
    }
    return group->vertex_buffers->type;
}
