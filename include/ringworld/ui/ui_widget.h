

#ifndef RINGWORLD__UI__UI_WIDGET_H
#define RINGWORLD__UI__UI_WIDGET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "../memory/memory.h"
#include "../types/types.h"
#include "../tag/tag.h"

typedef struct WidgetMemoryPoolBlockHeader {
    uint32_t head;
    uint32_t size;
    uint32_t block_index;
    struct MemoryPoolResourceHeader *previous;
    struct MemoryPoolResourceHeader *next;
} WidgetMemoryPoolElementHeader;

typedef struct WidgetMemoryPool {
    const char *name;
    void *allocated_memory;
    size_t allocated_size;
    size_t unk_1;
    char unk_2[0x24];
} WidgetMemoryPool;
_Static_assert(sizeof(WidgetMemoryPool) == 0x34);

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

typedef struct Widget {
    TagHandle definition_tag_handle;
    const char *name;
    uint16_t controller_index;
    VectorXYInt position;
    uint16_t type;
    Bool visible;
    Bool render_regardless_of_controller_index;
    Bool pad_1;
    Bool pauses_game_time; 
    Bool deleted;
    uint32_t creation_process_start_time;
    uint32_t ms_to_close;
    uint32_t ms_to_close_fade_time;
    float opacity;
    struct Widget *previous_widget;
    struct Widget *next_widget;
    struct Widget *parent_widget;
    struct Widget *child_widget;
    struct Widget *focused_child;
    const wchar_t *text;
    uint16_t element_index;
    void *elements;
    uint16_t element_count;
    struct Widget *extended_description;
    void *userdata; // no idea about this
    uint32_t pad_2;
    uint16_t bitmap_index;
    uint32_t pad_3;
} Widget; 
_Static_assert(sizeof(Widget) == 0x60);

typedef struct WidgetHistoryEntry {
    Widget *previous_menu;
    Widget *previous_menu_list;
    uint16_t focused_item_index;
    struct WidgetHistoryEntry *previous;
} WidgetHistoryEntry;
_Static_assert(sizeof(WidgetHistoryEntry) == 0x10);

typedef struct WidgetGlobals {
    Widget *root_widget;
    WidgetHistoryEntry *history_top_entry;
    int32_t current_time;
    int32_t popup_display_time;
    int16_t error_message_index;
    int16_t widget_pause_counter;
    float unk_1;
    EnqueuedErrorDescriptor enqueued_errors[1];
    DeferredErrorDescriptor priority_warning;
    DeferredErrorDescriptor deferred_for_cinematic_errors[1];
    void *initialization_thread;
    int16_t demo_error;
    Bool initialized;
    Bool unk_2;
    Bool unk_3;
    Bool unk_4;
    Bool unk_5;
    Bool unk_6;
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
