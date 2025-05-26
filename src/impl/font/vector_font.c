#include <windows.h>
#include "../exception/exception.h"
#include "../font/font.h"
#include "../rasterizer/rasterizer_vector_font.h"
#include "../tag/definitions/vector_font.h"
#include "../tag/definitions/vector_font_data.h"
#include "../memory/dynamic_array.h"
#include "font.h"
#include "vector_font.h"

VectorFontStyle *vector_font_get_style(VectorFont *font, FontStyle style) {
    switch(style) {
        case FONT_STYLE_BOLD:
            return &font->bold;
        case FONT_STYLE_ITALIC:
            return &font->italic;
        case FONT_STYLE_CONDENSED:
            return &font->condensed;
        case FONT_STYLE_UNDERLINE:
            return &font->underline;
        default:
            return &font->regular;
    }
}

static void add_new_unicode_string_rect(DynamicArray *text_recs, const wchar_t *str, size_t tabs, int16_t start, int16_t end, int16_t x, int16_t y, int16_t width, int16_t height, TextJustification justification) {
    int16_t substr_size = end - start;
    if(substr_size == 0) {
        return;
    }

    VectorFontTextRect *new_fmt = dynamic_array_push(text_recs, NULL);
    
    dynamic_buffer_init(&new_fmt->text);
    dynamic_buffer_push_data(&new_fmt->text, str + start, substr_size * sizeof(wchar_t));
    dynamic_buffer_push_data(&new_fmt->text, L"\0", sizeof(wchar_t));

    new_fmt->x = x;
    new_fmt->width = width;
    if(tabs > 0) {
        TextDrawGlobals *text_drawing_globals = text_get_drawing_globals();
        int start_x = text_drawing_globals->tabs[tabs - 1];
        new_fmt->x = x + start_x;
        if(new_fmt->width > width) {
            new_fmt->width = width;
        }
    }

    new_fmt->y = y;
    new_fmt->height = height;
    new_fmt->justification = justification;
}

void vector_font_handle_unicode_string_formatting(const wchar_t *text, int16_t x, int16_t y, int16_t width, int16_t height, DynamicArray *text_recs) {
    dynamic_array_init(text_recs, sizeof(VectorFontTextRect));
    
    size_t size = wcslen(text);
    if(size == 0) {
        return;
    }

    const TextDrawGlobals *text_drawing_globals = text_get_drawing_globals();
    const uint16_t font_height = font_get_height(text_drawing_globals->font);
    
    size_t start = 0;
    size_t tabs = 0;
    TextJustification justification = text_drawing_globals->justification;
    int16_t offset_x = x + text_drawing_globals->first_line_indentation;

    for(size_t i = 0; i < size - 1; i++) {
        bool break_it_up = false;
        bool tabbed = text[i] == L'\t';

        if(text[i] == L'|' || tabbed) {
            wchar_t control_char = text[i + 1];
            switch(control_char) {
                case L'n':
                case L'r':
                case L'l':
                case L'c':
                case L't':
                    break_it_up = true;
                    break;
            }

            if(tabbed) {
                control_char = 't';
                break_it_up = true;
            }

            if(break_it_up) {
                add_new_unicode_string_rect(text_recs, text, tabs, start, i, offset_x, y, width, height, justification);
                switch(control_char) {
                    case L'n':
                        tabs = 0;
                        y += font_height;
                        height -= font_height;
                        justification = text_drawing_globals->justification;
                        offset_x = x;
                        break;
                    case L'r':
                        tabs = 0;
                        justification = TEXT_JUSTIFICATION_LEFT;
                        break;
                    case L'l':
                        tabs = 0;
                        justification = TEXT_JUSTIFICATION_LEFT;
                        break;
                    case L'c':
                        tabs = 0;
                        justification = TEXT_JUSTIFICATION_CENTER;
                        break;
                    case L't':
                        justification = TEXT_JUSTIFICATION_LEFT;
                        if(tabs < text_drawing_globals->tabs_count) {
                            tabs++;
                        }
                        break;
                }

                if(text[i] == L'|') {
                    i += 1;
                }
                start = i + 1;
            }
        }
    }

    add_new_unicode_string_rect(text_recs, text, tabs, start, size, offset_x, y, width, height, justification);
}

static void add_new_string_rect(DynamicArray *text_recs, const char *str, size_t tabs, int16_t start, int16_t end, int16_t x, int16_t y, int16_t width, int16_t height, TextJustification justification) {
    int16_t substr_size = end - start;
    if(substr_size == 0) {
        return;
    }

    VectorFontTextRect *new_fmt = dynamic_array_push(text_recs, NULL);
    
    dynamic_buffer_init(&new_fmt->text);
    dynamic_buffer_push_data(&new_fmt->text, str + start, substr_size * sizeof(char));
    dynamic_buffer_push_data(&new_fmt->text, L"\0", sizeof(char));

    new_fmt->x = x;
    new_fmt->width = width;
    if(tabs > 0) {
        TextDrawGlobals *text_drawing_globals = text_get_drawing_globals();
        int start_x = text_drawing_globals->tabs[tabs - 1];
        new_fmt->x = x + start_x;
        if(new_fmt->width > width) {
            new_fmt->width = width;
        }
    }

    new_fmt->y = y;
    new_fmt->height = height;
    new_fmt->justification = justification;
}

