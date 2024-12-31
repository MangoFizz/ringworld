#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>

void printHelp() {
    printf("Ringworld Tool\n");
    printf("Usage: tool <command> [parameters...]\n");
    printf("Commands:\n");
    printf("  build-pixel-shader <file> <output-file>\n");
    printf("\n");
}

void CompileShader(const char* shaderFile, const char* outputFile) {
    LPD3DXBUFFER code;
    LPD3DXBUFFER errors;
    
    HRESULT hr = D3DXCompileShaderFromFile(shaderFile, NULL, NULL, "main", "ps_3_0", 0, &code, &errors, NULL);
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
        FILE* file = fopen(outputFile, "wb");
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

int main(int argc, char* argv[]) {
    if(argc < 2) {
        printHelp();
        return 0;
    }

    const char* verb = argv[1];

    if (strcmp(verb, "build-pixel-shader") == 0) {
        if(argc < 3) {
            fprintf(stderr, "Error: Missing file parameter.\n");
            return 1;
        }
        if(argc < 4) {
            fprintf(stderr, "Error: Missing output file parameter.\n");
            return 1;
        }
        CompileShader(argv[2], argv[3]);
    }
    else {
        fprintf(stderr, "Error: Unknown verb '%s'.\n", verb);
        printHelp();
        return 1;
    }

    return 0;
}
