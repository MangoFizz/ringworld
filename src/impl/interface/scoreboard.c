#include <stdio.h>
#include "../bitmap/bitmap.h"
#include "../exception/exception.h"
#include "../game/game_engine.h"
#include "../game/game_globals.h"
#include "../main/main_globals.h"
#include "../time/time.h"
#include "../console/console.h"
#include "../interface/hud.h"
#include "../network/network_game.h"
#include "../math/math.h"
#include "../math/color.h"
#include "../multiplayer/multiplayer_game_text.h"
#include "../player/player.h"
#include "../text/text.h"
#include "../font/font.h"
#include "../render/render.h"
#include "scoreboard.h"

void scoreboard_draw_row(wchar_t *text, bool highlight, ColorARGB *color, int16_t row_index) {
    const HUDGlobals *hud_globals = hud_get_globals();

    TagHandle font = hud_globals->messaging_parameters.splitscreen_font.tag_handle;
    if(HANDLE_IS_NULL(font)) {
        font = font_get_default_small();
    }

    uint16_t screen_width = render_get_screen_width();
    float widescreen_margin = ((float)screen_width - RASTERIZER_SCREEN_BASE_WIDTH) / 2.0f;

    if(row_index != 0) {
        uint16_t tabs[8];
        tabs[0] = 7;
        tabs[1] = 25 + widescreen_margin;
        tabs[2] = 90 + widescreen_margin;
        tabs[3] = 300 + widescreen_margin;
        tabs[4] = 365 + widescreen_margin;
        tabs[5] = 430 + widescreen_margin;
        tabs[6] = 495 + widescreen_margin;
        tabs[7] = 560 + widescreen_margin;
        text_set_tab_stops(tabs, 8);
    }
    else {
        text_reset_tab_stops();
    }

    uint16_t font_height = font_get_height(font);

    Rectangle2D rect;
    rect.left = 10;
    rect.right = RASTERIZER_SCREEN_BASE_WIDTH;
    rect.top = 60;
    rect.bottom = rect.top + font_height;
    math_rectangle_2d_translate(&rect, widescreen_margin, row_index * font_height + 3);

    ColorARGB text_color = *color;
    if(highlight) {
        text_color.r = min_f32(text_color.r + 0.4f, 1.0f);
        text_color.g = min_f32(text_color.g + 0.4f, 1.0f);
        text_color.b = min_f32(text_color.b + 0.4f, 1.0f);
    }

    text_set_drawing_parameters(-1, 0, 0, font, &text_color);
    text_draw_unicode_string(&rect, NULL, NULL, 0, text);
    text_reset_tab_stops();
}

typedef struct PlayerScore {
    PlayerHandle player_handle;
    int16_t team_index;
    int16_t score;
    int16_t place;
    bool tied;
} PlayerScore;

int scoreboard_players_sort_compare_scores(const void *a, const void *b) {
    const PlayerScore *player_a = a;
    const PlayerScore *player_b = b;
    if(player_a->score > player_b->score) {
        return -1;
    }
    else if(player_a->score < player_b->score) {
        return 1;
    }
    return 0;
}

