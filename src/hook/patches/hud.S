.intel_syntax noprefix

.text

.globl _hud_calculate_font_height_asm
_hud_calculate_font_height_asm:
    pushad
    pushfd
    mov cl, byte ptr [esp + 0x50]
    push ecx
    push eax
    call _hud_calculate_font_height
    add esp, 0x8
    mov dword ptr [esp + 0x1C], eax
    popfd
    popad
    ret
