#ifndef RINGWORLD__HOOK__HOOK_HPP
#define RINGWORLD__HOOK__HOOK_HPP

#include <cstdint>
#include <vector>
#include <windows.h>

namespace Ringworld {
    enum HookType {
        /**
         * This is intended to be called from the game to the library.
         *
         * This will patch the function at the given address.
         */
        GameToLib,

        /**
         * This is intended to be called from the library to the game.
         *
         * No patches will be done to the game.
         */
        LibToGame
    };

    enum ParameterStorageType {
        Register,
        Stack
    };

    enum ParameterRegister {
        ST0, ST1, ST2, ST3, ST4, ST5, ST6, ST7,
        EAX, EBX, ECX, EDX, ESI, EDI, EBP, ESP
    };

    class Hook {
        public:
            Hook(const char *name, std::uintptr_t function_address) :
                name(name),
                function_address(function_address),
                hook_type(LibToGame) {
            }
            Hook(const char *name, std::uintptr_t function_address, std::uintptr_t destination_address) :
                name(name),
                function_address(function_address),
                destination_address(destination_address),
                hook_type(GameToLib) {
            }

            /**
             * Push a parameter.
             *
             * If Register, index should be a ParameterRegister. Otherwise, it should be the stack offset.
             *
             * For GameToLib functions, it will convert these into using the stack.
             *
             * For LibToGame functions, it will convert from stack into these.
             */
            Hook &push_parameter(ParameterStorageType type, std::size_t index) {
                parameters.emplace_back(type, index);
                return *this;
            }

            /**
             * Specify that a return value should be handled.
             */
            Hook &has_return_value() {
                this->uses_return_value = true;
                return *this;
            }

            /**
             * Specify that the return value is 64 bits.
             */
            Hook &return_64_bit_value() {
                this->returns_64_bit_value = true;
                return *this;
            }

            /**
             * Write the hook.
             *
             * @return hook address
             */
            void *write_hook();

            /**
             * Specify that this may not be used. The game will crash if this is called.
             */
            Hook &forbid() {
                this->forbidden = true;
                this->hook_type = GameToLib;
                return *this;
            }

            /**
             * Specify that this function instantly returns when called.
             */
            Hook &stub() {
                this->stubbed = true;
                this->hook_type = GameToLib;
                return *this;
            }

        private:
            std::uintptr_t function_address;
            std::uintptr_t destination_address;
            HookType hook_type;
            bool returns_64_bit_value = false;
            bool uses_return_value = false;
            bool forbidden = false;
            bool stubbed = false;
            const char *name;
            std::vector<std::pair<ParameterStorageType, std::size_t>> parameters;
    };

    void set_up_game_function_mappings();
    void set_up_server_function_mappings();
    void set_up_game_variables();
    void set_up_server_variables();
}

/**
 * Overwrite the data at the pointer with the given data even if this pointer is read-only.
 * @param pointer This is the pointer that points to the data to be overwritten.
 * @param data    This is the pointer that points to the data to be copied.
 * @param length  This is the length of the data.
 */
template<typename T> inline void overwrite(void *pointer, const T *data, std::size_t length) noexcept {
    // Instantiate our new_protection and old_protection variables.
    DWORD new_protection = PAGE_EXECUTE_READWRITE, old_protection;

    // Apply read/write/execute protection
    VirtualProtect(pointer, length, new_protection, &old_protection);

    // Copy
    std::copy(data, data + length, reinterpret_cast<T *>(pointer));

    // Restore the older protection unless it's the same
    if(new_protection != old_protection) {
        VirtualProtect(pointer, length, old_protection, &new_protection);
    }
}

/**
 * Overwrite the data at the pointer with the given data even if this pointer is read-only.
 * @param pointer This is the pointer that points to the data to be overwritten.
 * @param data    This is the pointer that points to the data to be copied.
 */
template<typename T> inline void overwrite(void *pointer, const T &data) noexcept {
    return overwrite(pointer, &data, 1);
}

#endif
