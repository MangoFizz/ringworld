#include <stdint.h>

#include "../exception/exception.h"
#include "render.h"

extern RenderGlobals *render_globals;
extern bool *fog_enable;
extern GlobalsRasterizerData **globals_rasterizer_data;

RenderGlobals *render_get_globals(void) {
    return render_globals;
}

bool render_get_fog_enabled(void) {
    return *fog_enable;
}

GlobalsRasterizerData *render_get_globals_rasterizer_data(void) {
    ASSERT(globals_rasterizer_data != NULL);
    return *globals_rasterizer_data;
}
