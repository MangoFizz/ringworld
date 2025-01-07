#ifndef RINGWORLD__PROFILE__PLAYER_PROFILE_H
#define RINGWORLD__PROFILE__PLAYER_PROFILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "../memory/memory.h"

#pragma pack(push)
#pragma pack(1)

typedef enum PACKED_ENUM PlayerColorSetting {
    PLAYER_COLOR_WHITE,
    PLAYER_COLOR_BLACK,
    PLAYER_COLOR_RED,
    PLAYER_COLOR_BLUE,
    PLAYER_COLOR_GRAY,
    PLAYER_COLOR_YELLOW,
    PLAYER_COLOR_GREEN,
    PLAYER_COLOR_PINK,
    PLAYER_COLOR_PURPLE,
    PLAYER_COLOR_CYAN,
    PLAYER_COLOR_COBALT,
    PLAYER_COLOR_ORANGE,
    PLAYER_COLOR_TEAL,
    PLAYER_COLOR_SAGE,
    PLAYER_COLOR_BROWN,
    PLAYER_COLOR_TAN,
    PLAYER_COLOR_MAROON,
    PLAYER_COLOR_SALMON,
    PLAYER_COLOR_RANDOM,
    NUM_OF_PLAYER_COLORS,
    PLAYER_COLOR_SIZE = 0xFF
} PlayerColorSetting;

typedef struct PlayerProfileDetails {
    wchar_t player_name[12]; 
    uint8_t pad1[0x100];
    PlayerColorSetting player_color;
} PlayerProfileDetails;
_Static_assert(sizeof(PlayerProfileDetails) == 281);

typedef enum PACKED_ENUM PlayerControlsAction {
    CONTROLS_ACTION_JUMP,
    CONTROLS_ACTION_SWITCH_GRENADE,
    CONTROLS_ACTION_ACTION,
    CONTROLS_ACTION_SWITCH_WEAPON,
    CONTROLS_ACTION_MELEE_ATTACK,
    CONTROLS_ACTION_FLASHLIGHT,
    CONTROLS_ACTION_THROW_GRENADE,
    CONTROLS_ACTION_FIRE_WEAPON,
    CONTROLS_ACTION_MENU_ACCEPT,
    CONTROLS_ACTION_MENU_BACK,
    CONTROLS_ACTION_CROUCH,
    CONTROLS_ACTION_SCOPE_ZOOM,
    CONTROLS_ACTION_SHOW_SCORES,
    CONTROLS_ACTION_RELOAD,
    CONTROLS_ACTION_EXCHANGE_WEAPON,
    CONTROLS_ACTION_SAY,
    CONTROLS_ACTION_SAY_TO_TEAM,
    CONTROLS_ACTION_SAY_TO_VEHICLE,
    CONTROLS_ACTION_SCREENSHOT,
    CONTROLS_ACTION_MOVE_FORWARD,
    CONTROLS_ACTION_MOVE_BACKWARD,
    CONTROLS_ACTION_MOVE_LEFT,
    CONTROLS_ACTION_MOVE_RIGHT,
    CONTROLS_ACTION_LOOK_UP,
    CONTROLS_ACTION_LOOK_DOWN,
    CONTROLS_ACTION_LOOK_LEFT,
    CONTROLS_ACTION_LOOK_RIGHT,
    CONTROLS_ACTION_SHOW_RULES,
    CONTROLS_ACTION_SHOW_PLAYER_NAMES,
    NUM_OF_CONTROLS_ACTIONS,
    CONTROLS_ACTION_SIZE = 0xFFFF
} PlayerControlsAction;