void scoreboard_sort_players(PlayerScore *scores, uint16_t *count, PlayerHandle local_player_handle, PlayerScore **local_player_score, bool network_players_only) {
    PlayersTable *players_table = player_get_table();
    GameEngineInterface *game_type_engine = game_engine_get_current_interface();
    NetworkGame *network_game = network_game_get();

    PlayerScore *current_player_score = scores;
    size_t players_count = 0;

    if(network_players_only) {
        for(size_t i = 0; i < NETWORK_GAME_MAX_PLAYERS; i++) {
            NetworkGamePlayer *netgame_player = &network_game->players[i];
            if(netgame_player->player_list_index != -1) {
                Player *player = table_get_element(players_table, MAKE_HANDLE(0, netgame_player->player_list_index));
                if(player) {
                    PlayerHandle player_handle = MAKE_HANDLE(player->player_id, netgame_player->player_list_index);
                    current_player_score->player_handle = player_handle;
                    current_player_score->score = game_type_engine->get_score(i, false);
                    if(network_game->gametype_variant.universal_variant.teams) {
                        current_player_score->team_index = player->team_index;
                    }
                    else {
                        current_player_score->team_index = 0;
                    }
                    if(!HANDLE_IS_NULL(local_player_handle) && local_player_handle.value == player_handle.value) {
                        *local_player_score = current_player_score;
                    }
                    current_player_score++;
                    players_count++;
                }
            }
        }
    }
    else {
        for(size_t i = 0; i < NETWORK_GAME_MAX_PLAYERS; i++) {
            Player *player = table_get_element(players_table, MAKE_HANDLE(0, i));
            if(player) {
                PlayerHandle player_handle = MAKE_HANDLE(player->player_id, i);
                current_player_score->player_handle = player_handle;
                current_player_score->score = game_type_engine->get_score(i, false);
                if(network_game->gametype_variant.universal_variant.teams) {
                    current_player_score->team_index = player->team_index;
                }
                else {
                    current_player_score->team_index = 0;
                }
                if(!HANDLE_IS_NULL(local_player_handle) && local_player_handle.value == player_handle.value) {
                    *local_player_score = current_player_score;
                }
                current_player_score++;
                players_count++;
            }
        }
    }

    // Sort by score
    qsort(scores, players_count, sizeof(PlayerScore), scoreboard_players_sort_compare_scores);
    
    // Assign places
    PlayerScore *previous_player_score = NULL;
    for(size_t i = 0; i < players_count; i++) {
        PlayerScore *player_score = &scores[i];
        if(previous_player_score == NULL) {
            player_score->place = 1;
        }
        else if(player_score->score != previous_player_score->score) {
            player_score->place = previous_player_score->place + 1;
        }
        else {
            player_score->place = previous_player_score->place;
            player_score->tied = true;
            previous_player_score->tied = true;
        }
        previous_player_score = player_score;
    }

    *count = players_count;
}

void scoreboard_push_rows_data_by_team(PlayerScore *players_scores, size_t players_scores_count, uint16_t team_index, ScoreboardRowData *scoreboard_rows, size_t *rows_count)  {
    PlayersTable *players_table = player_get_table();
    NetworkGame *network_game = network_game_get();

    for(size_t player_score_index = 0; player_score_index < players_scores_count; player_score_index++) {
        PlayerScore *player_score = &players_scores[player_score_index];
        if(player_score->team_index == team_index) {
            ScoreboardRowData *scoreboard_row = &scoreboard_rows[*rows_count];
            PlayerHandle player_handle = player_score->player_handle;
            Player *player = table_get_element(players_table, player_handle);
            ASSERT(player != NULL);
            scoreboard_row->player_handle = player_handle;
            scoreboard_row->name = player->name;
            scoreboard_row->kills = player->kills[0];
            scoreboard_row->deaths = player->deaths;
            scoreboard_row->assists = player->assists[0];
            scoreboard_row->ping = player->ping;
            scoreboard_row->team = player_score->team_index;
            scoreboard_row->place = player_score->place;
            scoreboard_row->lives = max_i32(network_game->gametype_variant.universal_variant.lives - player->deaths, 0);
            (*rows_count)++;
        }
    }
}

