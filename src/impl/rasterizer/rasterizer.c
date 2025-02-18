#include "rasterizer.h"

extern RasterizerGlobals *rasterizer_globals;
extern RasterizerWindowRenderParameters *window_parameters;

RasterizerGlobals *rasterizer_get_globals(void) {
    return rasterizer_globals;
}

RasterizerWindowRenderParameters *rasterizer_get_window_parameters(void) {
    return window_parameters;
}
