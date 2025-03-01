#include <stdio.h>
#include "../interface/hud.h"
#include "../interface/ui_widget.h"
#include "../game/game_time.h"
#include "../math/math.h"
#include "../math/color.h"
#include "../rasterizer/rasterizer_text.h"
#include "../render/render.h"
#include "../scenario/scenario.h"
#include "../tag/tag.h"
#include "../text/text.h"
#include "../text/unicode_string_list.h"
#include "cutscene.h"

extern CinematicGlobals **cinematic_globals;

CinematicGlobals *cutscene_get_globals(void) {
    return *cinematic_globals;
}

void cutscene_cinematic_render(void) {
    CinematicGlobals *cinematic_globals = cutscene_get_globals();
    HUDGlobals *hud_globals = hud_get_globals();
    WidgetGlobals *widget_globals = ui_widget_get_globals();
    GameTimeGlobals *game_time_globals = game_time_get_globals();
    RenderGlobals *render_globals = render_get_globals();
    Scenario *current_scenario = scenario_get();

    bool showing_letter_box = cinematic_globals->showing_letter_box;
    if(showing_letter_box || cinematic_globals->letter_box_size > 0) {
        bool draw_letter_box = true;
        if(widget_globals->initialized) {
            for(size_t i = 0; i < sizeof(widget_globals->active_widget) / sizeof(widget_globals->active_widget[0]); i++) {
                if(widget_globals->active_widget[i] != NULL) {
                    draw_letter_box = false;
                    break;
                }
            }
        }
        if(draw_letter_box) {
            uint32_t local_time = game_time_globals->local_time;
            uint32_t letter_box_last_game_time = cinematic_globals->letter_box_last_game_time;
            cinematic_globals->letter_box_last_game_time = local_time;
            float letter_box_transition_progress = (float)(local_time - letter_box_last_game_time) * 0.03333334;
            if(showing_letter_box) {
                letter_box_transition_progress = letter_box_transition_progress + cinematic_globals->letter_box_size;
                cinematic_globals->letter_box_size = letter_box_transition_progress;
                if(letter_box_transition_progress > 1.0f) {
                    letter_box_transition_progress = 1.0f;
                }
            }
            else {
                letter_box_transition_progress = cinematic_globals->letter_box_size - letter_box_transition_progress;
                cinematic_globals->letter_box_size = letter_box_transition_progress;
                if(letter_box_transition_progress < 0.0f) {
                    letter_box_transition_progress = 0.0f;
                }
            }

            cinematic_globals->letter_box_size = letter_box_transition_progress;
            if(letter_box_transition_progress > 0.0f) {
                uint16_t screen_width = render_get_screen_width();
                uint16_t screen_height = render_get_screen_height();
                float progress = cinematic_globals->letter_box_size * 0.125f * screen_height;
                Rectangle2D letter_box_rect;
                // @todo fix camera viewport bounds
                letter_box_rect.left = 0;
                letter_box_rect.right = screen_width;
                letter_box_rect.top = render_globals->camera.viewport_bounds.top;
                letter_box_rect.bottom = render_globals->camera.viewport_bounds.top + progress;
                rasterizer_screen_geometry_draw_quad(&letter_box_rect, 0xFF000000);
                letter_box_rect.top = screen_height - math_float_to_long(progress);
                letter_box_rect.bottom = screen_height;
                rasterizer_screen_geometry_draw_quad(&letter_box_rect, 0xFF000000);
            }
        }
    }

    for(size_t i = 0; i < sizeof(cinematic_globals->titles) / sizeof(cinematic_globals->titles[0]); i++) {
        uint16_t letter_box_size = cinematic_globals->letter_box_size + 12;
        TagHandle fullscreen_font_tag = hud_globals->messaging_parameters.fullscreen_font.tag_handle;
        if(!HANDLE_IS_NULL(fullscreen_font_tag) && cinematic_globals->titles[i].index != -1) {
            ScenarioCutsceneTitle *title = TAG_BLOCK_GET_ELEMENT(current_scenario->cutscene_titles, cinematic_globals->titles[i].index);
            TagHandle ingame_help_text_tag = current_scenario->ingame_help_text.tag_handle;
            if(!HANDLE_IS_NULL(ingame_help_text_tag)) {
                UnicodeStringList *ingame_help_text = tag_get_data(TAG_GROUP_UNICODE_STRING_LIST, ingame_help_text_tag);
                if(title->string_index != -1 && title->string_index < ingame_help_text->strings.count) {
                    Rectangle2D title_pos = title->text_bounds;
                    if(title_pos.left == title_pos.right || title_pos.top == title_pos.bottom) {
                        title_pos = hud_globals->hud_crap.default_chapter_title_bounds;
                    }

                    if(render_widescreen_support_enabled()) {
                        uint16_t screen_width = render_get_screen_width();
                        float margin = (float)(screen_width - RASTERIZER_SCREEN_BASE_WIDTH) / 2;
                        switch(title->justification) {
                            case SCENARIO_JUSTIFICATION_CENTER:
                                math_rectangle_2d_translate(&title_pos, margin, 0);
                                break;
                            case SCENARIO_JUSTIFICATION_RIGHT:
                                math_rectangle_2d_translate(&title_pos, margin * 2, 0);
                                break;
                        }
                    }
                    
                    float title_timer = cinematic_globals->titles[i].timer;
                    float text_fade = 1.0f;
                    if(nan_f32(title->fade_in_time) || title_timer > title->fade_in_time) {
                        if(title_timer > title->up_time) {
                            text_fade = 1.0f - (title_timer - title->up_time) / title->fade_out_time;
                        }
                    }
                    else {
                        text_fade = title_timer / title->fade_in_time;
                    }
                    text_fade = clamp_f32(text_fade, 0.0f, 1.0f);

                    ColorARGB text_color;
                    color_decode_a8r8g8b8(title->text_color, &text_color);
                    text_color.a = text_color.a * text_fade;
                    if(nan_f32(text_color.r) || abs(text_color.r - 1.0f) < 0.0001) {
                        text_color.r = 1.0f;
                    }
                    if(nan_f32(text_color.g) || abs(text_color.g - 1.0f) < 0.0001) {
                        text_color.g = 1.0f;
                    }
                    if(nan_f32(text_color.b) || abs(text_color.b - 1.0f) < 0.0001) {
                        text_color.b = 1.0f;
                    }

                    text_set_drawing_parameters(title->text_style, title->justification, *(uint32_t *)(&title->text_flags), fullscreen_font_tag, &text_color);

                    ColorARGB text_shadow_color;
                    color_decode_a8r8g8b8(title->shadow_color, &text_shadow_color);
                    text_shadow_color.a = text_shadow_color.a * text_fade;
                    text_shadow_color.a = clamp_f32(text_shadow_color.a, 0.0f, 1.0f);
                    text_set_shadow_color(color_encode_a8r8g8b8(&text_shadow_color));

                    const wchar_t *title_text = unicode_string_list_get_string(ingame_help_text_tag, title->string_index);
                    rasterizer_draw_unicode_string(&title_pos, &title_pos, 0, 0, title_text);
                    text_set_shadow_color(0);

                    uint16_t time_elapsed = 0;
                    if(!game_time_globals->paused) {
                        time_elapsed = game_time_globals->elapsed;
                    }
                    cinematic_globals->titles[i].timer += time_elapsed;

                    float title_time_elapsed = title->up_time + title->fade_out_time;
                    if(cinematic_globals->titles[i].timer >= title_time_elapsed) {
                        cinematic_globals->titles[i].timer = -1;
                        cinematic_globals->titles[i].index = -1;
                    }
                }
            }
        }
    }
}
