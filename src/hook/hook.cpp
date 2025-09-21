#include <windows.h>
#include <cstring>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <stdexcept>

#include "hook/hook.hpp"
#include "ringworld.h"
#include "version.h"

using namespace Ringworld;

static std::byte *hook_heap;
static const std::size_t hook_heap_size = 512 * 1024;
static std::size_t hook_heap_usage;
bool ringworld_server_mode = false;

extern "C" wchar_t *build_number;
extern "C" void exception_throw_forbidden_function_called(const char *function_name, void *from);

void set_up_transparent_generic_hooks();
void set_up_game_state_hooks();
void set_up_hud_hooks();

static void set_up_build_number() {
    const wchar_t *ringworld_version = L"" RINGWORLD_VERSION_STRING;
    std::wcscpy(build_number, ringworld_version);
}

extern "C" void set_up_ringworld_hooks(Platform platform) {
    // Enable DEP (if doable) because executing code not marked as executable is bad
    SetProcessDEPPolicy(PROCESS_DEP_ENABLE);

    DWORD old_protection;

    // Allocate the heap
    hook_heap = reinterpret_cast<std::byte *>(VirtualAlloc(nullptr, hook_heap_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    hook_heap_usage = 0;

    try {
        switch(platform) {
            case RW_PLATFORM_GAME: 
                ringworld_server_mode = false;
                set_up_game_variables();
                set_up_game_function_mappings();
                set_up_transparent_generic_hooks();
                set_up_game_state_hooks();
                set_up_hud_hooks();
                break;
            case RW_PLATFORM_DEDICATED_SERVER:
                ringworld_server_mode = true;
                set_up_server_variables();
                set_up_server_function_mappings();
                break;
        }
    }
    catch(std::exception &e) {
        MessageBox(nullptr, e.what(), "Error patching the game!", 0);
        std::terminate();
    }

#ifdef RINGWORLD_DEBUG
    set_up_build_number();
#endif

    // Once done, set the protection to execute/read only so we don't get pwned.
    VirtualProtect(hook_heap, hook_heap_size, PAGE_EXECUTE_READ, &old_protection);
}

extern "C" void dispose_ringworld_heap() {
    VirtualFree(hook_heap, hook_heap_size, MEM_DECOMMIT | MEM_RELEASE);
}

void *Hook::write_hook() {
    std::byte *hook_offset = hook_heap + hook_heap_usage;
    std::byte *starting_hook_offset = hook_offset;

    #define HOOK_PUSH_BYTE(value) *(hook_offset++) = static_cast<std::byte>(value)
    #define HOOK_PUSH_DWORD(value) *reinterpret_cast<std::uint32_t *>(hook_offset) = static_cast<std::uint32_t>(value); hook_offset += sizeof(std::uint32_t)

    if(this->forbidden) {
        // Push dword ptr [esp] (our caller address)
        HOOK_PUSH_BYTE(0xFF);
        HOOK_PUSH_BYTE(0x34);
        HOOK_PUSH_BYTE(0x24);

        // Push the name
        HOOK_PUSH_BYTE(0x68);
        HOOK_PUSH_DWORD(reinterpret_cast<std::uintptr_t>(this->name));

        // Now let's call this error handler
        std::uintptr_t call_instruction_end = reinterpret_cast<std::byte *>(exception_throw_forbidden_function_called) - (hook_offset + 5);
        HOOK_PUSH_BYTE(0xE8);
        HOOK_PUSH_DWORD(call_instruction_end);

        // Debug trap
        HOOK_PUSH_BYTE(0xCC);
    }

    else if(this->stubbed) {
        // Return
        HOOK_PUSH_BYTE(0xC3);
    }

    else {
        // Push flags and registers
        //
        // pushad, pushfd
        HOOK_PUSH_BYTE(0x9C);
        HOOK_PUSH_BYTE(0x60);

        // Current stack offset (0x20 bytes from pushad, 0x4 bytes from pushfd, 0x4 bytes from call)
        std::size_t starting_stack_offset = 0x28;
        std::size_t stack_offset = starting_stack_offset;
        std::size_t fp_stack_offset = 0;

        // Push parameters
        if(!this->parameters.empty()) {
            std::size_t parameter_count = this->parameters.size();
            for(std::size_t pi = 0; pi < parameter_count; pi++) {
                std::size_t pi_rev = parameter_count - (pi + 1);
                auto &p = this->parameters[pi_rev];
                switch(p.first) {
                    case Stack: {
                        std::size_t offset = p.second + stack_offset;
                        // push dword ptr [esp + <offset>]
                        if(offset <= 0x7F) {
                            HOOK_PUSH_BYTE(0xFF);
                            HOOK_PUSH_BYTE(0x74);
                            HOOK_PUSH_BYTE(0x24);
                            HOOK_PUSH_BYTE(offset);
                        }
                        else {
                            HOOK_PUSH_BYTE(0xFF);
                            HOOK_PUSH_BYTE(0xB4);
                            HOOK_PUSH_BYTE(0x24);
                            HOOK_PUSH_DWORD(offset);
                        }
                        stack_offset += sizeof(std::uint32_t);
                        break;
                    }
                    case Register: {
                        switch (this->hook_type) {
                            case GameToLib: {
                                switch(p.second) {
                                    case EAX:
                                        HOOK_PUSH_BYTE(0x50);
                                        stack_offset += sizeof(std::uint32_t);
                                        break;
                                    case EBX:
                                        HOOK_PUSH_BYTE(0x53);
                                        stack_offset += sizeof(std::uint32_t);
                                        break;
                                    case ECX:
                                        HOOK_PUSH_BYTE(0x51);
                                        stack_offset += sizeof(std::uint32_t);
                                        break;
                                    case EDX:
                                        HOOK_PUSH_BYTE(0x52);
                                        stack_offset += sizeof(std::uint32_t);
                                        break;
                                    case ESI:
                                        HOOK_PUSH_BYTE(0x56);
                                        stack_offset += sizeof(std::uint32_t);
                                        break;
                                    case EDI:
                                        HOOK_PUSH_BYTE(0x57);
                                        stack_offset += sizeof(std::uint32_t);
                                        break;
                                    case EBP:
                                        HOOK_PUSH_BYTE(0x55);
                                        stack_offset += sizeof(std::uint32_t);
                                        break;
                                    case ESP:
                                        throw std::logic_error("ESP register is not supported");
                                    case ST0:
                                    case ST1:
                                    case ST2:
                                    case ST3:
                                    case ST4:
                                    case ST5:
                                    case ST6:
                                    case ST7: {
                                        std::size_t fp_offset = p.second - ST0;
                                        if(fp_stack_offset > fp_offset) {
                                            char error_message[1024];
                                            std::snprintf(error_message, sizeof(error_message) - 1, "Can't use FP register ST%zu because we already popped %zu float(s)."
                                                                                                    "For example, ST0 is now what ST%zu was.\n\n"
                                                                                                    "Try re-arranging the registers!", fp_offset, fp_stack_offset, fp_stack_offset);
                                            throw std::logic_error(error_message);
                                        }

                                        // sub esp, 0x8 (makes room in the stack)
                                        HOOK_PUSH_BYTE(0x83);
                                        HOOK_PUSH_BYTE(0xEC);
                                        HOOK_PUSH_BYTE(0x08);
                                        stack_offset += sizeof(double);

                                        // pop the float stack until we get what we want
                                        for(; fp_stack_offset <= fp_offset; fp_stack_offset++) {
                                            // fstp qword ptr [esp]
                                            HOOK_PUSH_BYTE(0xDD);
                                            HOOK_PUSH_BYTE(0x1C);
                                            HOOK_PUSH_BYTE(0x24);
                                        }

                                        break;
                                    }

                                    default:
                                        throw std::logic_error("Unknown register: " + std::to_string(p.second));
                                }
                                break;
                            }
                            case LibToGame: {
                                HOOK_PUSH_BYTE(0x8B);

                                std::uint8_t operand;

                                // mov <register>, <offset>
                                switch(p.second) {
                                    case EAX:
                                        operand = 0x44;
                                        break;
                                    case EBX:
                                        operand = 0x5C;
                                        break;
                                    case ECX:
                                        operand = 0x4C;
                                        break;
                                    case EDX:
                                        operand = 0x54;
                                        break;
                                    case ESI:
                                        operand = 0x74;
                                        break;
                                    case EDI:
                                        operand = 0x7C;
                                        break;
                                    case EBP:
                                        operand = 0x6C;
                                        break;
                                    case ESP:
                                        throw std::logic_error("ESP register is not supported");
                                    default:
                                        throw std::logic_error("Unknown register (" + std::to_string(p.second) + ") in hook " + this->name);
                                }

                                std::size_t offset = stack_offset + sizeof(std::uint32_t) * pi_rev;

                                if(offset <= 0x7F) {
                                    HOOK_PUSH_BYTE(operand);
                                    HOOK_PUSH_BYTE(0x24);
                                    HOOK_PUSH_BYTE(offset);
                                }
                                else {
                                    HOOK_PUSH_BYTE(operand + 0x40);
                                    HOOK_PUSH_BYTE(0x24);
                                    HOOK_PUSH_DWORD(offset);
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }

        // Put a call function here
        //
        // call <function_address>
        auto *call_instruction_end = (hook_offset + 1 + sizeof(std::uintptr_t));
        std::uintptr_t call_offset;
        switch(this->hook_type) {
            case GameToLib:
                call_offset = reinterpret_cast<std::byte *>(this->destination_address) - call_instruction_end;
                break;
            case LibToGame:
                call_offset = reinterpret_cast<std::byte *>(this->function_address) - call_instruction_end;
                break;
        }
        HOOK_PUSH_BYTE(0xE8);
        HOOK_PUSH_DWORD(call_offset);

        // Cleanup stack
        //
        // add esp, <stack_to_cleanup>
        std::size_t stack_to_cleanup = stack_offset - starting_stack_offset;
        if(stack_to_cleanup != 0) {
            if(stack_to_cleanup < 0x80) {
                HOOK_PUSH_BYTE(0x83);
                HOOK_PUSH_BYTE(0xC4);
                HOOK_PUSH_BYTE(stack_to_cleanup & 0xFF);
            }
            else {
                HOOK_PUSH_BYTE(0x81);
                HOOK_PUSH_BYTE(0xC4);
                HOOK_PUSH_DWORD(stack_to_cleanup);
            }
        }

        // Return value? If so, set EAX in our stack to our current EAX value
        //
        // mov dword ptr [esp+0x1C], eax
        if(this->uses_return_value) {
            HOOK_PUSH_BYTE(0x89);
            HOOK_PUSH_BYTE(0x44);
            HOOK_PUSH_BYTE(0x24);
            HOOK_PUSH_BYTE(0x1C);

            // If we return a 64-bit value, set EDX as well
            //
            // mov dword ptr [esp+0x14], edx
            if(this->returns_64_bit_value) {
                HOOK_PUSH_BYTE(0x89);
                HOOK_PUSH_BYTE(0x54);
                HOOK_PUSH_BYTE(0x24);
                HOOK_PUSH_BYTE(0x14);
            }
        }

        // Pop flags and registers
        //
        // popad, popfd
        HOOK_PUSH_BYTE(0x61);
        HOOK_PUSH_BYTE(0x9D);

        // Return
        //
        // ret
        HOOK_PUSH_BYTE(0xC3);
    }

    #undef HOOK_PUSH_BYTE

    hook_heap_usage += (hook_offset - starting_hook_offset);

    // Jump if needed
    if(this->hook_type == GameToLib) {
        DWORD old_protection;
        auto *addr = reinterpret_cast<std::byte *>(this->function_address);
        VirtualProtect(reinterpret_cast<void *>(addr), 5, PAGE_READWRITE, &old_protection);
        *addr = static_cast<std::byte>(0xE9);
        *reinterpret_cast<std::uintptr_t *>(addr + 1) = static_cast<std::uintptr_t>((starting_hook_offset) - (addr + 5));
        VirtualProtect(reinterpret_cast<void *>(addr), 5, old_protection, &old_protection);
    }

    return reinterpret_cast<void *>(starting_hook_offset);
}
