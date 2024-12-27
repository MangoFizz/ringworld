#ifndef RINGWORLD__RINGWORLD_H
#define RINGWORLD__RINGWORLD_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef RW_EXPORT_SYMBOLS
    #define RINGWORLD_API __declspec(dllexport)
#else
    #define RINGWORLD_API
#endif

typedef enum Platform {
    RW_PLATFORM_GAME,
    RW_PLATFORM_DEDICATED_SERVER
} Platform;

/**
 * Set up the hooks for the ringworld functions.
 * 
 * This function should be called ONCE at the start of the game by 
 * the module responsible for setting up ringworld.
 * 
 * @param platform The platform to set up the hooks for.
 */
void set_up_ringworld_hooks(Platform platform);

/**
 * Free the heap used by ringworld.
 */
void dispose_ringworld_heap();

#ifdef __cplusplus
}
#endif

#endif
