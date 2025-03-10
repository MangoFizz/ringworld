#include <stdio.h>
#include "../bitmap/bitmap.h"
#include "../exception/exception.h"
#include "../game/game_engine.h"
#include "../main/main_globals.h"
#include "../math/color.h"
#include "../multiplayer/multiplayer_game_text.h"
#include "../network/network_game.h"
#include "../rasterizer/rasterizer_text.h"
#include "../render/render.h"
#include "../text/font.h"
#include "../text/text.h"
#include "hud.h"
#include "scoreboard.h"

extern float *pgcr_footer_fade;

const ColorRGB pgcr_scoreboard_header_color = { 0.4588235f, 0.7294118f, 1.0f }; // blue
const ColorRGB pgcr_scoreboard_local_player_place_color = { 1.0f, 1.0f, 0.0f }; // yellow

void pgcr_set_scoreboard_tabs(void) {
    uint16_t tabs[] = { 6, 50, 125, 250, 350, 410, 500 };
    if(render_widescreen_support_enabled()) {
        uint16_t canvas_width = render_get_screen_width();
        float widescreen_margin = (float)(canvas_width - RASTERIZER_SCREEN_BASE_WIDTH) / 2.0f;
        for(size_t i = 1; i < sizeof(tabs) / sizeof(tabs[0]); i++) {
            tabs[i] += widescreen_margin;
        }
    }
    text_set_tab_stops(tabs, 7);
}

void pgcr_set_teams_scores_tabs(void) {
    uint16_t tabs[] = { 6, 50, 200 };
    if(render_widescreen_support_enabled()) {
        uint16_t canvas_width = render_get_screen_width();
        float widescreen_margin = (float)(canvas_width - RASTERIZER_SCREEN_BASE_WIDTH) / 2.0f;
        for(size_t i = 1; i < sizeof(tabs) / sizeof(tabs[0]); i++) {
            tabs[i] += widescreen_margin;
        }
    }
    text_set_tab_stops(tabs, 3);
}

void pgcr_draw_row_text(wchar_t *string, uint32_t justification, int16_t row) {
    TextDrawGlobals *text_globals = text_get_drawing_globals();
    uint16_t canvas_width = render_get_screen_width();
    uint16_t canvas_height = render_get_screen_height();
    HUDGlobals *hud_globals = hud_get_globals();

    TagHandle font = hud_globals->messaging_parameters.fullscreen_font.tag_handle;
    if(HANDLE_IS_NULL(font)) {
        font = font_get_default_large();
    }
    
    Rectangle2D text_bounds;
    text_bounds.left = 0;
    text_bounds.right = canvas_width;
    text_bounds.top = row * 18;
    text_bounds.bottom = text_bounds.top + 26;

    if(render_widescreen_support_enabled()) {
        float widescreen_margin = (float)(canvas_width - RASTERIZER_SCREEN_BASE_WIDTH) / 2.0f;
        math_rectangle_2d_translate(&text_bounds, widescreen_margin, 0);
    }

    text_globals->justification = justification;
    text_globals->flags = 0;
    text_globals->style = -1;
    text_globals->font = font;

    rasterizer_draw_unicode_string(&text_bounds, NULL, NULL, 0, string);
}

void pgcr_set_text_color(const ColorRGB *color) {
    TextDrawGlobals *text_globals = text_get_drawing_globals();
    text_globals->color.r = color->r;
    text_globals->color.g = color->g;
    text_globals->color.b = color->b;
    text_globals->color.a = 1.0f;
} 

void pgcr_draw_background(void) {
    uint16_t screen_width = render_get_screen_width();
    uint16_t screen_height = render_get_screen_height();
    float widescreen_margin = ((float)screen_width - RASTERIZER_SCREEN_BASE_WIDTH) / 2.0f;
    HUDGlobals *hud_globals = hud_get_globals();

    TagHandle background_tag_handle = hud_globals->more_hud_crap.carnage_report_bitmap.tag_handle;
    if(!HANDLE_IS_NULL(background_tag_handle)) {
        BitmapData *background = bitmap_group_sequence_get_bitmap_for_frame(background_tag_handle, 0, 0);
        Rectangle2D background_rect;
        background_rect.left = 0;
        background_rect.right = screen_width;
        background_rect.top = 0;
        background_rect.bottom = screen_height;
        Rectangle2D texture_rect = background_rect;
        texture_rect.right = RASTERIZER_SCREEN_BASE_WIDTH;
        bitmap_draw_in_rect(background, NULL, 0xFFFFFFFF, &background_rect, &texture_rect);
    }
}