void vector_font_handle_string_formatting(const char *text, int16_t x, int16_t y, int16_t width, int16_t height, DynamicArray *text_recs) {
    dynamic_array_init(text_recs, sizeof(VectorFontTextRect));
    
    size_t size = strlen(text);
    if(size == 0) {
        return;
    }

    const TextDrawGlobals *text_drawing_globals = text_get_drawing_globals();
    const uint16_t font_height = font_get_height(text_drawing_globals->font);
    
    size_t start = 0;
    size_t tabs = 0;
    TextJustification justification = text_drawing_globals->justification;
    int16_t offset_x = x + text_drawing_globals->first_line_indentation;

    for(size_t i = 0; i < size - 1; i++) {
        bool break_it_up = false;
        bool tabbed = text[i] == '\t';

        if(text[i] == '|' || tabbed) {
            wchar_t control_char = text[i + 1];
            switch(control_char) {
                case 'n':
                case 'r':
                case 'l':
                case 'c':
                case 't':
                    break_it_up = true;
                    break;
            }

            if(tabbed) {
                control_char = 't';
                break_it_up = true;
            }

            if(break_it_up) {
                add_new_string_rect(text_recs, text, tabs, start, i, offset_x, y, width, height, justification);
                switch(control_char) {
                    case 'n':
                        tabs = 0;
                        y += font_height;
                        height -= font_height;
                        justification = text_drawing_globals->justification;
                        offset_x = x;
                        break;
                    case 'r':
                        tabs = 0;
                        justification = TEXT_JUSTIFICATION_LEFT;
                        break;
                    case 'l':
                        tabs = 0;
                        justification = TEXT_JUSTIFICATION_LEFT;
                        break;
                    case 'c':
                        tabs = 0;
                        justification = TEXT_JUSTIFICATION_CENTER;
                        break;
                    case 't':
                        justification = TEXT_JUSTIFICATION_LEFT;
                        if(tabs < text_drawing_globals->tabs_count) {
                            tabs++;
                        }
                        break;
                }

                if(text[i] == '|') {
                    i += 1;
                }
                start = i + 1;
            }
        }
    }

    add_new_string_rect(text_recs, text, tabs, start, size, offset_x, y, width, height, justification);
}

void vector_font_calculate_unicode_string_draw_bounds(const wchar_t *string, const Rectangle2D *position, Rectangle2D *first_character_position, Rectangle2D *text_bounds) {
    TextDrawGlobals *text_globals = text_get_drawing_globals();
    VectorFont *font = tag_get_data(TAG_GROUP_VECTOR_FONT, text_globals->font);
    
    Rectangle2D bounds;
    rasterizer_vector_font_calculate_unicode_string_draw_bounds(string, font, text_globals->style, &bounds);

    switch(text_globals->justification) {
        case TEXT_JUSTIFICATION_LEFT:
            break;
        case TEXT_JUSTIFICATION_RIGHT: {
            uint16_t width = bounds.right - bounds.left;
            bounds.left = position->right - width;
            bounds.right = position->right;
            break;
        }
        case TEXT_JUSTIFICATION_CENTER: {
            uint16_t width = bounds.right - bounds.left;
            int16_t center = (position->right - position->left) / 2;
            bounds.left = center - (width / 2);
            bounds.right = center + (width / 2);
            break;
        }
    }

    bounds.left += position->left;
    bounds.top += position->top;
    bounds.right += position->left;
    bounds.bottom += position->top;

    if(first_character_position) {
        first_character_position->left = bounds.right;
        first_character_position->right = bounds.right + 1;
        first_character_position->top = bounds.top;
        first_character_position->bottom = bounds.bottom;
    }
    
    if(text_bounds) {
        *text_bounds = bounds; 
    }
}

uint32_t vector_font_calculate_unicode_string_width(wchar_t *string, VectorFont *font, FontStyle style) {
    Rectangle2D bounds;
    rasterizer_vector_font_calculate_unicode_string_draw_bounds(string, font, style, &bounds);
    return bounds.right - bounds.left;
}

bool vector_font_glyph_exists(VectorFont *font, FontStyle font_style, char character) {
    VectorFontStyle *font_style_data = vector_font_get_style(font, font_style);
    VectorFontData *font_data = tag_get_data(TAG_GROUP_VECTOR_FONT_DATA, font_style_data->data.tag_handle);
    
    LOGFONTA lf = {0};
    lf.lfHeight = -12; 
    strncpy(lf.lfFaceName, font_data->font_family_name.string, LF_FACESIZE - 1);

    HFONT hFont = CreateFontIndirectA(&lf);
    if(!hFont) {
        return false;
    }

    HDC hdc = GetDC(NULL);
    if(!hdc) {
        DeleteObject(hFont);
        return false;
    }

    HGDIOBJ oldFont = SelectObject(hdc, hFont);
    WORD glyphIndex = 0xFFFF;
    GetGlyphIndicesA(hdc, &character, 1, &glyphIndex, GGI_MARK_NONEXISTING_GLYPHS);

    SelectObject(hdc, oldFont);
    ReleaseDC(NULL, hdc);
    DeleteObject(hFont);

    return glyphIndex != 0xFFFF;
}