void scoreboard_build_rows(PlayerHandle player_handle, ScoreboardRowData *scoreboard_rows, uint16_t count, bool player_team_first, bool network_players_only) {
    PlayerScore players_scores[NETWORK_GAME_MAX_PLAYERS];
    PlayerScore *local_player_score = NULL;
    uint16_t players_scores_count = 0;
    scoreboard_sort_players(players_scores, &players_scores_count, player_handle, &local_player_score, network_players_only);

    memset(scoreboard_rows, 0, sizeof(ScoreboardRowData) * count);
    for(size_t i = 0; i < count; i++) {
        scoreboard_rows[i].player_handle = NULL_HANDLE;
    }

    int8_t local_player_team = -1;
    size_t scoreboard_row_index = 0;
    
    if(player_team_first) {
        ASSERT(local_player_score != NULL);
        local_player_team = local_player_score->team_index;
        scoreboard_push_rows_data_by_team(players_scores, players_scores_count, local_player_team, scoreboard_rows, &scoreboard_row_index);
    }
    
    NetworkGame *network_game = network_game_get();
    uint8_t teams_count = network_game->gametype_variant.universal_variant.teams ? NUM_OF_NETWORK_GAME_TEAMS : 1;
    for(size_t team_index = 0; team_index < teams_count; team_index++) {
        if(!player_team_first || team_index != local_player_team) {
            scoreboard_push_rows_data_by_team(players_scores, players_scores_count, team_index, scoreboard_rows, &scoreboard_row_index);
        }
    }
}

void scoreboard_draw_header(ScoreboardRowData *player_score_data, float fade) {
    const HUDGlobals *hud_globals = hud_get_globals();
    NetworkGame *network_game = network_game_get();
    GameEngineInterface *game_engine = game_engine_get_current_interface();
    GameEngineGlobals *game_engine_globals = game_engine_get_globals();

    ColorARGB header_color;
    header_color.a = fade;
    header_color.r = 1.0f;
    header_color.g = 1.0f;
    header_color.b = 1.0f;

    TagHandle server_name_font = hud_globals->messaging_parameters.splitscreen_font.tag_handle;
    if(HANDLE_IS_NULL(server_name_font)) {
        server_name_font = font_get_default_small();
    }
    
    wchar_t lives_text[32];
    if(network_game->gametype_variant.universal_variant.lives > 0) {
        uint16_t text_index;
        if(player_score_data->lives == 0) {
            swprintf_s(lives_text, 32, multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_HEADER_NO_LIVES));
        }
        else if(player_score_data->lives == 1) {
            swprintf_s(lives_text, 32, multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_HEADER_ONE_LIFE));
        }
        else {
            swprintf_s(lives_text, 32, multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_HEADER_LIVES), player_score_data->lives);
        }
    }
    else {
        lives_text[0] = L'\0';
    }

    wchar_t text[256];

    if(game_engine_globals->mode == GAME_ENGINE_MODE_ACTIVE) {
        if(network_game->gametype_variant.universal_variant.teams) {
            int16_t red_team_score = game_engine->get_team_score(NETWORK_GAME_TEAM_RED);
            int16_t blue_team_score = game_engine->get_team_score(NETWORK_GAME_TEAM_BLUE);
            
            wchar_t red_team_score_text[32];
            swprintf_s(red_team_score_text, 32, L"%d", red_team_score);
            
            wchar_t blue_team_score_text[32];
            swprintf_s(blue_team_score_text, 32, L"%d", blue_team_score);
    
            if(red_team_score == blue_team_score) {
                swprintf_s(text, 256, multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_HEADER_TEAMS_TIED), red_team_score_text, lives_text);
            }
            else if(red_team_score > blue_team_score) {
                swprintf_s(text, 256, multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_HEADER_RED_LEADS_BLUE), red_team_score_text, blue_team_score_text, lives_text);
            }
            else {
                swprintf_s(text, 256, multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_HEADER_BLUE_LEADS_RED), blue_team_score_text, red_team_score_text, lives_text);
            }
        }
        else {
            wchar_t player_score_text[32];
            swprintf_s(player_score_text, 32, L"%d", game_engine->get_score(player_score_data->player_handle.index, false));
    
            if(player_score_data->tied) {
                swprintf_s(text, 256, multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_HEADER_TIED_FOR_PLACE), player_score_text, lives_text);
            }
            else {
                const wchar_t *place_text = multiplayer_game_text_get_place_string(player_score_data->place);
                swprintf_s(text, 256, multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_HEADER_IN_PLACE), place_text, player_score_text, lives_text);
            }
        }
    }
    else {
        if(network_game->gametype_variant.universal_variant.teams) {
            int16_t red_team_score = game_engine->get_team_score(NETWORK_GAME_TEAM_RED);
            int16_t blue_team_score = game_engine->get_team_score(NETWORK_GAME_TEAM_BLUE);
            uint16_t text_index;
            if(red_team_score == blue_team_score) {
                text_index = MULTIPLAYER_GAME_TEXT_HEADER_DRAW;
            }
            else if((red_team_score > blue_team_score) == (player_score_data->team == NETWORK_GAME_TEAM_RED)) {
                text_index = MULTIPLAYER_GAME_TEXT_HEADER_TEAM_WON;
            }
            else {
                text_index = MULTIPLAYER_GAME_TEXT_HEADER_TEAM_LOST;
            }
            swprintf_s(text, 256, multiplayer_game_text_get_string(text_index));
        }
        else {
            uint16_t text_index;
            if(player_score_data->place == 1) {
                if(player_score_data->tied) {
                    text_index = MULTIPLAYER_GAME_TEXT_HEADER_DRAW;
                }
                else {
                    text_index = MULTIPLAYER_GAME_TEXT_HEADER_YOU_WON;
                }
            } 
            else {
                text_index = MULTIPLAYER_GAME_TEXT_HEADER_YOU_LOST;
            }
            swprintf_s(text, 256, multiplayer_game_text_get_string(text_index));
        }
    }

    scoreboard_draw_row(text, false, &header_color, 0);
}

