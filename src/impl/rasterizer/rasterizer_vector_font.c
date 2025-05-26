#include <stdio.h>
#include <d3dx9.h>
#include "../exception/exception.h"
#include "../font/font.h"
#include "../math/color.h"
#include "../tag/definitions/vector_font.h"
#include "../tag/definitions/vector_font_data.h"
#include "../text/text.h"
#include "../render/render.h"
#include "../memory/dynamic_array.h"
#include "rasterizer.h"
#include "rasterizer_dx9.h"
#include "rasterizer_dx9_render_target.h"
#include "rasterizer_vector_font.h"


static float get_scale_factor(void) {
    RasterizerDx9RenderTarget *render_target = rasterizer_dx9_render_target_get(RENDER_TARGET_BACK_BUFFER);
    return (float)render_target->height / RASTERIZER_SCREEN_BASE_HEIGHT;
}

static uint16_t upscale_offset(uint16_t offset) {
    float scale = get_scale_factor();
    return ceil(offset * scale);
}

static uint16_t calculate_space_width(ID3DXFont *font) {
    RECT wrapper_rect = {0, 0, 0, 0};
    ID3DXFont_DrawTextW(font, NULL, L"_", -1, &wrapper_rect, DT_CALCRECT, 0xFFFFFFFF);
    return wrapper_rect.right;
}

void *rasterizer_vector_font_load_data(VectorFontData *font_data) {
    if(font_data->resource_handle == NULL) {
        void *font_data_blob = font_data->font_data.pointer;
        size_t font_data_size = font_data->font_data.size;
        DWORD num_fonts = 0;
        font_data->resource_handle = AddFontMemResourceEx(font_data_blob, font_data_size, NULL, &num_fonts);
    }
}

void rasterizer_vector_font_release_data(VectorFontData *font_data) {
    if(font_data->resource_handle != NULL) {
        RemoveFontMemResourceEx(font_data->resource_handle);
        font_data->resource_handle = NULL;
    }
}

ID3DXFont *rasterizer_vector_font_get_hardware_format(VectorFont *font, FontStyle font_style) {
    VectorFontStyle *font_style_data = vector_font_get_style(font, font_style);
    VectorFontData *font_data = tag_get_data(TAG_GROUP_VECTOR_FONT_DATA, font_style_data->data.tag_handle);
    rasterizer_vector_font_load_data(font_data);

    if(font_style_data->hardware_format != NULL) {
        return font_style_data->hardware_format;
    }

    IDirect3DDevice9 *device = rasterizer_dx9_device();
    float scaled_size = font->font_size * get_scale_factor();
    const char *font_name = font_data->font_family_name.string;
    font_style_data->hardware_format = NULL;
    HRESULT hr = D3DXCreateFontA(device, scaled_size, 0, 400, 1, font_style == FONT_STYLE_ITALIC, DEFAULT_CHARSET, 
                                OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                                font_name, (ID3DXFont **)&font_style_data->hardware_format);
    if(FAILED(hr)) {
        CRASHF_DEBUG("Failed to create D3DX9 font instance");
    }

    font_style_data->space_width = calculate_space_width(font_style_data->hardware_format);

    return font_style_data->hardware_format;
}

void rasterizer_vector_font_release_hardware_format(VectorFontStyle *style) {
    if(style->hardware_format != NULL) {
        ID3DXFont *font = style->hardware_format;
        ID3DXFont_Release(font);
        style->hardware_format = NULL;
    }
}

void rasterizer_vector_fonts_flush(void) {
    IDirect3DDevice9 *device = rasterizer_dx9_device();
    TagDataHeader *tag_header = tag_get_data_header();
    if(!device || !tag_header || !tag_header->tags) {
        return;
    }
    /**
     * @todo Implement an iterator for the tag array to avoid loops
     */
    for(size_t i = 0; i < tag_header->tag_count; i++) {
        TagEntry *tag_entry = &tag_header->tags[i];
        if(tag_entry->primary_group == TAG_GROUP_VECTOR_FONT) {
            VectorFont *font = tag_entry->data;
            if(font != NULL) {
                rasterizer_vector_font_release_hardware_format(&font->regular);
                rasterizer_vector_font_release_hardware_format(&font->bold);
                rasterizer_vector_font_release_hardware_format(&font->italic);
                rasterizer_vector_font_release_hardware_format(&font->condensed);
                rasterizer_vector_font_release_hardware_format(&font->underline);
            }
            continue;
        }
        if(tag_entry->primary_group == TAG_GROUP_VECTOR_FONT_DATA) {
            VectorFontData *font_data = tag_entry->data;
            rasterizer_vector_font_release_data(font_data);
            continue;
        }
    }
}

