#include <stdio.h>
#include <winsock.h>
#include "../bitmap/bitmap.h"
#include "../game/game_engine.h"
#include "../game/game_globals.h"
#include "../main/main_globals.h"
#include "../time/time.h"
#include "../console/console.h"
#include "../interface/hud.h"
#include "../network/network_game.h"
#include "../math/math.h"
#include "../math/color.h"
#include "../player/player.h"
#include "../text/text.h"
#include "../text/unicode_string_list.h"
#include "../rasterizer/rasterizer_screen.h"
#include "../rasterizer/rasterizer_text.h"

wchar_t *scoreboard_get_column_header_text(TagHandle multiplayer_text_tag, uint16_t index) {
    if(HANDLE_IS_NULL(multiplayer_text_tag)) {
        return L"";
    }
    wchar_t *text = unicode_string_list_get_string(multiplayer_text_tag, 67 + index);
    if(text == NULL) {
        return L"<missing string>";
    }
    return text;
}

wchar_t *scoreboard_get_place_text(TagHandle multiplayer_text_tag, uint16_t place) {
    if(HANDLE_IS_NULL(multiplayer_text_tag) || place > 16) {
        return L"";
    }
    wchar_t *text = unicode_string_list_get_string(multiplayer_text_tag, 36 + place - 1);
    if(text == NULL) {
        return L"<missing string>";
    }
    return text;
}

void scoreboard_draw_row(wchar_t *text, bool highlight, ColorARGB *color, int16_t row_index) {
    TextDrawGlobals *text_draw_globals = text_get_drawing_globals();
    Globals *globals = game_globals_get();

    if(globals->interface_bitmaps.count == 0) {
        return;
    }

    GlobalsInterfaceBitmaps *interface_bitmaps = TAG_BLOCK_GET_ELEMENT(globals->interface_bitmaps, 0);
    TagHandle font = interface_bitmaps->font_terminal.tag_handle;
    if(HANDLE_IS_NULL(font)) {
        return;
    }

    uint16_t screen_width = rasterizer_screen_get_width();
    uint16_t screen_height = rasterizer_screen_get_height();
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

    Rectangle2D rect;
    rect.left = 0;
    rect.right = RASTERIZER_SCREEN_BASE_WIDTH;
    rect.top = 59;
    rect.bottom = 76;
    math_rectangle_2d_translate(&rect, widescreen_margin, row_index * 15);

    ColorARGB text_color = *color;
    if(highlight) {
        text_color.r = min_f32(text_color.r + 0.4f, 1.0f);
        text_color.g = min_f32(text_color.g + 0.4f, 1.0f);
        text_color.b = min_f32(text_color.b + 0.4f, 1.0f);
    }

    text_set_drawing_parameters(-1, 0, 0, font, &text_color);
    rasterizer_draw_unicode_string(&rect, NULL, NULL, 0, text);
    text_reset_tab_stops();
}

