.intel_syntax noprefix

.text

.globl _shader_transparent_generic_switch_case_asm
_shader_transparent_generic_switch_case_asm:
    mov edx, dword ptr [ebp + 0xC]
    push edx
    push ebx
    call _rasterizer_shader_transparent_generic_draw
    add esp, 0x8
    mov eax, 0x5380C1
    jmp eax

.globl _shader_transparent_generic_update_instances_asm
_shader_transparent_generic_update_instances_asm:
    pushfd
    pushad
    call _rasterizer_shader_transparent_generic_update_instances_for_current_map
    popad
    popfd
    call dword ptr [_map_load_thing]
    jmp dword ptr [_map_load_thing_return]