void scoreboard_draw_table(PlayerHandle player_handle, float fade) {
    GameEngineInterface *game_type_engine = game_engine_get_current_interface();
    PlayersTable *players_table = player_get_table();
    NetworkGame *network_game = network_game_get();

    ScoreboardRowData rows_data[NETWORK_GAME_MAX_PLAYERS];
    scoreboard_build_rows(player_handle, rows_data, NETWORK_GAME_MAX_PLAYERS, true, true);

    const wchar_t *place_label = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_PLACE_COLUMN_HEADER);
    const wchar_t *name_label = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_NAME_COLUMN_HEADER);
    const wchar_t *kills_label = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_KILLS_COLUMN_HEADER);
    const wchar_t *assists_label = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_ASSISTS_COLUMN_HEADER);
    const wchar_t *deaths_label = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_DEATHS_COLUMN_HEADER);
    const wchar_t *ping_label = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_PING_COLUMN_HEADER);
    
    wchar_t score_label[256];
    game_type_engine->get_score_header_string(score_label);

    wchar_t row_text[256];
    swprintf_s(row_text, 256, L"\t%s\t%s\t%s\t%s\t%s\t%s\t%s", place_label, name_label, score_label, kills_label, assists_label, deaths_label, ping_label);

    ColorARGB text_color;
    text_color.a = fade;
    text_color.r = 0.5f;
    text_color.g = 0.5f;
    text_color.b = 0.5f;
    scoreboard_draw_row(row_text, false, &text_color, 1);

    for(size_t i = 0; i < NETWORK_GAME_MAX_PLAYERS; i++) {
        ScoreboardRowData *row_data = &rows_data[i];
        if(!HANDLE_IS_NULL(row_data->player_handle)) {
            Player *player = table_get_element(players_table, row_data->player_handle);
            ASSERT(player != NULL);

            const wchar_t *place_text = multiplayer_game_text_get_place_string(row_data->place);
            const wchar_t *score_string = NULL;
            if(network_game->gametype_variant.universal_variant.lives > 0 && row_data->lives == 0) {
                score_string = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_SCORE_DEAD); 
            }
            if(player->quit) {
                score_string = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_SCORE_QUIT);
            }
            if(score_string == NULL) {
                score_string = game_type_engine->get_score_string(row_data->player_handle.index, score_label);
            }
            swprintf_s(row_text, 256, L"\t%s\t%s\t%s\t%d\t%d\t%d\t%d", place_text, row_data->name, score_string, row_data->kills, row_data->assists, row_data->deaths, row_data->ping);
            
            ColorARGB row_color;
            row_color.a = fade;
            if(network_game->gametype_variant.universal_variant.teams) {
                const ColorRGB *team_color = network_game_get_team_color(row_data->team);
                row_color.r = team_color->r;
                row_color.g = team_color->g;
                row_color.b = team_color->b;
            }
            else {
                row_color.r = 0.7f;
                row_color.g = 0.7f;
                row_color.b = 0.7f;
            }
            
            bool should_highlight = false;
            if(row_data->player_handle.value == player_handle.value) {
                scoreboard_draw_header(row_data, fade);
                should_highlight = true;
            }
            
            scoreboard_draw_row(row_text, should_highlight, &row_color, i + 2);
        }
    }
}

