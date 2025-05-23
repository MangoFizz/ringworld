#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_VERTEX_SHADER_CONSTANTS_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_VERTEX_SHADER_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../math/math.h"

#define FLOAT4_SIZE (sizeof(float) * 4)
#define FLOAT4_ALIGNED __attribute__((packed, aligned(FLOAT4_SIZE)))

enum VertexShaderConstants {
    VSH_CONSTANTS_VIEWPROJ_OFFSET = 0,
    VSH_CONSTANTS_VIEWPROJ_COUNT = 6,
    VSH_CONSTANTS_FOG_OFFSET = 6,
    VSH_CONSTANTS_FOG_COUNT = 4,
    VSH_CONSTANTS_TEXSCALE_OFFSET = 10,
    VSH_CONSTANTS_TEXSCALE_COUNT = 3,
    VSH_CONSTANTS_FAKELIGHT_OFFSET = 13,
    VSH_CONSTANTS_FAKELIGHT_COUNT = 4,
    VSH_CONSTANTS_POINTLIGHT_OFFSET = 13,
    VSH_CONSTANTS_POINTLIGHT_COUNT = 5,
    VSH_CONSTANTS_TEXANIM_OFFSET = 13,
    VSH_CONSTANTS_TEXANIM_COUNT = 8,
    VSH_CONSTANTS_SHADOW_OFFSET = 13,
    VSH_CONSTANTS_SHADOW_COUNT = 5,
    VSH_CONSTANTS_EFFECT_OFFSET = 13,
    VSH_CONSTANTS_EFFECT_COUNT = 2,
    VSH_CONSTANTS_VERTEXLIGHT_OFFSET = 15,
    VSH_CONSTANTS_VERTEXLIGHT_COUNT = 11,
    VSH_CONSTANTS_INVERSE_OFFSET = 26,
    VSH_CONSTANTS_INVERSE_COUNT = 3,
    VSH_CONSTANTS_NODEMATRIX_OFFSET = 29,
    VSH_CONSTANTS_NODEMATRIX_COUNT = 63 * 3,
    VSH_CONSTANTS_SCREENPROJ_OFFSET = 13,
    VSH_CONSTANTS_SCREENPROJ_COUNT = 5,
    VSH_CONSTANTS_SCREENPROJ2_OFFSET = 18,
    VSH_CONSTANTS_SCREENPROJ2_COUNT = 6,
    VSH_CONSTANTS_ZSPRITE_OFFSET = 18,
    VSH_CONSTANTS_ZSPRITE_COUNT = 2,
    VSH_CONSTANTS_TINTFACTOR_OFFSET = 18,
    VSH_CONSTANTS_TINTFACTOR_COUNT = 1,
    VSH_CONSTANTS_EYEXFORM_OFFSET = 27,
    VSH_CONSTANTS_EYEXFORM_COUNT = 2,
    VSH_CONSTANTS_DETAILOBJ_OFFSET = 13,
    VSH_CONSTANTS_DETAILOBJ_COUNT = 6,
    VSH_CONSTANTS_DETAILOBJ_TYPEDATA_OFFSET = 19,
    VSH_CONSTANTS_DETAILOBJ_FRAMEDATA_OFFSET = 29,
    VSH_CONSTANTS_DETAILOBJ_CELL_ORIGIN_OFFSET = 18,
    VSH_CONSTANTS_DETAILOBJ_XFORM_Z_OFFSET = 19,
    VSH_CONSTANTS_DECALCOLOR_OFFSET = 10,
    
    MAX_VSH_CONSTANTS = VSH_CONSTANTS_NODEMATRIX_OFFSET + VSH_CONSTANTS_NODEMATRIX_COUNT
};

typedef struct FLOAT4_ALIGNED VertexShaderScreenprojConstants {
    ProjectionMatrix projection;
    VectorXY texture_scale;
    float pad1;
    float pad2;
} VertexShaderScreenprojConstants;
_Static_assert(sizeof(VertexShaderScreenprojConstants) == VSH_CONSTANTS_SCREENPROJ_COUNT * FLOAT4_SIZE);

typedef struct FLOAT4_ALIGNED VertexShaderScreenproj2Constants {
    VectorXY screen_texture_scales_1;
    VectorXY screen_texture_scales_2;
    VectorXY screen_mask_0;
    VectorXY screen_mask_1;
    VectorXY screen_mask_2;
    VectorXY screen_offset_0;
    VectorXY screen_offset_1;
    VectorXY screen_offset_2;
    VectorXY screen_scale_0;
    VectorXY screen_scale_1;
    VectorXY screen_scale_2;
} VertexShaderScreenproj2Constants;
_Static_assert(sizeof(VertexShaderScreenproj2Constants) == VSH_CONSTANTS_SCREENPROJ2_COUNT * FLOAT4_SIZE);

typedef struct FLOAT4_ALIGNED VertexShaderTexanimConstants {
    Plane3D tex0_transform_x;
    Plane3D tex0_transform_y;
    Plane3D tex1_transform_x;
    Plane3D tex1_transform_y;
    Plane3D tex2_transform_x;
    Plane3D tex2_transform_y;
    Plane3D tex3_transform_x;
    Plane3D tex3_transform_y;
} VertexShaderTexanimConstants;
_Static_assert(sizeof(VertexShaderTexanimConstants) == VSH_CONSTANTS_TEXANIM_COUNT * FLOAT4_SIZE);

#ifdef __cplusplus
}
#endif

#endif
