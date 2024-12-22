#include <windows.h>

#include <ringworld/console/console.h>
#include <ringworld/interface/ui_widget.h>
#include <ringworld/memory/pool.h>
#include <ringworld/math/math.h>
#include <ringworld/tag/definitions/bitmap.h>
#include <ringworld/multiplayer/mode.h>
#include <ringworld/game/game_time.h>

#define UI_WIDGET_MEMORY_POOL_SIZE 0x20000
#define UI_WIDGET_MEMORY_POOL_BLOCKS_NUMBER 0x1000

extern MemoryPool **ui_widget_memory_pool;
extern WidgetGlobals *ui_widget_globals;
extern TagHandle *cursor_bitmap_tag_handle;
extern bool *ui_widgets_unknown_1;
extern bool *is_main_menu;

void ui_widgets_initialize(void) {
    size_t memory_pool_size = UI_WIDGET_MEMORY_POOL_SIZE;
    void *allocated_memory = GlobalAlloc(GMEM_FIXED, memory_pool_size);
    MemoryPool *memory_pool = *ui_widget_memory_pool;
    size_t blocks_number = UI_WIDGET_MEMORY_POOL_BLOCKS_NUMBER;
    memset(memory_pool, 0, sizeof(MemoryPool));
    memset(memory_pool->blocks, 0, blocks_number * 4);
    memory_pool->name = "widget_memory_pool";
    memory_pool->allocated_memory = allocated_memory;
    memory_pool->allocated_size = memory_pool_size;
    memory_pool->blocks_number = blocks_number;

    WidgetGlobals *globals = ui_widget_globals;
    memset(globals, 0, sizeof(WidgetGlobals));
    globals->deferred_error_code = -1;
    globals->priority_warning.error_string = -1;
    globals->enqueued_errors[0].error_string = -1;
    globals->deferred_for_cinematic_errors[0].error_string = -1;
    globals->fade_to_black = -1.0f;
    globals->initialized = allocated_memory != NULL;
}

Widget *ui_widget_load_by_name_or_tag(const char *definition_tag_path, TagHandle definition_tag_handle, Widget *parent, int16_t controller_index, 
                        TagHandle topmost_widget_definition_handle, TagHandle parent_widget_definition_handle, uint16_t child_index_from_parent) {
    
    *cursor_bitmap_tag_handle = lookup_tag("ui\\shell\\bitmaps\\cursor", TAG_GROUP_BITMAP);
    *ui_widgets_unknown_1 = true;

    int16_t player_controller_index = controller_index == -1 ? 0 : controller_index;

    if(HANDLE_IS_NULL(definition_tag_handle)) {
        if(definition_tag_path) {
            definition_tag_handle = lookup_tag(definition_tag_path, TAG_GROUP_UI_WIDGET_DEFINITION);
            if(HANDLE_IS_NULL(definition_tag_handle)) {
                console_printf_debug_err("ui_widget_load_by_name_or_tag(): tag not found: %s", definition_tag_path);
                return NULL;
            }
        }
        else {
            return NULL;
        }
    }

    UIWidgetDefinition *definition = get_tag_data(definition_tag_handle);
    Widget *widget = memory_pool_new_block(*ui_widget_memory_pool, sizeof(Widget));
    if(widget == NULL) {
        console_printf_debug_err("ui_widget_load_by_name_or_tag(): failed to allocate memory for widget");
        return NULL;
    }

    if(parent == NULL) {
        Widget *active_widget = ui_widget_globals->active_widget[player_controller_index];
        int16_t local_player_index;
        if(active_widget != NULL) {
            local_player_index = active_widget->local_player_index;
            ui_widget_delete_recursive(active_widget);
        }
        else {
            local_player_index = -1;
        }
        ui_widget_globals->active_widget[0] = widget;
        
        if(!HANDLE_IS_NULL(topmost_widget_definition_handle)) {
            UIWidgetDefinition *topmost_definition = get_tag_data(topmost_widget_definition_handle);
            if(topmost_definition->flags.dont_push_history == false) {
                WidgetHistoryNode history_node;
                history_node.previous_menu = topmost_widget_definition_handle;
                history_node.previous_menu_list = parent_widget_definition_handle;
                history_node.focused_item_index = child_index_from_parent;
                history_node.local_player_index = local_player_index;
                ui_widget_new_history_node(&history_node, &ui_widget_globals->history_top_node[player_controller_index]);
            }
        }
    }

    if(controller_index == -1) {
        switch(definition->controller_index) {
            case UI_CONTROLLER_INDEX_PLAYER_1:
                controller_index = 0;
                break;
            case UI_CONTROLLER_INDEX_PLAYER_2:
                controller_index = 1;
                break;
            case UI_CONTROLLER_INDEX_PLAYER_3:
                controller_index = 2;
                break;
            case UI_CONTROLLER_INDEX_PLAYER_4:
                controller_index = 3;
                break;
            case UI_CONTROLLER_INDEX_ANY_PLAYER:
                controller_index = -1;
                break;
        }
    }

    ui_widget_new_instance(controller_index, definition, widget, definition_tag_handle, parent);

    return widget;
}

