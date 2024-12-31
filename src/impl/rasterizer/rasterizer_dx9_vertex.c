#include <stdio.h>

#include "../exception/exception.h"
#include "rasterizer_dx9_vertex.h"

extern DynamicVertices *dynamic_vertices;
extern short *vertex_shader_permutations;
extern VertexDeclaration *vertex_declarations;

VertexBufferType rasterizer_dx9_vertex_get_dynamic_buffer_type(uint16_t dynamic_vertex_index) {
    IDirect3DDevice9 *device = rasterizer_dx9_device();
    ASSERT(device != NULL);
    ASSERT(dynamic_vertex_index != -1);
    return dynamic_vertices->buffers[dynamic_vertex_index].type;
}

IDirect3DVertexDeclaration9 *rasterizer_dx9_vertex_get_declaration(uint16_t vertex_buffer_type) {
    ASSERT(vertex_buffer_type < NUM_OF_VERTEX_DECLARATIONS);
    return vertex_declarations[vertex_buffer_type].declaration;
}
