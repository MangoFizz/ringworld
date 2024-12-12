#ifndef RINGWORLD__RINGWORLD_H
#define RINGWORLD__RINGWORLD_H

typedef enum Platform {
    PLATFORM_CLIENT,
    PLATFORM_DEDICATED_SERVER
} Platform;

void set_up_ringworld_hooks(Platform platform);

#endif