void pgcr_draw_scoreboard(void) {
    GameEngineInterface *game_engine = game_engine_get_current_interface();
    if(game_engine == NULL) {
        return;
    }

    wchar_t score_column_text[256];
    wchar_t row_text[256];

    pgcr_set_scoreboard_tabs();

    const wchar_t *place_label = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_PLACE_COLUMN_HEADER);
    const wchar_t *name_label = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_NAME_COLUMN_HEADER);
    const wchar_t *kills_label = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_KILLS_COLUMN_HEADER);
    const wchar_t *assists_label = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_ASSISTS_COLUMN_HEADER);
    const wchar_t *deaths_label = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_DEATHS_COLUMN_HEADER);
    game_engine->get_score_header_string(score_column_text);
    swprintf_s(row_text, 512, L"\t%s\t%s\t%s\t%s\t%s\t%s", place_label, name_label, score_column_text, kills_label, assists_label, deaths_label);
    pgcr_set_text_color(&pgcr_scoreboard_header_color);
    pgcr_draw_row_text(row_text, 0, 7);

    ScoreboardRowData rows_data[NETWORK_GAME_MAX_PLAYERS];
    scoreboard_build_rows(NULL_HANDLE, rows_data, NETWORK_GAME_MAX_PLAYERS, false, false);

    for(size_t i = 0; i < NETWORK_GAME_MAX_PLAYERS; i++) {
        ScoreboardRowData *row_data = &rows_data[i];
        if(HANDLE_IS_NULL(row_data->player_handle)) {
            continue;
        }

        Player *player = table_get_element(player_get_table(), row_data->player_handle);
        ASSERT(player != NULL);
        
        const wchar_t *place_text = multiplayer_game_text_get_place_string(row_data->place);
        const ColorRGB *place_color = &pgcr_scoreboard_header_color;
        if(player->local_player_index != -1) {
            place_color = &pgcr_scoreboard_local_player_place_color;
        }
        swprintf_s(row_text, 256, L"\t%s", place_text);
        pgcr_set_text_color(place_color);
        pgcr_draw_row_text(row_text, 0, i + 8);
        
        const wchar_t *score_string = game_engine->get_score_string(row_data->player_handle.index, score_column_text);
        swprintf_s(row_text, 256, L"\t\t%s\t%s\t%d\t%d\t%d", row_data->name, score_string, row_data->kills, row_data->assists, row_data->deaths);
        pgcr_set_text_color(&pgcr_scoreboard_header_color);
        pgcr_draw_row_text(row_text, 0, i + 8);
    }

    text_reset_tab_stops();
}

void pgcr_draw_teams_scores(void) {
    NetworkGame *network_game = network_game_get();
    GameEngineInterface *game_engine = game_engine_get_current_interface();
    if(network_game->gametype_variant.universal_variant.teams) {
        wchar_t team_score[32];
        wchar_t team_score_text[64];
        
        pgcr_set_text_color(&pgcr_scoreboard_header_color);
        pgcr_set_teams_scores_tabs();
        
        const wchar_t *red_team_score = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_RED_TEAM_SCORE);
        swprintf_s(team_score_text, 64, red_team_score, game_engine->get_team_score_string(NETWORK_GAME_TEAM_RED, team_score));
        pgcr_draw_row_text(team_score_text, 0, 4);
        
        const wchar_t *blue_team_score = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_BLUE_TEAM_SCORE);
        swprintf_s(team_score_text, 64, blue_team_score, game_engine->get_team_score_string(NETWORK_GAME_TEAM_BLUE, team_score));
        pgcr_draw_row_text(team_score_text, 0, 5);

        text_reset_tab_stops();
    }
}

void pgcr_draw_footer(void) {
    MainGlobals *main_globals = main_get_globals();
    HUDGlobals *hud_globals = hud_get_globals();
    float widescreen_margin = ((float)render_get_screen_width() - RASTERIZER_SCREEN_BASE_WIDTH) / 2.0f;

    Rectangle2D rect;
    rect.left = widescreen_margin;
    rect.right = render_get_screen_width();
    rect.top = 410;
    rect.bottom = render_get_screen_height();

    const wchar_t *text;
    if(main_globals->game_connection == GAME_CONNECTION_NETWORK_SERVER) {
        rect.left += 280;
        text = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_QUIT_CONTINUE);
    }
    else {
        rect.left += 420;
        text = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_QUIT);
    }

    ColorARGB color;
    color.a = clamp_f32(*pgcr_footer_fade, 0.0f, 1.0f);
    color.r = pgcr_scoreboard_header_color.r;
    color.g = pgcr_scoreboard_header_color.g;
    color.b = pgcr_scoreboard_header_color.b;

    TagHandle font = hud_globals->messaging_parameters.fullscreen_font.tag_handle;
    if(HANDLE_IS_NULL(font)) {
        font = font_get_default_large();
    }
    
    text_set_drawing_parameters(-1, 0, 0, font, &color);
    rasterizer_draw_unicode_string_and_hack_in_icons(&rect, false, text);
}

void pgcr_draw(void) {
    pgcr_draw_background();
    pgcr_draw_teams_scores();
    pgcr_draw_scoreboard();
    pgcr_draw_footer();
}
