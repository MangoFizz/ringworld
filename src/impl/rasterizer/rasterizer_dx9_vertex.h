#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_VERTEX_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_VERTEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "../tag/definitions/model.h"
#include "../memory/memory.h"
#include "rasterizer_dx9.h"

typedef enum VertexBufferType {
    VERTEX_BUFFER_TYPE_ENVIRONMENT_UNCOMPRESSED,
    VERTEX_BUFFER_TYPE_ENVIRONMENT_COMPRESSED,
    VERTEX_BUFFER_TYPE_ENVIRONMENT_LIGHTMAP_UNCOMPRESSED,
    VERTEX_BUFFER_TYPE_ENVIRONMENT_LIGHTMAP_COMPRESSED,
    VERTEX_BUFFER_TYPE_MODEL_UNCOMPRESSED,
    VERTEX_BUFFER_TYPE_MODEL_COMPRESSED,
    VERTEX_BUFFER_TYPE_UNLIT,
    VERTEX_BUFFER_TYPE_DYNAMIC_UNLIT,
    VERTEX_BUFFER_TYPE_DYNAMIC_SCREEN,
    VERTEX_BUFFER_TYPE_DEBUG,
    VERTEX_BUFFER_TYPE_DECAL,
    VERTEX_BUFFER_TYPE_DETAIL_OBJECT,
    VERTEX_BUFFER_TYPE_ENVIRONMENT_UNCOMPRESSED_FF,
    VERTEX_BUFFER_TYPE_ENVIRONMENT_LIGHTMAP_UNCOMPRESSED_FF,
    VERTEX_BUFFER_TYPE_MODEL_UNCOMPRESSED_FF,
    VERTEX_BUFFER_TYPE_MODEL_PROCESSED,
    VERTEX_BUFFER_TYPE_UNLIT_ZSPRITE,
    VERTEX_BUFFER_TYPE_SCREEN_TRANSFORMED_LIT,
    VERTEX_BUFFER_TYPE_SCREEN_TRANSFORMED_LIT_SPECULAR,
    VERTEX_BUFFER_TYPE_ENVIRONMENT_SINGLE_STREAM_FF,
    NUMBER_OF_VERTEX_BUFFER_TYPES
} VertexBufferType;

enum {
    MAX_DYNAMIC_TRIANGLES = 0x8000,
    MAX_DYNAMIC_TRIANGLES_BUFFERS = 0x400,
    MAX_DYNAMIC_VERTICES_BUFFER_GROUPS = 0x13,
    MAX_DYNAMIC_VERTICES_BUFFERS = 0x400,
};

enum VertexDeclarationIndex {
    VERTEX_DECLARATION_ENVIRONMENT,
    VERTEX_DECLARATION_ENVIRONMENT_C,
    VERTEX_DECLARATION_ENVIRONMENT_LIGHTMAP,
    VERTEX_DECLARATION_ENVIRONMENT_LIGHTMAP_C,
    VERTEX_DECLARATION_MODEL,
    VERTEX_DECLARATION_MODEL_C,
    VERTEX_DECLARATION_UNLIT,
    VERTEX_DECLARATION_DYNAMIC_UNLIT,
    VERTEX_DECLARATION_SCREEN,
    VERTEX_DECLARATION_DEBUG,
    VERTEX_DECLARATION_DECAL,
    VERTEX_DECLARATION_DETAIL_OBJECT,
    VERTEX_DECLARATION_ENVIRONMENT_FF,
    VERTEX_DECLARATION_ENVIRONMENT_LIGHTMAP_FF,
    VERTEX_DECLARATION_MODEL_UNCOMPRESSED_FF,
    VERTEX_DECLARATION_MODEL_PROCESSED,
    VERTEX_DECLARATION_UNLIT_ZSPRITE,
    VERTEX_DECLARATION_WIDGET,
    NUM_OF_VERTEX_DECLARATIONS,
};

typedef struct RasterizerTriangle {
    uint16_t vertex_indices[3];
} RasterizerTriangle;
_Static_assert(sizeof(RasterizerTriangle) == 0x6);

typedef struct TriangleBuffer {
    TriangleBufferType type;
    int32_t triangles_count;
    RasterizerTriangle *data;
    void *hardware_format;
} TriangleBuffer;
_Static_assert(sizeof(TriangleBuffer) == 0x10);

typedef struct DynamicTriangles {
    TriangleBuffer buffers[MAX_DYNAMIC_TRIANGLES_BUFFERS];
    int buffer_count;
    int triangle_count;
    void *d3d_index_buffer;
} DynamicTriangles;
_Static_assert(sizeof(DynamicTriangles) == 0x400C);

typedef struct DynamicVertexBufferGroup {
    int32_t vertex_count;
    int32_t max_vertex_count;
    uint32_t vertex_buffer_index;
} DynamicVertexBufferGroup;
_Static_assert(sizeof(DynamicVertexBufferGroup) == 0xC);

typedef struct VertexBuffer {
    VertexBufferType type;
    int32_t vertex_start_index;
    int32_t vertex_buf_offset;
    int32_t vertex_count;
    void *data;
} VertexBuffer; 
_Static_assert(sizeof(VertexBuffer) == 0x14);

typedef struct DynamicVertices {
    DynamicVertexBufferGroup groups[MAX_DYNAMIC_VERTICES_BUFFER_GROUPS];
    VertexBuffer buffers[MAX_DYNAMIC_VERTICES_BUFFERS];
    int32_t buffer_count;
} DynamicVertices;
_Static_assert(sizeof(DynamicVertices) == 0x50E8);

typedef struct VertexDeclaration {
    IDirect3DVertexDeclaration9 *declaration;
    uint32_t fvf;
    uint32_t vertex_processing_method;
} VertexDeclaration;
_Static_assert(sizeof(VertexDeclaration) == 0xC);

/**
 * Get the dynamic vertex buffer type
 * @param dynamic_vertex_index  The index of the dynamic vertex buffer.
 * @return The dynamic vertex buffer type.
 */
VertexBufferType rasterizer_dx9_vertex_get_dynamic_buffer_type(uint16_t dynamic_vertex_index);

/**
 * Get the vertex declaration of a vertex buffer type.
 * @param vertex_buffer_type The index of the vertex buffer.
 * @return The vertex declaration interface.
 */
IDirect3DVertexDeclaration9 *rasterizer_dx9_vertex_get_declaration(uint16_t vertex_buffer_type);

#ifdef __cplusplus
}
#endif

#endif
