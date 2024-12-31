#ifndef RINGWORLD__RASTERIZER__RASTERIZER_DX9_SHADER_COMPILER_H
#define RINGWORLD__RASTERIZER__RASTERIZER_DX9_SHADER_COMPILER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <d3dcompiler.h>

/**
 * Compiles a pixel shader.
 * @param source            The source code of the shader.
 * @param entry             Name of the entry point of the shader.
 * @param profile           Shader model of the shader.
 * @param defines           List of preprocessor defines.
 * @param compiled_shader   Pointer to the compiled shader.
 * @return                  true if successful, false if not.
 */
bool rasterizer_dx9_shader_compiler_compile_psh(const char *source, const char *entry, const char *profile, D3D_SHADER_MACRO *defines, ID3DBlob **compiled_shader);

#ifdef __cplusplus
}
#endif

#endif