void rasterizer_vector_font_calculate_unicode_string_draw_bounds(const wchar_t *string, VectorFont *font, FontStyle font_style, Rectangle2D *bounds) {
    VectorFontStyle *font_style_data = vector_font_get_style(font, font_style);
    ID3DXFont *d3dx9_font = rasterizer_vector_font_get_hardware_format(font, font_style);
    float scale = get_scale_factor();
    uint16_t screen_width = render_get_screen_width();
    uint16_t screen_height = render_get_screen_height();

    bounds->left = INT16_MAX;
    bounds->top = INT16_MAX;
    bounds->right = INT16_MIN;
    bounds->bottom = INT16_MIN;
    
    DynamicArray text_rects;
    vector_font_handle_unicode_string_formatting(string, 0, 0, screen_width, screen_height, &text_rects);
    for(size_t i = 0; i < text_rects.lenght; i++) {
        VectorFontTextRect *text_rect = dynamic_array_get(&text_rects, i);
        wchar_t text[wcslen(text_rect->text.data) + 2];
        swprintf_s(text, SIZEOF_ARRAY(text), L"%s_", text_rect->text.data);

        RECT rect = { 0, 0, 0, 0 };
        ID3DXFont_DrawTextW(d3dx9_font, NULL, text, -1, &rect, DT_CALCRECT, 0xFFFFFFFF);
        int16_t width = (float)(rect.right - font_style_data->space_width) / scale;
        int16_t height = (float)rect.bottom / scale;

        if(text_rect->x < bounds->left) {
            bounds->left = text_rect->x;
        }
        if(text_rect->y < bounds->top) {
            bounds->top = text_rect->y;
        }
        if(bounds->right < text_rect->x + width) {
            bounds->right = text_rect->x + width;
        }
        if(bounds->bottom < height + text_rect->y) {
            bounds->bottom = height + text_rect->y;
        }

        dynamic_buffer_free(&text_rect->text);
    }
    if(text_rects.lenght == 0) {
        bounds->left = 0;
        bounds->top = 0;
        bounds->right = 0;
        bounds->bottom = font->font_size;
    }
    dynamic_array_free(&text_rects);
}

void rasterizer_vector_font_calculate_string_draw_bounds(const char *string, VectorFont *font, FontStyle font_style, Rectangle2D *bounds) {
    VectorFontStyle *font_style_data = vector_font_get_style(font, font_style);
    ID3DXFont *d3dx9_font = rasterizer_vector_font_get_hardware_format(font, font_style);
    float scale = get_scale_factor();
    uint16_t screen_width = render_get_screen_width();
    uint16_t screen_height = render_get_screen_height();

    bounds->left = INT16_MAX;
    bounds->top = INT16_MAX;
    bounds->right = INT16_MIN;
    bounds->bottom = INT16_MIN;
    
    DynamicArray text_rects;
    vector_font_handle_string_formatting(string, 0, 0, screen_width, screen_height, &text_rects);
    for(size_t i = 0; i < text_rects.lenght; i++) {
        VectorFontTextRect *text_rect = dynamic_array_get(&text_rects, i);
        char text[strlen(text_rect->text.data) + 2];
        sprintf_s(text, SIZEOF_ARRAY(text), "%s_", text_rect->text.data);

        RECT rect = { 0, 0, 0, 0 };
        ID3DXFont_DrawTextA(d3dx9_font, NULL, text, -1, &rect, DT_CALCRECT, 0xFFFFFFFF);
        int16_t width = (float)(rect.right - font_style_data->space_width) / scale;
        int16_t height = (float)rect.bottom / scale;

        if(text_rect->x < bounds->left) {
            bounds->left = text_rect->x;
        }
        if(text_rect->y < bounds->top) {
            bounds->top = text_rect->y;
        }
        if(bounds->right < text_rect->x + width) {
            bounds->right = text_rect->x + width;
        }
        if(bounds->bottom < height + text_rect->y) {
            bounds->bottom = height + text_rect->y;
        }

        dynamic_buffer_free(&text_rect->text);
    }
    if(text_rects.lenght == 0) {
        bounds->left = 0;
        bounds->top = 0;
        bounds->right = 0;
        bounds->bottom = font->font_size;
    }
    dynamic_array_free(&text_rects);
}