typedef struct PlayerControlsKeyboardSettings {
    PlayerControlsAction escape;
    PlayerControlsAction f1;
    PlayerControlsAction f2;
    PlayerControlsAction f3;
    PlayerControlsAction f4;
    PlayerControlsAction f5;
    PlayerControlsAction f6;
    PlayerControlsAction f7;
    PlayerControlsAction f8;
    PlayerControlsAction f9;
    PlayerControlsAction f10;
    PlayerControlsAction f11;
    PlayerControlsAction f12;
    PlayerControlsAction printscreen;
    PlayerControlsAction scroll_lock;
    PlayerControlsAction pause_break;
    PlayerControlsAction grave;
    PlayerControlsAction num_row_1;
    PlayerControlsAction num_row_2;
    PlayerControlsAction num_row_3;
    PlayerControlsAction num_row_4;
    PlayerControlsAction num_row_5;
    PlayerControlsAction num_row_6;
    PlayerControlsAction num_row_7;
    PlayerControlsAction num_row_8;
    PlayerControlsAction num_row_9;
    PlayerControlsAction num_row_0;
    PlayerControlsAction en_dash;
    PlayerControlsAction equals;
    PlayerControlsAction backspace;
    PlayerControlsAction tab;
    PlayerControlsAction q;
    PlayerControlsAction w;
    PlayerControlsAction e;
    PlayerControlsAction r;
    PlayerControlsAction t;
    PlayerControlsAction y;
    PlayerControlsAction u;
    PlayerControlsAction i;
    PlayerControlsAction o;
    PlayerControlsAction p;
    PlayerControlsAction bracket_l;
    PlayerControlsAction bracket_r;
    PlayerControlsAction backslash;
    PlayerControlsAction caps_lock;
    PlayerControlsAction a;
    PlayerControlsAction s;
    PlayerControlsAction d;
    PlayerControlsAction f;
    PlayerControlsAction g;
    PlayerControlsAction h;
    PlayerControlsAction j;
    PlayerControlsAction k;
    PlayerControlsAction l;
    PlayerControlsAction semi_colon;
    PlayerControlsAction apostrophe;
    PlayerControlsAction enter;
    PlayerControlsAction shift_l;
    PlayerControlsAction z;
    PlayerControlsAction x;
    PlayerControlsAction c;
    PlayerControlsAction v;
    PlayerControlsAction b;
    PlayerControlsAction n;
    PlayerControlsAction m;
    PlayerControlsAction comma;
    PlayerControlsAction period;
    PlayerControlsAction slash;
    PlayerControlsAction r_shift;
    PlayerControlsAction l_ctrl;
    PlayerControlsAction l_win;
    PlayerControlsAction l_alt;
    PlayerControlsAction space;
    PlayerControlsAction r_alt;
    PlayerControlsAction r_win;
    PlayerControlsAction menu;
    PlayerControlsAction r_ctrl;
    PlayerControlsAction up_arrow;
    PlayerControlsAction down_arrow;
    PlayerControlsAction left_arrow;
    PlayerControlsAction right_arrow;
    PlayerControlsAction insert;
    PlayerControlsAction home;
    PlayerControlsAction pg_up;
    PlayerControlsAction del;
    PlayerControlsAction end;
    PlayerControlsAction pg_down;
    PlayerControlsAction num_lock;
    PlayerControlsAction kp_divide;
    PlayerControlsAction kp_multiply;
    PlayerControlsAction keypad_0;
    PlayerControlsAction keypad_1;
    PlayerControlsAction keypad_2;
    PlayerControlsAction keypad_3;
    PlayerControlsAction keypad_4;
    PlayerControlsAction keypad_5;
    PlayerControlsAction keypad_6;
    PlayerControlsAction keypad_7;
    PlayerControlsAction keypad_8;
    PlayerControlsAction keypad_9;
    PlayerControlsAction keypad_minus;
    PlayerControlsAction keypad_plus;
    PlayerControlsAction unknown_1;
    PlayerControlsAction keypad_decimal;
    PlayerControlsAction unknown_2;
    PlayerControlsAction unknown_3;
    PlayerControlsAction unknown_4;
    PlayerControlsAction unknown_5;
    PlayerControlsAction unknown_6;
} PlayerControlsKeyboardSettings;
_Static_assert(sizeof(PlayerControlsKeyboardSettings) == 218);

