#include "../debug/assertion.h"
#include "rasterizer.h"

extern RasterizerGlobals *rasterizer_globals;
extern RasterizerWindowRenderParameters *window_parameters;
extern RasterizerFrameParameters *frame_parameters;
extern GlobalsRasterizerData **globals_rasterizer_data;

RasterizerGlobals *rasterizer_get_globals(void) {
    return rasterizer_globals;
}

RasterizerWindowRenderParameters *rasterizer_get_window_parameters(void) {
    return window_parameters;
}

RasterizerFrameParameters *rasterizer_get_frame_parameters(void) {
    ASSERT(frame_parameters != NULL);
    return frame_parameters;
}

GlobalsRasterizerData *render_get_globals_rasterizer_data(void) {
    ASSERT(globals_rasterizer_data != NULL);
    return *globals_rasterizer_data;
}
