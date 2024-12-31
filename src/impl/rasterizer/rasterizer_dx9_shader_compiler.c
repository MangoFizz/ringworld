#include <stdio.h>
#define COBJMACROS 

#include "rasterizer_dx9_shader_compiler.h"

bool rasterizer_dx9_shader_compiler_compile_psh(const char *source, const char *entry, const char *profile, D3D_SHADER_MACRO *defines, ID3DBlob **compiled_shader) {
    ID3DBlob *error_messages = NULL;
    HRESULT result = D3DCompile(source, strlen(source), NULL, defines, NULL, entry, profile, 0, 0, compiled_shader, &error_messages);
    if(FAILED(result)) {
        if(error_messages != NULL) {
            fprintf(stderr, "Error compiling pixel shader: %s\n", (char *)ID3D10Blob_GetBufferPointer(error_messages));
            ID3D10Blob_Release(error_messages);
        }
        return false;
    }
    return true;
}