void rasterizer_vector_font_draw_unicode_string(const Rectangle2D *bounds, const Rectangle2D *text_rect, VectorXYInt *offset_out, uint32_t flags, const wchar_t *string) {
    RasterizerWindowRenderParameters *window_parameters = rasterizer_get_window_parameters();
    TextDrawGlobals *text_globals = text_get_drawing_globals();
    VectorFont *font = tag_get_data(TAG_GROUP_VECTOR_FONT, text_globals->font);
    VectorFontStyle *font_style_data = vector_font_get_style(font, text_globals->style);
    ID3DXFont *d3dx9_font = rasterizer_vector_font_get_hardware_format(font, text_globals->style);
    float scale = get_scale_factor();
    uint16_t screen_width = render_get_screen_width();
    uint16_t screen_height = render_get_screen_height();

    if(render_user_interface_enabled() == false || window_parameters->render_target != 1) {
        return;
    }

    RECT rect;
    rect.left = bounds->left;
    rect.right = bounds->right;
    rect.top = bounds->top;
    rect.bottom = bounds->bottom;

    Rectangle2D text_final_rect;
    text_final_rect.top = INT16_MAX;
    text_final_rect.left = INT16_MAX;
    text_final_rect.bottom = INT16_MIN;
    text_final_rect.right = INT16_MIN;

    D3DCOLOR encoded_color = color_encode_a8r8g8b8(&text_globals->color);

    // Calculate the alignment offset based on the justification
    int16_t align_offset_x = 0;
    Rectangle2D text_bounds;
    rasterizer_vector_font_calculate_unicode_string_draw_bounds(string, font, text_globals->style, &text_bounds);
    switch(text_globals->justification) {
        case TEXT_JUSTIFICATION_RIGHT:
            align_offset_x += (rect.right - rect.left) - (text_bounds.right - text_bounds.left);
            break;
        case TEXT_JUSTIFICATION_CENTER:
            align_offset_x += ((rect.right - rect.left) - (text_bounds.right - text_bounds.left)) / 2;
            break;
    }

    DynamicArray text_rects;
    vector_font_handle_unicode_string_formatting(string, rect.left + align_offset_x, rect.top, rect.right - rect.left, rect.bottom - rect.top, &text_rects);
    for(size_t i = 0; i < text_rects.lenght; i++) {
        VectorFontTextRect *text_rect = dynamic_array_get(&text_rects, i);

        int16_t scaled_font_offset_x = font->offset_x * scale;
        int16_t scaled_font_offset_y = font->offset_y * scale;

        RECT text_rect_area;
        text_rect_area.left = ceil(text_rect->x * scale + scaled_font_offset_x);
        text_rect_area.right = ceil((text_rect->x + text_rect->width) * scale + scaled_font_offset_x);
        text_rect_area.top = ceil(text_rect->y * scale + scaled_font_offset_y);
        text_rect_area.bottom = ceil((text_rect->y + text_rect->height) * scale + scaled_font_offset_y);

        D3DCOLOR shadow_color = text_get_shadow_color() | (0xFF000000 & encoded_color);
        
        RECT shadow_rect_area = text_rect_area;
        shadow_rect_area.left += 1 * scale;
        shadow_rect_area.right += 1 * scale;
        shadow_rect_area.top += 1 * scale;
        shadow_rect_area.bottom += 1 * scale;

        ID3DXFont_DrawTextW(d3dx9_font, NULL, text_rect->text.data, -1, &shadow_rect_area, DT_LEFT, shadow_color);

        ID3DXFont_DrawTextW(d3dx9_font, NULL, text_rect->text.data, -1, &text_rect_area, DT_LEFT, encoded_color);

        if(offset_out != NULL) {
            if(text_rect->x < text_final_rect.left) {
                text_final_rect.left = text_rect->x;
            }
            if(text_rect->y < text_final_rect.top) {
                text_final_rect.top = text_rect->y;
            }
            if(text_final_rect.right < text_rect->x + text_rect->width) {
                text_final_rect.right = text_rect->x + text_rect->width;
            }
            if(text_final_rect.bottom < text_rect->height + text_rect->y) {
                text_final_rect.bottom = text_rect->height + text_rect->y;
            }
        }

        dynamic_buffer_free(&text_rect->text);
    }

    dynamic_array_free(&text_rects);

    if(offset_out != NULL) {
        offset_out->x = text_final_rect.left;
        offset_out->y = text_final_rect.top;
    }
}

