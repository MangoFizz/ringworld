#include <stdbool.h>
#include "netgame.h"

extern bool *close_server_connection_flag;

void network_game_issue_disconnect(void) {
    *close_server_connection_flag = true;
}
