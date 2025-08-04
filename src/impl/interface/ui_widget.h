#ifndef RINGWORLD__INTERFACE__UI_WIDGET_H
#define RINGWORLD__INTERFACE__UI_WIDGET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "../memory/memory.h"
#include "../player/player.h"
#include "../tag/tag.h"
#include "../tag/definitions/ui_widget_definition.h"

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
    char pad1[2];
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
    int16_t local_player_index;
    VectorXYInt position;
    UIWidgetType type;
    bool visible;
    bool render_regardless_of_controller_index;
    bool never_receive_events;
    bool pauses_game_time; 
    bool deleted;
    bool is_error_dialog;
    bool close_if_local_player_controller_present;
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
    TagHandle previous_menu;
    TagHandle previous_menu_list;
    uint16_t focused_item_index;
    uint16_t local_player_index;
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

typedef enum PACKED_ENUM UIWidgetEventRecordType {
    EVENT_TYPE_NONE = 0x0,
    EVENT_TYPE_KEYBOARD = 0x3,
    EVENT_TYPE_MOUSE = 0x4,
    EVENT_TYPE_SIZE = 0xFFFF
} UIWidgetEventRecordType;

typedef struct UIWidgetEventRecord {
    UIWidgetEventRecordType type;
    int16_t controller_index;
    VectorXYInt analog_input; // this could be actually an union
} UIWidgetEventRecord;

typedef enum UIWidgetButtonCaptionStringIndex {
    UI_WIDGET_BUTTON_CAPTION_PROFILE_LABEL = 7
} UIWidgetButtonCaptionStringIndex;

typedef enum FeedbackSound {
    FEEDBACK_SOUND_CURSOR = 1,
    FEEDBACK_SOUND_FORWARD,
    FEEDBACK_SOUND_BACK,
    FEEDBACK_SOUND_FLAG_FAILURE
} FeedbackSound;

/**
 * Get the UI widget globals.
 * @return pointer to the UI widget globals
 */
WidgetGlobals *ui_widget_get_globals(void);

/**
 * Initialize the UI widgets.
 */
void ui_widgets_initialize(void);

/**
 * Get the active widget for a specific controller index.
 * @param controller_index controller index
 * @return pointer to the active widget for the specified controller index
 */
Widget *ui_widget_get_active_widget(int16_t controller_index);

/**
 * Delete a widget and all of its children.
 * @param widget pointer widget to delete
 * @param widget_memory_pool pointer to the memory pool
 */
void ui_widget_delete_recursive(Widget *widget);

/**
 * Create a new widget history node.
 * @param history_node_data pointer to the history node data to copy to the new node
 * @param history_top_node pointer to the top node of the history
 */
void ui_widget_new_history_node(WidgetHistoryNode *history_node_data, WidgetHistoryNode **history_top_node);

/**
 * Create a new widget instance
 * @param controller_index controller index
 * @param definition_tag_data pointer to the widget definition tag data
 * @param widget pointer to the widget block where the new widget will be instantiated
 * @param definition_tag_handle handle of the widget definition tag
 * @param parent pointer to the parent widget
 */
void ui_widget_new_instance(int16_t controller_index, UIWidgetDefinition *definition_tag_data, Widget *widget, TagHandle definition_tag_handle, Widget *parent);

/**
 * Load widget children recursively.
 * @param definition_tag_data pointer to the widget definition tag data
 * @param widget pointer to the widget block where the new widget will be instantiated
 * @return true if the children were loaded successfully, false otherwise
 */
bool ui_widget_load_children_recursive(UIWidgetDefinition *definition_tag_data, Widget *widget);

/**
 * Load a widget by name or tag.
 * @param definition_tag_path path to the widget definition tag
 * @param definition_tag handle of the widget definition tag
 * @param parent pointer to the parent widget
 * @param controller_index controller index
 * @param topmost_widget_definition_handle handle of the topmost widget definition tag
 * @param parent_widget_definition_handle handle of the parent widget definition tag
 * @param child_index_from_parent index of the child widget from the parent widget
 * @return pointer to the loaded widget; NULL if the widget could not be loaded
 */
Widget *ui_widget_load_by_name_or_tag(const char *definition_tag_path, TagHandle definition_tag, Widget *parent, int16_t controller_index, TagHandle topmost_widget_definition_handle, TagHandle parent_widget_definition_handle, uint16_t child_index_from_parent);

/**
 * Launch a widget.
 * @param widget pointer to the widget to launch
 * @param definition_tag_handle handle of the widget definition tag
 */
Widget *ui_widget_launch(Widget *widget, TagHandle definition_tag_handle);

/**
 * Close a widget.
 */
void ui_widget_close(void);

/**
 * Replace a widget with a new one.
 * @param widget pointer to the old widget
 * @param new_widget_definition handle of the new widget definition tag
 * @return pointer to the new widget
 */
Widget *ui_widget_replace(Widget *widget, TagHandle new_widget_definition);

