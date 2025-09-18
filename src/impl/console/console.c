#include <stdio.h>
#include <stdarg.h>
#include "../../event/event.h"
#include "../game/game_engine.h"
#include "../script/hsc.h"
#include "../main/main_globals.h"
#include "../math/color.h"
#include "../shell/shell.h"
#include "../terminal/terminal.h"
#include "console.h"

extern ConsoleGlobals *console_globals;
extern bool *console_command_is_being_processed;

static const ColorARGB CONSOLE_INPUT_COLOR = {1.0f, 1.0f, 0.3f, 1.0f};
static const char *CONSOLE_PROMPT = "Ringworld( ";

ConsoleGlobals *console_get_globals(void) {
    return console_globals;
}

void console_set_command_processing(bool is_processing) {
    *console_command_is_being_processed = is_processing;
}

void console_initialize(void) {
    ConsoleGlobals *console_globals = console_get_globals();
    strncpy(console_globals->input_state.prompt, CONSOLE_PROMPT, sizeof(console_globals->input_state.prompt));
    console_globals->input_state.color.a = CONSOLE_INPUT_COLOR.a;
    console_globals->input_state.color.r = CONSOLE_INPUT_COLOR.r;
    console_globals->input_state.color.g = CONSOLE_INPUT_COLOR.g;
    console_globals->input_state.color.b = CONSOLE_INPUT_COLOR.b;
    console_globals->last_history_command_index = -1;
    console_globals->selected_history_command_index = -1;
    console_globals->input_state.command_result[0] = '\0';
    console_globals->history_commands_count = 0;
    console_globals->console_is_enabled = false;

    const char **argv = shell_get_process_argv();
    for(int i = 1; i < shell_get_process_argc(); i++) {
        if(stricmp(argv[i], "-console") == 0) {
            console_globals->console_is_enabled = true;
            break;
        }
    }
}

bool console_process_command(uint32_t flags, const char *command) {
    if(command[0] == ';' || command[0] == '#' || (command[0] == '/' && command[1] == '/')) {
        return false;
    }

    // Get the command name
    char command_name[252];
    strncpy(command_name, command, sizeof(command_name));
    char *first_space = strchr(command_name, ' ');
    if(first_space) {
        *first_space = '\0';
    }

    ConsoleGlobals *console_globals = console_get_globals();
    MainGlobals *main_globals = main_get_globals();
    GameEngineInterface *game_engine = game_engine_get_current_interface();

    // Copy command to history
    console_globals->last_history_command_index = (console_globals->last_history_command_index + 1) % 8;
    char *history_command = console_globals->history_commands[console_globals->history_commands_count];
    strncpy(history_command, command, sizeof(console_globals->history_commands[0]) - 1);
    history_command[sizeof(console_globals->history_commands[0]) - 1] = '\0';

    // Update history index
    console_globals->selected_history_command_index = -1;
    if(console_globals->history_commands_count < 8) {
        console_globals->history_commands_count++;
    }
    
    uint16_t game_flags = 0;
    if(game_engine != NULL) {
        switch(main_globals->game_connection) {
            case GAME_CONNECTION_NETWORK_CLIENT:
                game_flags = 0x11;
                break;
            case GAME_CONNECTION_NETWORK_SERVER:
                game_flags = 0x12;
                break;
            default:
                game_flags = 0x10;
                break;
        }
    }

    char *search_result[258];
    uint16_t results_count = hsc_find_functions(0x28, (search_result + 1), 0x100, command_name, game_flags | flags);
    bool command_found = false;
    for(uint16_t i = results_count; 0 < i; i--) {
        if(stricmp(command_name, search_result[i]) == 0) {
            command_found = true;
            break;
        }
    }

    if(!command_found) {
        char command_input[252];
        strncpy(command_input, command, sizeof(command_input));
        bool event_handled = RINGWORLD_EVENT_DISPATCH(RW_EVENT_CONSOLE_COMMAND_NOT_FOUND, command_input);
        if(event_handled) {
            return true;
        }
        terminal_info_printf("Requested function \"%s\" cannot be executed now.", command_name);
        return false;
    }

    console_set_command_processing(true);
    bool res = hsc_execute_script(command);
    console_set_command_processing(false);

    return res;
}
