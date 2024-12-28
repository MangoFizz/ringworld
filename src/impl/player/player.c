#include "../memory/memory.h"
#include "../memory/table.h"
#include "player.h"

extern PlayersGlobals **players_globals;
extern PlayersTable **players_data;
extern TeamsTable **teams_data;
extern PlayerControlGlobals **player_control_globals;

void players_data_initialize(void) {
    *players_data = (PlayersTable *)create_table("players", 16, sizeof(Player));
    *teams_data = (TeamsTable *)create_table("teams", 16, sizeof(Team));
    
    PlayersGlobals *p_globals = (PlayersGlobals *)allocate_heap(sizeof(PlayersGlobals));
    p_globals->local_player_network_id = 0xFFFFFFFF;
    for(size_t i = 0; i < MAX_LOCAL_PLAYERS; i++) {
        p_globals->local_players[i] = NULL_HANDLE;
        p_globals->local_player_dead_units[i] = NULL_HANDLE;
    }
    p_globals->local_player_count = 0;
    *players_globals = p_globals;

    *player_control_globals = (PlayerControlGlobals *)allocate_heap(sizeof(PlayerControlGlobals));
}
