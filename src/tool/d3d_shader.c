#include <stdio.h>
#include <stdbool.h>
#include <d3d9.h>
#include <d3dx9.h>
#define COBJMACROS 
#include <d3dcompiler.h>
#include "../impl/rasterizer/rasterizer_dx9_shader_compiler.h"

void compile_shader(const char* shader_file, const char *output_file, const char *profile) {
    ID3DBlob *code;
    bool res = rasterizer_dx9_shader_compiler_compile_shader_from_file(shader_file, "main", profile, NULL, &code);
    if(!res) {
        printf("Shader compilation failed.\n");
    } 
    else {
        FILE* file = fopen(output_file, "wb");
        if(!file) {
            printf("Error: Could not open file for writing.\n");
            return;
        }
        fwrite(ID3D10Blob_GetBufferPointer(code), 1, ID3D10Blob_GetBufferSize(code), file);
        fclose(file);
        printf("Shader compiled successfully.\n");
        ID3D10Blob_Release(code);
    }
}

void disassemble_shader(const char* shader_file, const char *output_file) {
    FILE* file = fopen(shader_file, "rb");
    if(!file) {
        printf("Error: Could not open shader file for reading.\n");
        return;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    void* shader_bytecode = malloc(file_size);
    if(!shader_bytecode) {
        printf("Error: Could not allocate memory for shader bytecode.\n");
        fclose(file);
        return;
    }
    fread(shader_bytecode, 1, file_size, file);
    fclose(file);
    
    LPD3DXBUFFER code;
    const char *comment = "// Disassembled with ringworld-tool \n";
    HRESULT hr = D3DXDisassembleShader(shader_bytecode, FALSE, comment, &code);
    if(FAILED(hr)) {
        printf("Shader disassembly failed.\n");
    } 
    else {
        FILE* file = fopen(output_file, "w");
        if(!file) {
            printf("Error: Could not open file for writing.\n");
            return;
        }
        fwrite(ID3DXBuffer_GetBufferPointer(code), 1, ID3DXBuffer_GetBufferSize(code) - 1, file);
        fclose(file);
        printf("Shader disassembled successfully.\n");
        ID3DXBuffer_Release(code);
    }
}

void assemble_shader(const char* shader_file, const char *output_file) {
    LPD3DXBUFFER code;
    LPD3DXBUFFER errors;

    HRESULT hr = D3DXAssembleShaderFromFile(shader_file, NULL, NULL, 0, &code, &errors);
    if(FAILED(hr)) {
        if(errors) {
            printf("Shader assembly failed: %s\n", (char*)ID3DXBuffer_GetBufferPointer(errors));
            ID3DXBuffer_Release(errors);
        } 
        else {
            printf("Shader assembly failed with unknown error.\n");
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
        printf("Shader assembled successfully.\n");
        ID3DXBuffer_Release(code);
    }
}