void ui_widget_new_instance(int16_t controller_index, UIWidgetDefinition *definition_tag_data, Widget *widget, TagHandle definition_tag_handle, Widget *parent) {
    memset(widget, 0, sizeof(Widget));
    widget->local_player_index = controller_index;
    widget->definition_tag_handle = definition_tag_handle;
    widget->name = definition_tag_data->name.string;
    widget->type = definition_tag_data->widget_type;
    widget->visible = true;
    widget->render_regardless_of_controller_index = definition_tag_data->flags.render_regardless_of_controller_index;
    widget->pauses_game_time = definition_tag_data->flags.pause_game_time;
    widget->creation_process_start_time = ui_widget_globals->current_time_ms;
    widget->ms_to_close = max_i32(definition_tag_data->milliseconds_to_auto_close, 0);
    widget->ms_to_close_fade_time = max_i32(definition_tag_data->milliseconds_auto_close_fade_time, 0);
    widget->alpha_modifier = 1.0;
    widget->parent = parent;
    if(widget->type == UI_WIDGET_TYPE_TEXT_BOX) {
        widget->text_box_parameters.string_list_index = -1;
        widget->text_box_parameters.flashing = false;
    }

    if(!HANDLE_IS_NULL(definition_tag_data->background_bitmap.tag_handle)) {
        Bitmap *background_bitmap = get_tag_data(definition_tag_data->background_bitmap.tag_handle);
        widget->animation_data.number_of_sprite_frames = background_bitmap->bitmap_group_sequence.elements[0].bitmap_count;
    }
    
    if(ui_widget_globals->dont_load_children_recursive == false) {
        ui_widget_load_children_recursive(definition_tag_data, widget);
    }
    
    if(definition_tag_data->event_handlers.count > 0) {
        for(size_t i = 0; i < definition_tag_data->event_handlers.count; i++) {
            EventHandlerReference *event_handler = &definition_tag_data->event_handlers.elements[i];
            if(event_handler->event_type == UI_EVENT_TYPE_CREATED) {
                UIWidgetEventRecord event_record;
                event_record.type = 0;
                event_record.controller_index = widget->local_player_index;
                event_record.unk1 = 0;
                ui_widget_event_handler_dispatch(widget, definition_tag_data, &event_record, event_handler, &controller_index);
            }
        }
    }

    if(widget->focused_child == NULL) {
        for(Widget *child = widget->child; child != NULL; child = child->next) {
            UIWidgetDefinition *child_definition = get_tag_data(child->definition_tag_handle);
            if(child->visible == false) {
                if(child_definition->event_handlers.count > 0 || child->type == UI_WIDGET_TYPE_SPINNER_LIST || child->type == UI_WIDGET_TYPE_COLUMN_LIST) {
                    ui_widget_instance_give_focus_directly(widget, child);
                }
            }
        }
    } 

    MultiplayerMode mp_mode = get_multiplayer_mode();
    if(widget->render_regardless_of_controller_index == true && mp_mode != MULTIPLAYER_MODE_HOST) {
        ui_widget_globals->pause_time_count++;
        bool game_paused = game_time_get_paused();
        if(is_main_menu == false && game_paused == false) {
            game_time_set_paused(true);
        }
    }
}
