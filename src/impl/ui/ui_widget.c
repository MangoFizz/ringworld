#include <windows.h>

#include <ringworld/console/console.h>
#include <ringworld/ui/ui_widget.h>
#include <ringworld/memory/pool.h>

#define UI_WIDGET_MEMORY_POOL_SIZE 0x20000
#define UI_WIDGET_MEMORY_POOL_BLOCKS_NUMBER 0x1000

extern MemoryPool **ui_widget_memory_pool;
extern WidgetGlobals *ui_widget_globals;
extern TagHandle *cursor_bitmap_tag_handle;
extern bool *ui_widgets_unknown_1;

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

    printf("ui_widget_load_by_name_or_tag(): widget: %s\n", definition->name.string);

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
            case PLAYER_1:
                controller_index = 0;
                break;
            case PLAYER_2:
                controller_index = 1;
                break;
            case PLAYER_3:
                controller_index = 2;
                break;
            case PLAYER_4:
                controller_index = 3;
                break;
            case ANY_PLAYER:
                controller_index = -1;
                break;
        }
    }

    ui_widget_new_instance(controller_index, definition, widget, definition_tag_handle, parent);

    return widget;
}