typedef struct PlayerScore {
    int16_t player_id;
    int16_t player_index;
    int16_t team_index;
    int16_t score;
    int16_t place;
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

void scoreboard_sort_players(PlayerScore *scores, uint16_t *count) {
    PlayersTable *players_table = player_get_table();
    GameTypeEngineInterface *game_type_engine = game_engine_get_current_interface();
    NetworkGame *netgame_globals = network_game_get_globals();

    PlayerScore *current_player_score = scores;
    size_t players_count = 0;
    for(size_t i = 0; i < NETWORK_GAME_MAX_PLAYERS; i++) {
        NetworkGamePlayer *netgame_player = &netgame_globals->players[i];
        if(netgame_player->player_list_index != -1) {
            Player *player = table_get_element(players_table, MAKE_HANDLE(0, netgame_player->player_list_index));
            current_player_score->player_id = player->player_id;
            current_player_score->player_index = i;
            current_player_score->score = game_type_engine->get_score(i, false);
            if(netgame_globals->gametype_variant.universal_variant.teams) {
                current_player_score->team_index = player->team_index;
            }
            else {
                current_player_score->team_index = 0;
            }
            current_player_score++;
            players_count++;
        }
    }

    // Sort by score
    qsort(scores, players_count, sizeof(PlayerScore), scoreboard_players_sort_compare_scores);
    
    // Assign places
    int16_t previous_score = -1, previous_place = 0;
    for(size_t i = 0; i < players_count; i++) {
        PlayerScore *player_score = &scores[i];
        if(player_score->score != previous_score) {
            player_score->place = previous_place + 1;
            previous_score = player_score->score;
            previous_place++;
        }
        else {
            player_score->place = previous_place;
        }
    }

    *count = players_count;
}

typedef struct ScoreboardRowData {
    PlayerHandle player_handle;
    uint16_t place;
    wchar_t *name;
    int16_t score;
    uint16_t kills;
    uint16_t deaths;
    uint16_t assists;
    uint8_t team;
    uint16_t ping;
} ScoreboardRowData;

void scoreboard_build_rows(ScoreboardRowData *scoreboard_rows, uint16_t count) {
    PlayersTable *players_table = player_get_table();
    NetworkGame *netgame_globals = network_game_get_globals();
    
    PlayerScore players_scores[NETWORK_GAME_MAX_PLAYERS];
    uint16_t players_scores_count = 0;
    scoreboard_sort_players(players_scores, &players_scores_count);

    memset(scoreboard_rows, 0, sizeof(ScoreboardRowData) * count);
    for(size_t i = 0; i < count; i++) {
        scoreboard_rows[i].player_handle = NULL_HANDLE;
    }

    uint8_t teams_count = netgame_globals->gametype_variant.universal_variant.teams ? NUM_OF_NETWORK_GAME_TEAMS : 1;
    uint8_t scoreboard_row_index = 0;
    for(size_t team_index = 0; team_index < teams_count; team_index++) {
        for(size_t player_score_index = 0; player_score_index < players_scores_count; player_score_index++) {
            PlayerScore *player_score = &players_scores[player_score_index];
            if(player_score->team_index == team_index) {
                ScoreboardRowData *scoreboard_row = &scoreboard_rows[scoreboard_row_index];
                PlayerHandle player_handle = MAKE_HANDLE(player_score->player_id, player_score->player_index);
                Player *player = table_get_element(players_table, player_handle);
                scoreboard_row->player_handle = player_handle;
                scoreboard_row->name = player->name;
                scoreboard_row->kills = player->kills[0];
                scoreboard_row->deaths = player->deaths;
                scoreboard_row->assists = player->assists[0];
                scoreboard_row->ping = player->ping;
                scoreboard_row->team = player_score->team_index;
                scoreboard_row->score = player_score->score;
                scoreboard_row->place = player_score->place;
                scoreboard_row_index++;
            }
        }
    }
}

void scoreboard_draw_score_table(PlayerHandle player_handle, float fade) {
    GameTypeEngineInterface *game_type_engine = game_engine_get_current_interface();
    PlayersTable *players_table = player_get_table();
    NetworkGame *netgame_globals = network_game_get_globals();
    MainGlobals *main_globals = main_get_globals();

    TagHandle mp_text_tag = lookup_tag("ui\\multiplayer_game_text", TAG_GROUP_UNICODE_STRING_LIST);
    wchar_t *place_label = scoreboard_get_column_header_text(mp_text_tag, 0);
    wchar_t *name_label = scoreboard_get_column_header_text(mp_text_tag, 1);
    wchar_t *kills_label = scoreboard_get_column_header_text(mp_text_tag, 2);
    wchar_t *assists_label = scoreboard_get_column_header_text(mp_text_tag, 3);
    wchar_t *deaths_label = scoreboard_get_column_header_text(mp_text_tag, 4);
    wchar_t *ping_label = L"Ping"; // not sure where this is stored
    
    wchar_t score_label[256];
    game_type_engine->get_score_header_string(&score_label);

    wchar_t scoreboard_header[512];
    swprintf(scoreboard_header, 512, L"\t%s\t%s\t%s\t%s\t%s\t%s\t%s", place_label, name_label, score_label, kills_label, assists_label, deaths_label, ping_label);

    ColorARGB text_color;
    text_color.a = fade;
    text_color.r = 0.5f;
    text_color.g = 0.5f;
    text_color.b = 0.5f;
    scoreboard_draw_row(scoreboard_header, false, &text_color, 1);

    ScoreboardRowData rows_data[16];
    scoreboard_build_rows(rows_data, 16);

    wchar_t row_text[256];
    for(size_t i = 0; i < 16; i++) {
        ScoreboardRowData *row_data = &rows_data[i];
        if(!HANDLE_IS_NULL(row_data->player_handle)) {
            wchar_t *place_text = scoreboard_get_place_text(mp_text_tag, row_data->place);
            swprintf(row_text, 256, L"\t%s\t%s\t%d\t%d\t%d\t%d\t%d", place_text, row_data->name, row_data->score, row_data->kills, row_data->assists, row_data->deaths, row_data->ping);
            ColorARGB row_color;
            row_color.a = fade;
            if(netgame_globals->gametype_variant.universal_variant.teams) {
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
            scoreboard_draw_row(row_text, row_data->player_handle.value == player_handle.value, &row_color, i + 2);
        }
    }
}

void scoreboard_draw_background(float fade) {
    uint16_t screen_width = rasterizer_screen_get_width();
    uint16_t screen_height = rasterizer_screen_get_height();
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

void scoreboard_draw_header(PlayerHandle player_handle, float fade) {
    HUDGlobals *hud_globals = hud_get_globals();
    NetworkGame *netgame_globals = network_game_get_globals();
    GameTypeEngineInterface *game_type_engine = game_engine_get_current_interface();

    uint16_t screen_width = rasterizer_screen_get_width();
    uint16_t screen_height = rasterizer_screen_get_height();

    Rectangle2D server_name_rect;
    server_name_rect.left = 0;
    server_name_rect.right = screen_width - 5;
    server_name_rect.top = 10;
    server_name_rect.bottom = screen_height - 40;

    ColorARGB server_name_color;
    server_name_color.a = fade;
    server_name_color.r = 1.0f;
    server_name_color.g = 1.0f;
    server_name_color.b = 1.0f;

    TagHandle server_name_font = hud_globals->fullscreen_font.tag_handle;
    if(HANDLE_IS_NULL(server_name_font)) {
        server_name_font = hud_globals->splitscreen_font.tag_handle;
        if(HANDLE_IS_NULL(server_name_font)) {
            server_name_font = lookup_tag("ui\\large_ui", TAG_GROUP_FONT);
        }
    }

    wchar_t text[256];
    wprintf(L"Score header text: %s\n", game_type_engine->get_score_string(player_handle, text));

    // text_set_drawing_parameters(-1, 1, 0, server_name_font, &server_name_color);
    // rasterizer_draw_unicode_string(&server_name_rect, NULL, NULL, 0, text);
}

void scoreboard_draw_server_name(float fade) {
    HUDGlobals *hud_globals = hud_get_globals();
    NetworkGame *netgame_globals = network_game_get_globals();

    uint16_t screen_width = rasterizer_screen_get_width();
    uint16_t screen_height = rasterizer_screen_get_height();

    Rectangle2D server_name_rect;
    server_name_rect.left = 10;
    server_name_rect.right = screen_width - 5;
    server_name_rect.top = screen_height - 60;
    server_name_rect.bottom = screen_height - 40;

    ColorARGB server_name_color;
    server_name_color.a = fade;
    server_name_color.r = 1.0f;
    server_name_color.g = 1.0f;
    server_name_color.b = 1.0f;

    TagHandle server_name_font = hud_globals->fullscreen_font.tag_handle;
    if(HANDLE_IS_NULL(server_name_font)) {
        server_name_font = hud_globals->splitscreen_font.tag_handle;
        if(HANDLE_IS_NULL(server_name_font)) {
            server_name_font = lookup_tag("ui\\large_ui", TAG_GROUP_FONT);
        }
    }

    text_set_drawing_parameters(-1, 1, 0, server_name_font, &server_name_color);
    rasterizer_draw_unicode_string(&server_name_rect, NULL, NULL, 0, netgame_globals->server_name);
}

void scoreboard_draw_server_address(float fade) {
    HUDGlobals *hud_globals = hud_get_globals();

    uint16_t screen_width = rasterizer_screen_get_width();
    uint16_t screen_height = rasterizer_screen_get_height();

    Rectangle2D server_address_rect;
    server_address_rect.left = 10;
    server_address_rect.right = screen_width - 5;
    server_address_rect.top = screen_height - 30;
    server_address_rect.bottom = screen_height - 10;

    ColorARGB server_address_color;
    server_address_color.a = fade;
    server_address_color.r = 1.0f;
    server_address_color.g = 1.0f;
    server_address_color.b = 1.0f;

    TagHandle server_address_font = hud_globals->fullscreen_font.tag_handle;
    if(HANDLE_IS_NULL(server_address_font)) {
        server_address_font = hud_globals->splitscreen_font.tag_handle;
        if(HANDLE_IS_NULL(server_address_font)) {
            server_address_font = lookup_tag("ui\\large_ui", TAG_GROUP_FONT);
        }
    }

    union {
        uint32_t value;
        NetworkTransportAddressIPv4 ipv4;
    } address;
    
    address.value = network_game_get_server_address();
    uint16_t server_port = network_game_get_server_port();;

    TagHandle mp_text_tag = lookup_tag("ui\\multiplayer_game_text", TAG_GROUP_UNICODE_STRING_LIST);
    wchar_t *prefix = unicode_string_list_get_string(mp_text_tag, 190);
    if(prefix == NULL) {
        prefix = L"Server IP address - ";
    }
    
    wchar_t server_address_text[256];
    swprintf(server_address_text, 256, L"%s%d.%d.%d.%d:%d", prefix, address.ipv4.class_a, address.ipv4.class_b, address.ipv4.class_c, address.ipv4.class_d, server_port);

    text_set_drawing_parameters(-1, 1, 0, server_address_font, &server_address_color);
    rasterizer_draw_unicode_string(&server_address_rect, NULL, NULL, 0, server_address_text);
}

void scoreboard_render(PlayerHandle player_handle, float fade) {
    scoreboard_draw_background(fade);
    scoreboard_draw_header(player_handle, fade);
    scoreboard_draw_score_table(player_handle, fade);
    scoreboard_draw_server_name(fade);
    scoreboard_draw_server_address(fade);
}
