#include "../exception/exception.h"
#include "../math/math.h"
#include "../network/network_game.h"
#include "../text/unicode_string_list.h"
#include "multiplayer_game_text.h"

const wchar_t *multiplayer_game_text_get_string(MultiplayerGameTextIndex index) {
    TagHandle multiplayer_text_tag = multiplayer_get_text_tag();
    if(!HANDLE_IS_NULL(multiplayer_text_tag)) {
        const wchar_t *text = unicode_string_list_get_string_or_null(multiplayer_text_tag, index);
        if(text != NULL) {
            return text;
        }
    }
    switch(index) {
        case MULTIPLAYER_GAME_TEXT_PLACE_COLUMN_HEADER:
            return L"Place";
        case MULTIPLAYER_GAME_TEXT_NAME_COLUMN_HEADER:
            return L"Name";
        case MULTIPLAYER_GAME_TEXT_KILLS_COLUMN_HEADER:
            return L"Kills";
        case MULTIPLAYER_GAME_TEXT_ASSISTS_COLUMN_HEADER:
            return L"Assists";
        case MULTIPLAYER_GAME_TEXT_DEATHS_COLUMN_HEADER:
            return L"Deaths";
        case MULTIPLAYER_GAME_TEXT_PING_COLUMN_HEADER:
            return L"Ping";
        case MULTIPLAYER_GAME_TEXT_SCORE_DEAD:
            return L"Dead";
        case MULTIPLAYER_GAME_TEXT_SCORE_QUIT:
            return L"Quit";
        case MULTIPLAYER_GAME_TEXT_HEADER_NO_LIVES:
            return L"(no lives)";
        case MULTIPLAYER_GAME_TEXT_HEADER_ONE_LIFE:
            return L"(1 life)";
        case MULTIPLAYER_GAME_TEXT_HEADER_LIVES:
            return L"(%d lives)";
        case MULTIPLAYER_GAME_TEXT_HEADER_DRAW:
            return L"Game ends in a draw";
        case MULTIPLAYER_GAME_TEXT_HEADER_TEAM_LOST:
            return L"Your team lost";
        case MULTIPLAYER_GAME_TEXT_HEADER_YOU_LOST:
            return L"You lost";
        case MULTIPLAYER_GAME_TEXT_HEADER_TEAM_WON:
            return L"Your team won";
        case MULTIPLAYER_GAME_TEXT_HEADER_YOU_WON:
            return L"You won";
        case MULTIPLAYER_GAME_TEXT_HEADER_RED_LEADS_BLUE:
            return L"Red leads Blue %s to %s %s";
        case MULTIPLAYER_GAME_TEXT_HEADER_BLUE_LEADS_RED:
            return L"Blue leads Red %s to %s %s";
        case MULTIPLAYER_GAME_TEXT_HEADER_TEAMS_TIED:
            return L"Teams tied at %s %s";
        case MULTIPLAYER_GAME_TEXT_HEADER_TIED_FOR_PLACE:
            return L"Tied for %s place with %s %s";
        case MULTIPLAYER_GAME_TEXT_HEADER_IN_PLACE:
            return L"In %s place with %s %s";
        case MULTIPLAYER_GAME_TEXT_SERVER_ADDRESS_PREFIX:
            return L"Server IP address - ";
        case MULTIPLAYER_GAME_TEXT_RED_TEAM_SCORE:
            return L"\tRed Team\t%s";
        case MULTIPLAYER_GAME_TEXT_BLUE_TEAM_SCORE:
            return L"\tBlue Team\t%s";
        default:
            CRASHF_DEBUG("Invalid scoreboard string index %d. FIX ME!", index);
    }
}

const wchar_t *SCOREBOARD_DEFAULT_PLACE_STRING[] = {
    L"1st",
    L"2nd",
    L"3rd",
    L"4th",
    L"5th",
    L"6th",
    L"7th",
    L"8th",
    L"9th",
    L"10th",
    L"11th",
    L"12th",
    L"13th",
    L"14th",
    L"15th",
    L"16th"
};

const wchar_t *multiplayer_game_text_get_place_string(uint16_t place) {
    TagHandle multiplayer_text_tag = multiplayer_get_text_tag();
    place = min_i32(place, NETWORK_GAME_MAX_PLAYERS); 
    if(!HANDLE_IS_NULL(multiplayer_text_tag)) {
        const wchar_t *text = unicode_string_list_get_string_or_null(multiplayer_text_tag, 36 + place - 1);
        if(text != NULL) {
            return text;
        }
    }
    return SCOREBOARD_DEFAULT_PLACE_STRING[place - 1];
}
