#ifndef RINGWORLD__MULTIPLAYER__MULTIPLAYER_TAGS_H
#define RINGWORLD__MULTIPLAYER__MULTIPLAYER_TAGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tag/tag.h"

/**
 * Get the multiplayer game text tag.
 * @return Tag handle of the unicode string list 
 */
inline TagHandle multiplayer_get_text_tag(void) {
    return lookup_tag("ui\\multiplayer_game_text", TAG_GROUP_UNICODE_STRING_LIST);
}

#ifdef __cplusplus
}
#endif

#endif
