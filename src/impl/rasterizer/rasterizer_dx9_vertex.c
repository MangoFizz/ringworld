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

IDirect3DVertexDeclaration9 *rasterizer_dx9_vertex_get_declaration(VertexDeclarationIndex vertex_buffer_type) {
    ASSERT(vertex_buffer_type >= 0 && vertex_buffer_type < NUM_OF_VERTEX_DECLARATIONS);
    return vertex_declarations[vertex_buffer_type].declaration;
}

uint32_t rasterizer_dx9_vertex_get_processing_method(VertexDeclarationIndex vertex_buffer_type) {
    ASSERT(vertex_buffer_type >= 0 && vertex_buffer_type < NUM_OF_VERTEX_DECLARATIONS);
    int mask = 0;
    if(rasterizer_dx9_device_supports_software_vertex_processing()) {
        mask = D3DUSAGE_SOFTWAREPROCESSING; 
    }
    return vertex_declarations[vertex_buffer_type].vertex_processing_method | mask;
}