typedef struct PlayerControlsMouseSettings {
    PlayerControlsAction left_button;
    PlayerControlsAction middle_button;
    PlayerControlsAction right_button;
    PlayerControlsAction button_4;
    PlayerControlsAction button_5;
    PlayerControlsAction button_6;
    PlayerControlsAction button_7;
    PlayerControlsAction button_8;
    PlayerControlsAction horizontal_axis_minus;
    PlayerControlsAction horizontal_axis_plus;
    PlayerControlsAction vertical_axis_minus;
    PlayerControlsAction vertical_axis_plus;
    PlayerControlsAction wheel_minus;
    PlayerControlsAction wheel_plus;
} PlayerControlsMouseSettings;
_Static_assert(sizeof(PlayerControlsMouseSettings) == 28);

typedef struct PlayerControlsGamepadButtonsSettings {
    PlayerControlsAction button_0;
    PlayerControlsAction button_1;
    PlayerControlsAction button_2;
    PlayerControlsAction button_3;
    PlayerControlsAction button_4;
    PlayerControlsAction button_5;
    PlayerControlsAction button_6;
    PlayerControlsAction button_7;
    PlayerControlsAction button_8;
    PlayerControlsAction button_9;
    PlayerControlsAction button_10;
    char pad1[0x2A];
} PlayerControlsGamepadButtonsSettings;
_Static_assert(sizeof(PlayerControlsGamepadButtonsSettings) == 64);

typedef struct PlayerControlsGamepadAxisSettings {
    PlayerControlsAction axis_1_pos;
    PlayerControlsAction axis_1_neg;
    PlayerControlsAction axis_2_pos;
    PlayerControlsAction axis_2_neg;
    PlayerControlsAction axis_3_pos;
    PlayerControlsAction axis_3_neg;
    PlayerControlsAction axis_4_pos;
    PlayerControlsAction axis_4_neg;
    PlayerControlsAction axis_5_pos;
    PlayerControlsAction axis_5_neg;
    PlayerControlsAction axis_6_pos;
    PlayerControlsAction axis_6_neg;
    char pad1[0x68];
} PlayerControlsGamepadAxisSettings;
_Static_assert(sizeof(PlayerControlsGamepadAxisSettings) == 128);

typedef struct PlayerControlsGamepadDPadSettings {
    PlayerControlsAction DirPadUp;
    char pad1[2];
    PlayerControlsAction DirPadRight;
    char pad2[2];
    PlayerControlsAction DirPadDown;
    char pad3[2];
    PlayerControlsAction DirPadLeft;
    char pad4[0xF2];
} PlayerControlsGamepadDPadSettings;
_Static_assert(sizeof(PlayerControlsGamepadDPadSettings) == 256);

typedef enum PACKED_ENUM DirectInputButtons {
    DINPUT_BUTTON_0,
    DINPUT_BUTTON_1,
    DINPUT_BUTTON_2,
    DINPUT_BUTTON_3,
    DINPUT_BUTTON_4,
    DINPUT_BUTTON_5,
    DINPUT_BUTTON_6,
    DINPUT_BUTTON_7,
    DINPUT_BUTTON_8,
    DINPUT_BUTTON_9,
    DINPUT_BUTTON_10,
    DINPUT_BUTTON_11,
    DINPUT_BUTTON_12,
    DINPUT_BUTTON_13,
    DINPUT_BUTTON_14,
    DINPUT_BUTTON_SIZE = 0xFFFF
} DirectInputButtons;

typedef struct PlayerControlsGamepadMenuSettings {
    DirectInputButtons menu_accept;
    DirectInputButtons menu_back;
} PlayerControlsGamepadMenuSettings;
_Static_assert(sizeof(PlayerControlsGamepadMenuSettings) == 4);

typedef struct PlayerControlsGamepadSettings {
    PlayerControlsGamepadButtonsSettings gamepad_buttons[4];
    PlayerControlsGamepadMenuSettings gamepad_menu_buttons[4];
    PlayerControlsGamepadAxisSettings gamepad_axis_set[4];
    PlayerControlsGamepadDPadSettings gamepad_dpad_buttons[4];
} PlayerControlsGamepadSettings;
_Static_assert(sizeof(PlayerControlsGamepadSettings) == 1808);

