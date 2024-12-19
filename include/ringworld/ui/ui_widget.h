

#ifndef RINGWORLD__UI__UI_WIDGET_H
#define RINGWORLD__UI__UI_WIDGET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "../memory/memory.h"
#include "../player/player.h"
#include "../tag/tag.h"

typedef struct EnqueuedErrorDescriptor {
    int16_t error_string;
    int16_t local_player;
    Bool display_modal;
    Bool display_paused;
} EnqueuedErrorDescriptor;
_Static_assert(sizeof(EnqueuedErrorDescriptor) == 0x06);

typedef struct DeferredErrorDescriptor {
    int16_t error_string;
    Bool display_modal;
    Bool display_paused;
} DeferredErrorDescriptor;
_Static_assert(sizeof(DeferredErrorDescriptor) == 0x04);

typedef struct WidgetTextBoxParameters {
    wchar_t *text;
    int16_t string_list_index;
    ColorARGB text_color;
    Bool flashing;
} WidgetTextBoxParameters;
_Static_assert(sizeof(WidgetTextBoxParameters) == 0x1C);

typedef struct WidgetListParameters {
    int16_t selected_list_item_index;
    int16_t list_item_top_index;
    int16_t current_list_item_index;
    int16_t last_list_tab_direction;
    void *list_items;
    int16_t number_of_items;
    struct Widget *extended_description;
    wchar_t *item_text;
    int16_t spin_activated;
} WidgetListParameters;
_Static_assert(sizeof(WidgetListParameters) == 0x1C);

typedef struct WidgetAnimationData {
    int16_t current_frame_index;
    int16_t first_frame_index;
    int16_t last_frame_index;
    int16_t number_of_sprite_frames;
} WidgetAnimationData;
_Static_assert(sizeof(WidgetAnimationData) == 0x8);

typedef struct Widget {
    TagHandle definition_tag_handle;
    char *name;
    uint16_t local_player_index;
    VectorXYInt position;
    uint16_t type;
    Bool visible;
    Bool render_regardless_of_controller_index;
    Bool never_receive_events;
    Bool pauses_game_time; 
    Bool deleted;
    Bool is_error_dialog;
    Bool close_if_local_player_controller_present;
    TickCount32 creation_process_start_time;
    uint32_t ms_to_close;
    uint32_t ms_to_close_fade_time;
    float alpha_modifier;
    struct Widget *previous;
    struct Widget *next;
    struct Widget *parent;
    struct Widget *child;
    struct Widget *focused_child;
    union {
        WidgetTextBoxParameters text_box_parameters;
        WidgetListParameters list_parameters;
    };
    WidgetAnimationData animation_data;
} Widget; 
_Static_assert(sizeof(Widget) == 0x60);

typedef struct WidgetHistoryNode {
    Widget *previous_menu;
    Widget *previous_menu_list;
    uint16_t focused_item_index;
    struct WidgetHistoryNode *previous;
} WidgetHistoryNode;
_Static_assert(sizeof(WidgetHistoryNode) == 0x10);

typedef struct WidgetGlobals {
    Widget *active_widget[MAX_LOCAL_PLAYERS];
    WidgetHistoryNode *history_top_node[MAX_LOCAL_PLAYERS];
    int32_t current_time_ms;
    int32_t popup_display_time;
    int16_t deferred_error_code;
    int16_t pause_time_count;
    float fade_to_black;
    EnqueuedErrorDescriptor enqueued_errors[MAX_LOCAL_PLAYERS];
    DeferredErrorDescriptor priority_warning;
    DeferredErrorDescriptor deferred_for_cinematic_errors[MAX_LOCAL_PLAYERS];
    void *initialization_thread;
    int16_t filesystem_check_result;
    Bool initialized;
    Bool dont_load_children_recursive;
    Bool debug_show_path;
    Bool processing_inhibited;
    Bool main_menu_music;
    Bool sound_paused;
} WidgetGlobals;
_Static_assert(sizeof(WidgetGlobals) == 0x34);

/**
 * Initialize the UI widgets.
 */
void ui_widgets_initialize(void);

#ifdef __cplusplus
}
#endif

#endif
