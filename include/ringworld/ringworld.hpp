#ifndef RINGWORLD__RINGWORLD_HPP
#define RINGWORLD__RINGWORLD_HPP

typedef enum Platform {
    PLATFORM_CLIENT,
    PLATFORM_DEDICATED_SERVER
} Platform;

void set_up_ringworld_hooks(Platform platform);

#endif
