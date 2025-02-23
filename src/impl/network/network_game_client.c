#include "network_game_client.h"

extern NetworkGameClient **network_game_client;

NetworkGameClient *network_game_client_get(void) {
    return *network_game_client;
}
