#include <stdbool.h>
#include <stdio.h>
#include "../exception/exception.h"
#include "../main/main_globals.h"
#include "network_game.h"
#include "network_game_client.h"

static const ColorRGB DEFAULT_COLOR = { 1.0f, 1.0f, 1.0f };

static const ColorRGB PLAYER_COLORS[] = {
    { 1.000f, 1.000f, 1.000f }, 
    { 0.000f, 0.000f, 0.000f }, 
    { 0.000f, 0.000f, 0.996f }, 
    { 0.890f, 0.004f, 0.008f }, 
    { 0.443f, 0.494f, 0.439f }, 
    { 0.004f, 1.000f, 1.000f }, 
    { 0.004f, 1.000f, 0.000f }, 
    { 0.725f, 0.337f, 1.000f }, 
    { 0.957f, 0.063f, 0.671f }, 
    { 1.000f, 1.000f, 0.004f }, 
    { 0.929f, 0.576f, 0.392f }, 
    { 0.000f, 0.498f, 1.000f }, 
    { 0.569f, 0.800f, 0.118f }, 
    { 0.004f, 0.392f, 0.000f }, 
    { 0.078f, 0.220f, 0.376f }, 
    { 0.424f, 0.612f, 0.776f }, 
    { 0.055f, 0.043f, 0.616f }, 
    { 0.620f, 0.600f, 0.961f }, 
};

static const ColorRGB TEAM_COLORS[] = {
    { 0.600f, 0.300f, 0.300f },
    { 0.300f, 0.300f, 0.600f },
};

extern bool *close_server_connection_flag;
extern NetworkGame *network_game_globals_client;
extern NetworkGame *network_game_globals_host;
extern uint32_t *network_game_server_address_host;
extern uint16_t *network_game_server_port_host;

NetworkGame *network_game_get(void) {
    MainGlobals *main_globals = main_get_globals();
    switch(main_globals->game_connection) {
        case GAME_CONNECTION_NETWORK_CLIENT: {
            NetworkGameClient *client = network_game_client_get();
            return &client->game;
        }
        case GAME_CONNECTION_NETWORK_SERVER:
            return network_game_globals_host;
        default:
            return NULL;
    }
}

uint32_t network_game_get_server_address(void) {
    MainGlobals *main_globals = main_get_globals();
    switch(main_globals->game_connection) {
        case GAME_CONNECTION_NETWORK_CLIENT: {
            NetworkGameClient *client = network_game_client_get();
            return client->connection->reliable_endpoint->gs_connection->address.S_un.S_addr; // what am I doing with my life?
        }
        case GAME_CONNECTION_NETWORK_SERVER:
            return *network_game_server_address_host;
        default:
            return 0;
    }
}

uint16_t network_game_get_server_port(void) {
    MainGlobals *main_globals = main_get_globals();
    switch(main_globals->game_connection) {
        case GAME_CONNECTION_NETWORK_CLIENT:
            NetworkGameClient *client = network_game_client_get();
            return client->connection->reliable_endpoint->gs_connection->port;
        case GAME_CONNECTION_NETWORK_SERVER:
            return *network_game_server_port_host;
        default:
            return 0;
    }
}

void network_game_issue_disconnect(void) {
    *close_server_connection_flag = true;
}

const ColorRGB *network_game_get_team_color(NetworkGameTeam team) {
    switch(team) {
        case NETWORK_GAME_TEAM_RED:
        case NETWORK_GAME_TEAM_BLUE:
            return &TEAM_COLORS[team];
        default:
            return &DEFAULT_COLOR;
    }
}

const ColorRGB *network_game_get_player_color(NetworkGamePlayerColor color) {
    switch(color) {
        case NETWORK_GAME_PLAYER_COLOR_WHITE:
        case NETWORK_GAME_PLAYER_COLOR_BLACK:
        case NETWORK_GAME_PLAYER_COLOR_RED:
        case NETWORK_GAME_PLAYER_COLOR_BLUE:
        case NETWORK_GAME_PLAYER_COLOR_GRAY:
        case NETWORK_GAME_PLAYER_COLOR_YELLOW:
        case NETWORK_GAME_PLAYER_COLOR_GREEN:
        case NETWORK_GAME_PLAYER_COLOR_PINK:
        case NETWORK_GAME_PLAYER_COLOR_PURPLE:
        case NETWORK_GAME_PLAYER_COLOR_CYAN:
        case NETWORK_GAME_PLAYER_COLOR_COBALT:
        case NETWORK_GAME_PLAYER_COLOR_ORANGE:
        case NETWORK_GAME_PLAYER_COLOR_TEAL:
        case NETWORK_GAME_PLAYER_COLOR_SAGE:
        case NETWORK_GAME_PLAYER_COLOR_BROWN:
        case NETWORK_GAME_PLAYER_COLOR_TAN:
        case NETWORK_GAME_PLAYER_COLOR_MAROON:
        case NETWORK_GAME_PLAYER_COLOR_SALMON:
        case NETWORK_GAME_PLAYER_COLOR_RANDOM:
            return &PLAYER_COLORS[color];
        default:
            CRASHF_DEBUG("Unknown player color %d", color);
    }
}