/**
 * Reload a widget.
 * @param widget pointer to the widget to reload
 * @return pointer to the reloaded widget
 */
Widget *ui_widget_reload(Widget *widget);

/**
 * Dispatch an event to a widget.
 * @param widget pointer to the widget
 * @param widget_definition pointer to the widget definition tag data
 * @param event_record pointer to the event record
 * @param event_handler pointer to the event handler
 * @param controller_index controller index
 */
void ui_widget_event_handler_dispatch(Widget *widget, UIWidgetDefinition *widget_definition, UIWidgetEventRecord *event_record, EventHandlerReference *event_handler, int16_t *controller_index);

/**
 * Give focus to a widget instance.
 * @param widget pointer to the widget instance
 * @param child pointer to the child widget
 */
void ui_widget_instance_give_focus_directly(Widget *widget, Widget *child);

/**
 * Check if a widget is a list.
 * @param widget pointer to the widget
 * @return true if the widget is a list, false otherwise
 */
bool ui_widget_is_list(Widget *widget);

/**
 * Get the nth child of a widget.
 * @param widget pointer to the widget
 * @param index index of the child
 * @return pointer to the nth child of the widget
 */
Widget *ui_widget_get_nth_child(Widget *widget, uint16_t index);

/**
 * Get the last child of a widget.
 * @param widget pointer to the widget
 * @return pointer to the last child of the widget
 */
Widget *ui_widget_get_last_child(Widget *widget);

/**
 * Get the topmost parent of a widget.
 * @param widget pointer to the widget
 * @return pointer to the topmost parent of the widget
 */
Widget *ui_widget_get_topmost_parent(Widget *widget);

/**
 * Get the index of a child in a widget.
 * @param widget pointer to the widget
 * @param child pointer to the child
 * @return the index of the child in the widget
 */
int16_t ui_widget_get_index_for_child(Widget *widget, Widget *child);

/**
 * Disable a widget instance.
 * @param widget pointer to the widget instance
 */
void ui_widget_disable_widget(Widget *widget);

/**
 * Enable a widget instance.
 * @param widget pointer to the widget instance
 */
void ui_widget_enable_widget(Widget *widget);

/**
 * Render a widget instance.
 * @param widget pointer to the widget instance
 * @param bounds pointer to the bounds of the widget
 * @param offset pointer to the offset of the widget
 * @param is_root_widget true if the widget is the root widget, false otherwise
 * @param is_focused true if the widget is focused, false otherwise
 * @todo Check if parameters 4 and 5 are correct
 */
void ui_widget_instance_render_recursive(Widget *widget, Rectangle2D *bounds, VectorXYInt offset, bool is_root_widget, bool is_focused);

/**
 * Check if a text box widget is focused.
 * @param widget pointer to the widget instance
 * @return true if the text box widget is focused, false otherwise
 */
bool ui_widget_text_box_is_focused(Widget *widget);

/**
 * Render a text box widget.
 * @param widget pointer to the widget instance
 * @param definition pointer to the widget definition tag data
 * @param bounds pointer to the bounds of the widget
 * @param offset pointer to the offset of the widget
 * @param is_focused true if the widget is focused, false otherwise
 */
void ui_widget_render_text_box(Widget *widget, UIWidgetDefinition *definition, Rectangle2D *bounds, VectorXYInt offset, bool is_focused);

/**
 * Render a column list widget.
 * @param widget pointer to the widget instance
 * @param definition pointer to the widget definition tag data
 * @param bounds pointer to the bounds of the widget
 * @param offset pointer to the offset of the widget
 * @param is_focused true if the widget is focused, false otherwise
 */
void ui_widget_render_column_list(Widget *widget, UIWidgetDefinition *definition, Rectangle2D *bounds, VectorXYInt offset, bool is_focused);

/**
 * Render a spinner list widget.
 * @param widget pointer to the widget instance
 * @param definition pointer to the widget definition tag data
 * @param bounds pointer to the bounds of the widget
 * @param offset pointer to the offset of the widget
 * @param is_focused true if the widget is focused, false otherwise
 */
void ui_widget_render_spinner_list(Widget *widget, UIWidgetDefinition *definition, Rectangle2D *bounds, VectorXYInt offset, bool is_focused);

/**
 * Get the widescreen margin for a widget.
 * @return the widescreen margin as a float
 */
float ui_widget_get_widescreen_margin(void);

/**
 * Get the common button caption string by index.
 * @param index the index of the button caption string
 * @return the common button caption string as a wide character string
 */
const wchar_t *ui_widget_get_common_button_caption(UIWidgetButtonCaptionStringIndex index);

/**
 * Update the player profile text.
 * @param text_widget pointer to the text widget
 * @param profile_name pointer to the profile name
 */
void ui_widget_update_player_profile_text(const wchar_t *profile_name, Widget *text_widget);

/**
 * Play a feedback sound.
 * @param sound the feedback sound to play
 */
void ui_widget_play_feedback_sound(FeedbackSound sound);

#ifdef __cplusplus
}
#endif

#endif
