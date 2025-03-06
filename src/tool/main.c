#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void compile_shader(const char* shader_file, const char* output_file, const char *profile);
void disassemble_shader(const char* shader_file, const char *output_file);
void assemble_shader(const char* shader_file, const char *output_file);
bool unpack_vsh_file(const char *vsh_file, const char *path);
bool pack_vsh_file(const char *path, const char *vsh_file);
bool unpack_effect_collection_file(const char *effect_collection_file, const char *path, bool disassemble);
bool pack_effect_collection(const char *path, const char *effect_collection_file);
bool update_effect_collection(const char *effect_collection_file, const char *output_file);

void printHelp() {
    printf("Ringworld Tool\n");
    printf("Usage: tool <command> [parameters...]\n");
    printf("Commands:\n");
    printf("  compile-shader <file> <output-file> <profile>\n");
    printf("  disassemble-shader <file> <output-file>\n");
    printf("  assemble-shader <file> <output-file>\n");
    printf("  unpack-vertex-shaders <vsh-file> <path>\n");
    printf("  pack-vertex-shaders <path> <vsh-file>\n");
    printf("  unpack-effect-collection <effect-collection-file> <path> [disassemble]\n");
    printf("  pack-effect-collection <path> <effect-collection-file>\n");
    printf("  update-effect-collection <effect-collection-file> <output-file>\n");
    printf("\n");
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        printHelp();
        return 0;
    }

    const char* verb = argv[1];

    if (strcmp(verb, "compile-shader") == 0) {
        if(argc < 3) {
            fprintf(stderr, "Error: Missing file parameter.\n");
            return 1;
        }
        if(argc < 4) {
            fprintf(stderr, "Error: Missing output file parameter.\n");
            return 1;
        }
        if(argc < 5) {
            fprintf(stderr, "Error: Missing profile parameter.\n");
            return 1;
        }
        compile_shader(argv[2], argv[3], argv[4]);
    }
    else if (strcmp(verb, "disassemble-shader") == 0) {
        if(argc < 3) {
            fprintf(stderr, "Error: Missing file parameter.\n");
            return 1;
        }
        if(argc < 4) {
            fprintf(stderr, "Error: Missing output file parameter.\n");
            return 1;
        }
        disassemble_shader(argv[2], argv[3]);
    }
    else if (strcmp(verb, "assemble-shader") == 0) {
        if(argc < 3) {
            fprintf(stderr, "Error: Missing file parameter.\n");
            return 1;
        }
        if(argc < 4) {
            fprintf(stderr, "Error: Missing output file parameter.\n");
            return 1;
        }
        assemble_shader(argv[2], argv[3]);
    }
    else if (strcmp(verb, "unpack-vertex-shaders") == 0) {
        if(argc < 3) {
            fprintf(stderr, "Error: Missing vsh file parameter.\n");
            return 1;
        }
        if(argc < 4) {
            fprintf(stderr, "Error: Missing path parameter.\n");
            return 1;
        }
        bool res = unpack_vsh_file(argv[2], argv[3]);
        if(!res) {
            return 1;
        }
    }
    else if (strcmp(verb, "pack-vertex-shaders") == 0) {
        if(argc < 3) {
            fprintf(stderr, "Error: Missing path parameter.\n");
            return 1;
        }
        if(argc < 4) {
            fprintf(stderr, "Error: Missing vsh file parameter.\n");
            return 1;
        }
        bool res = pack_vsh_file(argv[2], argv[3]);
        if(!res) {
            return 1;
        }
    }
    else if (strcmp(verb, "unpack-effect-collection") == 0) {
        if(argc < 3) {
            fprintf(stderr, "Error: Missing effect collection file parameter.\n");
            return 1;
        }
        if(argc < 4) {
            fprintf(stderr, "Error: Missing path parameter.\n");
            return 1;
        }
        bool disassemble = false;
        if(argc >= 5) {
            disassemble = strcmp(argv[4], "true") == 0;
        }
        bool res = unpack_effect_collection_file(argv[2], argv[3], disassemble);
        if(!res) {
            return 1;
        }
    }
    else if (strcmp(verb, "pack-effect-collection") == 0) {
        if(argc < 3) {
            fprintf(stderr, "Error: Missing path parameter.\n");
            return 1;
        }
        if(argc < 4) {
            fprintf(stderr, "Error: Missing effect collection file parameter.\n");
            return 1;
        }
        bool res = pack_effect_collection(argv[2], argv[3]);
        if(!res) {
            return 1;
        }
    }
    else if (strcmp(verb, "update-effect-collection") == 0) {
        if(argc < 3) {
            fprintf(stderr, "Error: Missing effect collection file parameter.\n");
            return 1;
        }
        if(argc < 4) {
            fprintf(stderr, "Error: Missing output file parameter.\n");
            return 1;
        }
        bool res = update_effect_collection(argv[2], argv[3]);
        if(!res) {
            return 1;
        }
    }
    else {
        fprintf(stderr, "Error: Unknown verb '%s'.\n", verb);
        printHelp();
        return 1;
    }

    return 0;
}
