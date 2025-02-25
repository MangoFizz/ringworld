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
