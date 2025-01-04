#include <stdbool.h>
#include <stdio.h>

#include "../tag/definitions/bitmap.h"
#include "../exception/exception.h"
#include "rasterizer_dx9.h"

D3DFORMAT rasterizer_dx9_texture_get_d3d_format(BitmapDataFormat format) {
    switch(format) {
        case BITMAP_DATA_FORMAT_BITMAP_DATA_FORMAT__R5_G6_B5: 
            return D3DFMT_R5G6B5;
        case BITMAP_DATA_FORMAT_BITMAP_DATA_FORMAT__A1_R5_G5_B5:
            return D3DFMT_A1R5G5B5;
        case BITMAP_DATA_FORMAT_BITMAP_DATA_FORMAT__A4_R4_G4_B4:
            return D3DFMT_A4R4G4B4;
        case BITMAP_DATA_FORMAT_BITMAP_DATA_FORMAT__X8_R8_G8_B8:
            return D3DFMT_X8R8G8B8;
        case BITMAP_DATA_FORMAT_BITMAP_DATA_FORMAT__A8_R8_G8_B8:
            return D3DFMT_A8R8G8B8;
        case BITMAP_DATA_FORMAT_BITMAP_DATA_FORMAT__D_X_T1:
            return D3DFMT_DXT1;
        case BITMAP_DATA_FORMAT_BITMAP_DATA_FORMAT__D_X_T3: 
            return D3DFMT_DXT3;
        case BITMAP_DATA_FORMAT_BITMAP_DATA_FORMAT__D_X_T5:
            return D3DFMT_DXT5;
#ifdef RINGWORLD_ENABLE_ENHANCEMENTS
        case BITMAP_DATA_FORMAT_BITMAP_DATA_FORMAT__A8:
            return D3DFMT_A8;
        case BITMAP_DATA_FORMAT_BITMAP_DATA_FORMAT__Y8:
        case BITMAP_DATA_FORMAT_BITMAP_DATA_FORMAT__A_Y8:
            return D3DFMT_L8;
        case BITMAP_DATA_FORMAT_BITMAP_DATA_FORMAT__A8_Y8:
            return D3DFMT_A8L8;
#endif
        case BITMAP_DATA_FORMAT_BITMAP_DATA_FORMAT__B_C7:
        default:
            printf("Unsupported bitmap format: %d\n", format);
            return D3DFMT_UNKNOWN;            
    }
}

uint32_t rasterizer_dx9_texture_calculate_mipmaps_levels(uint16_t *height, BitmapData *bitmap, uint16_t *width);

bool rasterizer_dx9_texture_create(BitmapData *bitmap) {
    ASSERT(bitmap != NULL);
    
    IDirect3DDevice9 *device = rasterizer_dx9_device();
    ASSERT(device != NULL);

    bitmap->hardware_format = NULL;

    D3DFORMAT format = rasterizer_dx9_texture_get_d3d_format(bitmap->format);
    if(format == D3DFMT_UNKNOWN) {
        return true;
    }

    switch(bitmap->type) {
        case BITMAP_TYPE_2D_TEXTURES: {
            VectorXYInt dimentions = {0};
            uint32_t mipmap_level = rasterizer_dx9_texture_calculate_mipmaps_levels(&dimentions.y, bitmap, &dimentions.x);
            HRESULT result = IDirect3DDevice9_CreateTexture(device, dimentions.x, dimentions.y, bitmap->mipmap_count - mipmap_level + 1, 
                                                            0, format, D3DPOOL_MANAGED, (IDirect3DTexture9 **)&bitmap->hardware_format, NULL);
            if(FAILED(result)) {
                return false;
            }
            break;                                                            
        }

        case BITMAP_TYPE_3D_TEXTURES: {
            D3DCAPS9 *caps = rasterizer_dx9_device_caps();
            if(caps->TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP != 0) {
                uint32_t mipmap_count = 1;
                if(caps->TextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP != 0) {
                    mipmap_count = bitmap->mipmap_count + 1;
                }
                
                HRESULT result = IDirect3DDevice9_CreateVolumeTexture(device, bitmap->width, bitmap->height, bitmap->depth, mipmap_count,
                                                                    0, format, D3DPOOL_MANAGED, (IDirect3DVolumeTexture9 **)&bitmap->hardware_format, NULL);
                if(FAILED(result)) {
                    return false;
                }
            }
            break;
        }

        case BITMAP_TYPE_CUBE_MAPS: {
            D3DCAPS9 *caps = rasterizer_dx9_device_caps();
            if(caps->TextureCaps & D3DPTEXTURECAPS_CUBEMAP != 0) {
                uint32_t mipmap_count = 1;
                if(caps->TextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP != 0) {
                    mipmap_count = bitmap->mipmap_count + 1;
                }
                
                HRESULT result = IDirect3DDevice9_CreateCubeTexture(device, bitmap->width, mipmap_count, 0, format, D3DPOOL_MANAGED, 
                                                                    (IDirect3DCubeTexture9 **)&bitmap->hardware_format, NULL);
                if(FAILED(result)) {
                    return false;
                }
            }
            break;
        }

        default:
            break;
    }

    if(bitmap->hardware_format == NULL) {
        return false;
    }

    return true;
}