void rasterizer_vector_font_draw_string(const Rectangle2D *bounds, const Rectangle2D *text_rect, VectorXYInt *offset_out, uint32_t flags, const char *string) {
    RasterizerWindowRenderParameters *window_parameters = rasterizer_get_window_parameters();
    TextDrawGlobals *text_globals = text_get_drawing_globals();
    VectorFont *font = tag_get_data(TAG_GROUP_VECTOR_FONT, text_globals->font);
    VectorFontStyle *font_style_data = vector_font_get_style(font, text_globals->style);
    ID3DXFont *d3dx9_font = rasterizer_vector_font_get_hardware_format(font, text_globals->style);
    float scale = get_scale_factor();
    uint16_t screen_width = render_get_screen_width();
    uint16_t screen_height = render_get_screen_height();

    if(render_user_interface_enabled() == false || window_parameters->render_target != 1) {
        return;
    }

    RECT rect;
    rect.left = bounds->left;
    rect.right = bounds->right;
    rect.top = bounds->top;
    rect.bottom = bounds->bottom;

    Rectangle2D text_final_rect;
    text_final_rect.top = INT16_MAX;
    text_final_rect.left = INT16_MAX;
    text_final_rect.bottom = INT16_MIN;
    text_final_rect.right = INT16_MIN;

    D3DCOLOR encoded_color = color_encode_a8r8g8b8(&text_globals->color);

    // Calculate the alignment offset based on the justification
    int16_t align_offset_x = 0;
    Rectangle2D text_bounds;
    rasterizer_vector_font_calculate_string_draw_bounds(string, font, text_globals->style, &text_bounds);
    switch(text_globals->justification) {
        case TEXT_JUSTIFICATION_LEFT:
            align_offset_x = 0;
            break;
        case TEXT_JUSTIFICATION_RIGHT:
            align_offset_x = (rect.right - rect.left) - (text_bounds.right - text_bounds.left);
            break;
        case TEXT_JUSTIFICATION_CENTER:
            align_offset_x = ((rect.right - rect.left) - (text_bounds.right - text_bounds.left)) / 2;
            break;
    }

    DynamicArray text_rects;
    vector_font_handle_string_formatting(string, rect.left + align_offset_x, rect.top, rect.right - rect.left, rect.bottom - rect.top, &text_rects);
    for(size_t i = 0; i < text_rects.lenght; i++) {
        VectorFontTextRect *text_rect = dynamic_array_get(&text_rects, i);

        int16_t scaled_font_offset_x = font->offset_x * scale;
        int16_t scaled_font_offset_y = font->offset_y * scale;

        RECT text_rect_area;
        text_rect_area.left = ceil(text_rect->x * scale + scaled_font_offset_x);
        text_rect_area.right = ceil((text_rect->x + text_rect->width) * scale + scaled_font_offset_x);
        text_rect_area.top = ceil(text_rect->y * scale + scaled_font_offset_y);
        text_rect_area.bottom = ceil((text_rect->y + text_rect->height) * scale + scaled_font_offset_y);

        D3DCOLOR shadow_color = text_get_shadow_color() | (0xFF000000 & encoded_color);
        
        RECT shadow_rect_area = text_rect_area;
        shadow_rect_area.left += 1 * scale;
        shadow_rect_area.right += 1 * scale;
        shadow_rect_area.top += 1 * scale;
        shadow_rect_area.bottom += 1 * scale;

        ID3DXFont_DrawTextA(d3dx9_font, NULL, text_rect->text.data, -1, &shadow_rect_area, DT_LEFT, shadow_color);

        ID3DXFont_DrawTextA(d3dx9_font, NULL, text_rect->text.data, -1, &text_rect_area, DT_LEFT, encoded_color);

        if(offset_out != NULL) {
            if(text_rect->x < text_final_rect.left) {
                text_final_rect.left = text_rect->x;
            }
            if(text_rect->y < text_final_rect.top) {
                text_final_rect.top = text_rect->y;
            }
            if(text_final_rect.right < text_rect->x + text_rect->width) {
                text_final_rect.right = text_rect->x + text_rect->width;
            }
            if(text_final_rect.bottom < text_rect->height + text_rect->y) {
                text_final_rect.bottom = text_rect->height + text_rect->y;
            }
        }

        dynamic_buffer_free(&text_rect->text);
    }
    
    dynamic_array_free(&text_rects);

    if(offset_out != NULL) {
        offset_out->x = text_final_rect.left;
        offset_out->y = text_final_rect.top;
    }
}
