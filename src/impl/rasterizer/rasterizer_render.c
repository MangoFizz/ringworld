#include <stdint.h>
#include <stdio.h>

#include "rasterizer_render.h"

extern RenderGlobals *render_globals;
extern FrameParameters *frame_parameters;
extern bool *fog_enable;

RenderGlobals *rasterizer_render_get_globals(void) {
    return render_globals;
}

FrameParameters *rasterizer_render_get_frame_parameters(void) {
    return frame_parameters;
}

bool rasterizer_render_get_fog_enabled(void) {
    return *fog_enable;
}
