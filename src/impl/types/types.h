#ifndef RINGWORLD__TYPES__TYPES_H
#define RINGWORLD__TYPES__TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../memory/table.h"

typedef uint32_t FourCC;
typedef uint32_t TickCount32;
typedef uint16_t TickCount16;
typedef uint32_t ColorARGBInt;
typedef float Angle;

typedef union ScenarioScriptNodeValue {
    float f;
    TableResourceHandle id;
    int8_t b;
    int16_t s;
    int32_t l;
    const char *string;
} ScenarioScriptNodeValue;
_Static_assert(sizeof(ScenarioScriptNodeValue) == 0x4);

typedef struct ColorARGB {
    float a;
    float r;
    float g;
    float b;
} ColorARGB;
_Static_assert(sizeof(ColorARGB) == 0x10);

typedef struct ColorRGB {
    float r;
    float g;
    float b;
} ColorRGB;
_Static_assert(sizeof(ColorRGB) == 0xC);

typedef struct String32 {
    char string[32];
} String32;
_Static_assert(sizeof(String32) == 32);

typedef struct Data {
    uint32_t size;
    uint32_t flags;
    uint32_t file_offset; // only applies to sounds
    void *pointer;
    char padding[4];
} Data;
_Static_assert(sizeof(Data) == 0x14);

typedef struct VectorXYZ {
    float x;
    float y;
    float z;
} VectorXYZ;
_Static_assert(sizeof(VectorXYZ) == 0xC);

typedef struct VectorXY {
    float x;
    float y;
} VectorXY;
_Static_assert(sizeof(VectorXY) == 0x8);

typedef struct VectorXYInt {
    int16_t x;
    int16_t y;
} VectorXYInt;
_Static_assert(sizeof(VectorXYInt) == 0x4);

typedef struct VectorIJK {
    float i;
    float j;
    float k;
} VectorIJK;
_Static_assert(sizeof(VectorIJK) == 0xC);

typedef struct VectorIJ {
    float i;
    float j;
} VectorIJ;
_Static_assert(sizeof(VectorIJ) == 0x8);

typedef struct Quaternion {
    float i;
    float j;
    float k;
    float l;
} Quaternion;
_Static_assert(sizeof(Quaternion) == 0x10);

typedef struct VectorPYR {
    float pitch;
    float yaw;
    float rotation;
} VectorPYR;
_Static_assert(sizeof(VectorPYR) == 0xC);

typedef struct VectorPY {
    float pitch;
    float yaw;
} VectorPY;
_Static_assert(sizeof(VectorPY) == 0x8);

typedef struct Rectangle2D {
    int16_t top;
    int16_t left;
    int16_t bottom;
    int16_t right;
} Rectangle2D;
_Static_assert(sizeof(Rectangle2D) == 0x8);

typedef struct Bounds2D {
    float left;
    float right;
    float top;
    float bottom;
} Bounds2D;
_Static_assert(sizeof(Bounds2D) == 0x10);

typedef struct Rectangle3DF {
    float top;
    float left;
    float bottom;
    float right;
    float front;
    float back;
} Rectangle3DF;
_Static_assert(sizeof(Rectangle3DF) == 0x18);

typedef struct Plane2D {
    float i;
    float j;
    float w;
} Plane2D;
_Static_assert(sizeof(Plane2D) == 0xC);

typedef struct Plane3D {
    float i;
    float j;
    float k;
    float w;
} Plane3D;
_Static_assert(sizeof(Plane3D) == 0x10);

// TODO: REPLACE THIS STRUCTURE WITH A NEW ONE
typedef struct Matrix {
    VectorXYZ m[3];
} Matrix;

typedef struct Matrix4x3 {
    float scale;
    VectorIJK forward;
    VectorIJK left;
    VectorIJK up;
    VectorXYZ position;
} Matrix4x3;
_Static_assert(sizeof(Matrix4x3) == 0x34);

typedef struct GenericTagBlock {
    uint32_t count;
    void *elements;
    uint8_t padding[4];
} GenericTagBlock;
_Static_assert(sizeof(GenericTagBlock) == 0xC);

#ifdef __cplusplus
}
#endif

#endif