void scoreboard_draw_background(float fade) {
    uint16_t screen_width = render_get_screen_width();
    float widescreen_margin = ((float)screen_width - RASTERIZER_SCREEN_BASE_WIDTH) / 2.0f;
    
    ColorARGB background_color;
    background_color.a = fade * 0.69f;
    background_color.r = 0.125f;
    background_color.g = 0.125f;
    background_color.b = 0.125f;
    Rectangle2D background_rect;
    background_rect.left = 10;
    background_rect.top = 60;
    background_rect.right = RASTERIZER_SCREEN_BASE_WIDTH - 10;
    background_rect.bottom = RASTERIZER_SCREEN_BASE_HEIGHT - 90;
    math_rectangle_2d_translate(&background_rect, widescreen_margin, 0);
    rasterizer_screen_geometry_draw_quad(&background_rect, color_encode_a8r8g8b8(&background_color));
}

void scoreboard_draw_server_info(float fade) {
    const HUDGlobals *hud_globals = hud_get_globals();
    NetworkGame *network_game = network_game_get();
    uint16_t screen_width = render_get_screen_width();
    uint16_t screen_height = render_get_screen_height();

    TagHandle font = hud_globals->messaging_parameters.fullscreen_font.tag_handle;
    if(HANDLE_IS_NULL(font)) {
        font = hud_globals->messaging_parameters.splitscreen_font.tag_handle;
        if(HANDLE_IS_NULL(font)) {
            font = font_get_default_large();
        }
    }
    ColorARGB color = { fade, 1.0f, 1.0f, 1.0f };
    text_set_drawing_parameters(-1, 1, 0, font, &color);

    uint16_t line_height = font_get_height(font) + 2;

    Rectangle2D rect;
    rect.left = 10;
    rect.right = screen_width - 5;
    rect.bottom = screen_height - 10;
    rect.top = rect.bottom - line_height * 2;
    
    text_draw_unicode_string(&rect, NULL, NULL, 0, network_game->server_name);

    union {
        uint32_t value;
        NetworkTransportAddressIPv4 ipv4;
    } address;
    address.value = network_game_get_server_address();
    uint16_t server_port = network_game_get_server_port();;

    wchar_t server_address_text[256];
    const wchar_t *prefix = multiplayer_game_text_get_string(MULTIPLAYER_GAME_TEXT_SERVER_ADDRESS_PREFIX);
    swprintf_s(server_address_text, 256, L"%s%d.%d.%d.%d:%d", prefix, address.ipv4.class_d, address.ipv4.class_c, address.ipv4.class_b, address.ipv4.class_a, server_port);
    
    math_rectangle_2d_translate(&rect, 0, line_height);
    text_draw_unicode_string(&rect, NULL, NULL, 0, server_address_text);
}

void scoreboard_render(PlayerHandle player_handle, float fade) {
    scoreboard_draw_background(fade);
    scoreboard_draw_table(player_handle, fade);
    scoreboard_draw_server_info(fade);
}
