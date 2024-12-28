#ifndef DEMON__IMPL_MULTIPLAYER_MODE_H
#define DEMON__IMPL_MULTIPLAYER_MODE_H

/**
 * Multiplayer mode determines if it is a multiplayer session and if we are hosting or not.
 * Note that this is not dependent on the map that is loaded, since any map can be loaded in singleplayer mode.
 */
typedef enum MultiplayerMode {
    /** Not in a multiplayer game */
    MULTIPLAYER_MODE_NONE = 0,

    /** Connected to a remote multiplayer game */
    MULTIPLAYER_MODE_CLIENT = 1,

    /** Hosting a multiplayer game */
    MULTIPLAYER_MODE_HOST = 2
} MultiplayerMode;

/**
 * Get the current multiplayer mode
 */
MultiplayerMode get_multiplayer_mode(void);

#endif
