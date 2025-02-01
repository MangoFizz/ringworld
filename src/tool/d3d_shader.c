#include <stdio.h>
#include <stdbool.h>
#include <d3d9.h>
#include <d3dx9.h>

void compile_shader(const char* shader_file, const char *output_file, const char *profile) {
    LPD3DXBUFFER code;
    LPD3DXBUFFER errors;
    
    HRESULT hr = D3DXCompileShaderFromFile(shader_file, NULL, NULL, "main", profile, 0, &code, &errors, NULL);
    if(FAILED(hr)) {
        if(errors) {
            printf("Shader compilation failed: %s\n", (char*)ID3DXBuffer_GetBufferPointer(errors));
            ID3DXBuffer_Release(errors);
        } 
        else {
            printf("Shader compilation failed with unknown error.\n");
        }
    } 
    else {
        FILE* file = fopen(output_file, "wb");
        if(!file) {
            printf("Error: Could not open file for writing.\n");
            return;
        }
        fwrite(ID3DXBuffer_GetBufferPointer(code), 1, ID3DXBuffer_GetBufferSize(code), file);
        fclose(file);
        printf("Shader compiled successfully.\n");
        ID3DXBuffer_Release(code);
    }
}