typedef struct PlayerControlsSettings {
    uint8_t invert_vertical_axis;
    char pad1[4];
    PlayerControlsKeyboardSettings keyboard_bindings;
    PlayerControlsMouseSettings mouse_bindings;
    PlayerControlsGamepadSettings gamepad_bindings;
    char pad2[26];
    uint8_t horizontal_mouse_sensitivity;
    uint8_t vertical_mouse_sensitivity;
    uint8_t gamepad_horizontal_sensitivities[4];
    uint8_t gamepad_vertical_sensitivities[4];
} ControlSettings;
_Static_assert(sizeof(ControlSettings) == 2095);

typedef struct PlayerVideoSettings {
    uint16_t resolution_width;
    uint16_t resolution_height;
    uint8_t refresh_rate;
    char pad1[2];
    uint8_t frame_rate;
    uint8_t specular;
    uint8_t shadows;
    uint8_t decals;
    uint8_t particles;
    uint8_t texture_quality;
    char pad2;
    uint8_t gamma;
    char pad3;
} PlayerVideoSettings;
_Static_assert(sizeof(PlayerVideoSettings) == 16);

typedef struct PlayerAudioSettings {
    uint8_t master_volume;
    uint8_t effects_volume;
    uint8_t music_volume;
    uint8_t enable_eax;
    uint8_t enable_hardware_acceleration;
    uint8_t sound_quality;
    char pad1;
    uint8_t audio_variety;
} AudioSettings;
_Static_assert(sizeof(AudioSettings) == 8);

typedef struct PlayerNetworkSettings {
    wchar_t server_name[32]; 
    char pad1[0xE0];
    wchar_t password[9]; 
    char pad2[1];
    uint8_t max_players;
    char pad3[0x100];
    uint8_t connection_type;
    char pad4[1];
    wchar_t server_address[32]; 
    uint16_t server_port;
    uint16_t client_port;
} PlayerNetworkSettings;
_Static_assert(sizeof(PlayerNetworkSettings) == 634);

typedef struct PlayerGamepadInfo {
    wchar_t gamepad_name[256]; 
    char pad1[12];
    uint16_t vendor_id;
    uint16_t product_id;
    char pad2[6];
    char pidvid[6];
    uint8_t dupe_id;
    char pad3[3];
} PlayerGamepadInfo;
_Static_assert(sizeof(PlayerGamepadInfo) == 544);

typedef struct PlayerProfile {
    char pad1[2];
    PlayerProfileDetails player_details;
    char pad2[0x14];
    ControlSettings control_settings;
    char pad3[0x10A];
    PlayerVideoSettings video_settings;
    char pad4[0x100];
    AudioSettings audio_settings;
    char pad5[0x20C];
    PlayerNetworkSettings network_settings;
    char pad6[0x102];
    PlayerGamepadInfo gamepad_info[4];
    char pad7[0x674];
    uint32_t crc32_hash;
} PlayerProfile;
_Static_assert(sizeof(PlayerProfile) == 0x2000);

/**
 * Reads the player profile data from the saved games directory.
 * @param profile_name The name of the player profile.
 * @param profile_data Pointer to a buffer to store the player profile data.
 * @return True if the player profile was successfully read, false otherwise.
 */
bool saved_games_read_player_profile(uint32_t profile_name, PlayerProfile *profile_data);

/**
 * Gets the ID of the player profile for the specified local player index.
 * @param local_player_index The local player index.
 * @return The ID of the player profile.
 */
uint32_t saved_games_get_current_profile_id(uint16_t local_player_index);

/**
 * Gets the player profile for the specified local player index.
 * @param local_player_index The local player index.
 * @return Pointer to the player profile.
 */
PlayerProfile *saved_games_get_player_profile(uint16_t local_player_index);


#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif
