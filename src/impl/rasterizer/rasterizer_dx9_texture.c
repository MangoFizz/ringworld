#include <stdbool.h>
#include <stdio.h>

#include "../exception/exception.h"
#include "../bitmap/bitmap.h"
#include "rasterizer_dx9_texture.h"
#include "rasterizer_dx9.h"

D3DFORMAT rasterizer_dx9_texture_get_d3d_format(BitmapDataFormat format) {
    switch(format) {
        case BITMAP_DATA_FORMAT_R5G6B5: 
            return D3DFMT_R5G6B5;
        case BITMAP_DATA_FORMAT_A1R5G5B5:
            return D3DFMT_A1R5G5B5;
        case BITMAP_DATA_FORMAT_A4R4G4B4:
            return D3DFMT_A4R4G4B4;
        case BITMAP_DATA_FORMAT_X8R8G8B8:
            return D3DFMT_X8R8G8B8;
        case BITMAP_DATA_FORMAT_A8R8G8B8:
            return D3DFMT_A8R8G8B8;
        case BITMAP_DATA_FORMAT_DXT1:
            return D3DFMT_DXT1;
        case BITMAP_DATA_FORMAT_DXT3: 
            return D3DFMT_DXT3;
        case BITMAP_DATA_FORMAT_DXT5:
            return D3DFMT_DXT5;
#ifdef RINGWORLD_ENABLE_ENHANCEMENTS
        case BITMAP_DATA_FORMAT_Y8:
            return D3DFMT_L8;
        case BITMAP_DATA_FORMAT_A8Y8:
            return D3DFMT_A8L8;
#endif
        default:
            printf("Unsupported bitmap format: %d\n", format);
            return D3DFMT_UNKNOWN;            
    }
}

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

bool rasterizer_dx9_texture_set_bitmap_data_texture_no_assert(uint32_t stage, uint16_t bitmap_data_index, TagHandle bitmap_tag) {
    ASSERT(stage >= 0 && stage < 4);

    Bitmap *bitmap = tag_get_data(TAG_GROUP_BITMAP, bitmap_tag);
    if(bitmap == NULL) {
        return false;
    }
    if(bitmap->bitmap_data.count > 0) {
        BitmapData *bitmap_data = bitmap_get_data(bitmap_tag, bitmap_data_index % bitmap->bitmap_data.count);
        if(bitmap_data != NULL) {
            bool loaded = rasterizer_dx9_texture_load_bitmap(true, true, bitmap_data);
            if(loaded) {
                IDirect3DTexture9 *texture = (IDirect3DTexture9 *)bitmap_data->hardware_format;
                rasterizer_dx9_set_texture(stage, texture);
                return true;
            }
        }
    }
    return true;
}
