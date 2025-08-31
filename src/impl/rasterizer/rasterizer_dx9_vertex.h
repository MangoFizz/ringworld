#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_VERTEX_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_VERTEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "../tag/definitions/model.h"
#include "../memory/memory.h"
#include "rasterizer_dx9.h"

enum {
    MAX_DYNAMIC_VERTICES_BUFFER_GROUPS = 0x13,
    MAX_DYNAMIC_VERTICES_BUFFERS = 0x400,
    MAX_DYNAMIC_TRIANGLES = 0x8000,
    MAX_DYNAMIC_TRIANGLES_BUFFERS = MAX_DYNAMIC_VERTICES_BUFFERS
};

typedef enum PACKED_ENUM VertexBufferType {
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
    NUMBER_OF_VERTEX_BUFFER_TYPES,
    SIZE_OF_VERTEX_BUFFER_TYPE = 0xFFFF
} VertexBufferType;
_Static_assert(sizeof(VertexBufferType) == sizeof(uint16_t));

typedef enum PACKED_ENUM VertexDeclarationIndex {
    VERTEX_DECLARATION_ENVIRONMENT_UNCOMPRESSED,
    VERTEX_DECLARATION_ENVIRONMENT_COMPRESSED,
    VERTEX_DECLARATION_ENVIRONMENT_LIGHTMAP_UNCOMPRESSED,
    VERTEX_DECLARATION_ENVIRONMENT_LIGHTMAP_COMPRESSED,
    VERTEX_DECLARATION_MODEL_UNCOMPRESSED,
    VERTEX_DECLARATION_MODEL_COMPRESSED,
    VERTEX_DECLARATION_UNLIT,
    VERTEX_DECLARATION_DYNAMIC_UNLIT,
    VERTEX_DECLARATION_DYNAMIC_SCREEN,
    VERTEX_DECLARATION_DEBUG,
    VERTEX_DECLARATION_DECAL,
    VERTEX_DECLARATION_DETAIL_OBJECT,
    VERTEX_DECLARATION_ENVIRONMENT_UNCOMPRESSED_FF,
    VERTEX_DECLARATION_ENVIRONMENT_LIGHTMAP_UNCOMPRESSED_FF,
    VERTEX_DECLARATION_MODEL_UNCOMPRESSED_FF,
    VERTEX_DECLARATION_MODEL_PROCESSED,
    VERTEX_DECLARATION_UNLIT_ZSPRITE,
    VERTEX_DECLARATION_SCREEN_TRANSFORMED_LIT,
    VERTEX_DECLARATION_SCREEN_TRANSFORMED_LIT_SPECULAR,
    VERTEX_DECLARATION_ENVIRONMENT_SINGLE_STREAM_FF,
    NUM_OF_VERTEX_DECLARATIONS,
    SIZE_OF_VERTEX_DECLARATION_INDEX = 0xFFFF
} VertexDeclarationIndex;
_Static_assert(sizeof(VertexDeclarationIndex) == sizeof(uint16_t));

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

typedef struct RasterizerDynamicVertex {
    VectorXYZ position;
    Pixel32 color;
    VectorXY texture_pos;
} RasterizerDynamicVertex;
_Static_assert(sizeof(RasterizerDynamicVertex) == 0x18);

/**
 * Get the triangles buffer type corresponding to a dynamic vertex buffer.
 * @param dynamic_vertex_index  The index of the dynamic vertex buffer.
 * @return The dynamic triangles buffer type.
 */
TriangleBufferType rasterizer_dx9_vertex_get_dynamic_buffer_type(uint16_t dynamic_vertex_index);

/**
 * Get the vertex declaration of a vertex buffer type.
 * @param vertex_buffer_type The index of the vertex buffer.
 * @return The vertex declaration interface.
 */
IDirect3DVertexDeclaration9 *rasterizer_dx9_vertex_get_declaration(VertexDeclarationIndex vertex_buffer_type);

/**
 * Get the processing method of a vertex buffer type.
 * @param vertex_buffer_type The index of the vertex buffer.
 * @return The processing method of the vertex buffer.
 */
uint32_t rasterizer_dx9_vertex_get_processing_method(VertexDeclarationIndex vertex_buffer_type);

#ifdef __cplusplus
}
#endif

#endif
