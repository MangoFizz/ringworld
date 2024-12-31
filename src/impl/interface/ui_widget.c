#include <windows.h>

#include "../console/console.h"
#include "../memory/pool.h"
#include "../math/math.h"
#include "../tag/definitions/bitmap.h"
#include "../tag/definitions/unicode_string_list.h"
#include "../multiplayer/mode.h"
#include "../game/game_time.h"
#include "../exception/exception.h"
#include "ui_widget.h"

#define UI_WIDGET_MEMORY_POOL_SIZE 0x20000
#define UI_WIDGET_MEMORY_POOL_BLOCKS_NUMBER 0x1000

extern MemoryPool **ui_widget_memory_pool;
extern WidgetGlobals *ui_widget_globals;
extern TagHandle *cursor_bitmap_tag_handle;
extern bool *ui_widgets_unknown_1;
extern bool *is_main_menu;

WidgetGlobals *get_ui_widget_globals(void) {
    return ui_widget_globals;
}

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

Widget *ui_widget_load_by_name_or_tag(const char *definition_tag_path, TagHandle definition_tag, Widget *parent, int16_t controller_index, 
                        TagHandle topmost_widget_definition_handle, TagHandle parent_widget_definition_handle, uint16_t child_index_from_parent) {
    
    *cursor_bitmap_tag_handle = lookup_tag("ui\\shell\\bitmaps\\cursor", TAG_GROUP_BITMAP);
    *ui_widgets_unknown_1 = true;

    int16_t player_controller_index = controller_index == -1 ? 0 : controller_index;

    if(HANDLE_IS_NULL(definition_tag)) {
        if(definition_tag_path) {
            definition_tag = lookup_tag(definition_tag_path, TAG_GROUP_UI_WIDGET_DEFINITION);
            if(HANDLE_IS_NULL(definition_tag)) {
                CRASHF_DEBUG("failed to find widget definition tag");                
            }
        }
        else {
            return NULL;
        }
    }

    UIWidgetDefinition *definition = tag_get_data(TAG_GROUP_UI_WIDGET_DEFINITION, definition_tag);
    Widget *widget = memory_pool_new_block(*ui_widget_memory_pool, sizeof(Widget));
    if(widget == NULL) {
        CRASHF_DEBUG("failed to allocate memory for widget");
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
            UIWidgetDefinition *topmost_definition = tag_get_data(TAG_GROUP_UI_WIDGET_DEFINITION, topmost_widget_definition_handle);
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

    ui_widget_new_instance(controller_index, definition, widget, definition_tag, parent);

    return widget;
}

void ui_widget_new_instance(int16_t controller_index, UIWidgetDefinition *widget_definition, Widget *widget, TagHandle definition_tag_handle, Widget *parent) {
    memset(widget, 0, sizeof(Widget));
    widget->local_player_index = controller_index;
    widget->definition_tag_handle = definition_tag_handle;
    widget->name = widget_definition->name.string;
    widget->type = widget_definition->widget_type;
    widget->visible = true;
    widget->render_regardless_of_controller_index = widget_definition->flags.render_regardless_of_controller_index;
    widget->pauses_game_time = widget_definition->flags.pause_game_time;
    widget->creation_process_start_time = ui_widget_globals->current_time_ms;
    widget->ms_to_close = max_i32(widget_definition->milliseconds_to_auto_close, 0);
    widget->ms_to_close_fade_time = max_i32(widget_definition->milliseconds_auto_close_fade_time, 0);
    widget->alpha_modifier = 1.0;
    widget->parent = parent;
    if(widget->type == UI_WIDGET_TYPE_TEXT_BOX) {
        widget->text_box_parameters.string_list_index = -1;
        widget->text_box_parameters.flashing = false;
    }

    if(!HANDLE_IS_NULL(widget_definition->background_bitmap.tag_handle)) {
        Bitmap *background_bitmap = tag_get_data(TAG_GROUP_BITMAP, widget_definition->background_bitmap.tag_handle);
        widget->animation_data.number_of_sprite_frames = background_bitmap->bitmap_group_sequence.elements[0].bitmap_count;
    }
    
    if(ui_widget_globals->dont_load_children_recursive == false) {
        if(!ui_widget_load_children_recursive(widget_definition, widget)) {
            CRASHF_DEBUG("failed to load children for widget");
        }
    }
    
    if(widget_definition->event_handlers.count > 0) {
        for(size_t i = 0; i < widget_definition->event_handlers.count; i++) {
            EventHandlerReference *event_handler = &widget_definition->event_handlers.elements[i];
            if(event_handler->event_type == UI_EVENT_TYPE_CREATED) {
                UIWidgetEventRecord event_record;
                event_record.type = 0;
                event_record.controller_index = widget->local_player_index;
                event_record.unk1 = 0;
                ui_widget_event_handler_dispatch(widget, widget_definition, &event_record, event_handler, &controller_index);
            }
        }
    }

    if(widget->focused_child == NULL) {
        for(Widget *child = widget->child; child != NULL; child = child->next) {
            UIWidgetDefinition *child_definition = tag_get_data(TAG_GROUP_UI_WIDGET_DEFINITION, child->definition_tag_handle);
            if(child->never_receive_events == false && child_definition->event_handlers.count > 0 || ui_widget_is_list(child)) {
                ui_widget_instance_give_focus_directly(widget, child);
            }
        }
    } 

    if(widget->render_regardless_of_controller_index == true && get_multiplayer_mode() != MULTIPLAYER_MODE_HOST) {
        ui_widget_globals->pause_time_count++;
        bool game_paused = game_time_get_paused();
        if(is_main_menu == false && game_paused == false) {
            game_time_set_paused(true);
        }
    }
}

Widget *ui_widget_get_last_child(Widget *widget) {
    Widget *last_child = widget->child;
    while(last_child && last_child->next != NULL) {
        last_child = last_child->next;
    }
    return last_child;
}

bool ui_widget_is_list(Widget *widget) {
    return widget->type == UI_WIDGET_TYPE_COLUMN_LIST || widget->type == UI_WIDGET_TYPE_SPINNER_LIST;
}

bool ui_widget_load_children_recursive(UIWidgetDefinition *widget_definition, Widget *widget) {
    bool result = true;
    
    if(widget_definition->flags_2.list_items_from_string_list_tag) {
        UnicodeStringList *string_list = tag_get_data(TAG_GROUP_UNICODE_STRING_LIST, widget_definition->text_label_unicode_strings_list.tag_handle);
        ui_widget_globals->dont_load_children_recursive = true;
        for(size_t i = 0; i < string_list->strings.count; i++) {
            Widget *child = ui_widget_load_by_name_or_tag(NULL, widget->definition_tag_handle, widget, widget->local_player_index, NULL_HANDLE, NULL_HANDLE, -1);
            if(child == NULL) {
                result = false;
                break;
            }

            Widget *last_child = ui_widget_get_last_child(widget);
            if(last_child != NULL) {
                last_child->next = child;
                child->previous = last_child;
            }
            else {
                widget->child = child;
            }
            
            widget->list_parameters.number_of_items++;
        }
        ui_widget_globals->dont_load_children_recursive = false;
    }

    size_t children_count = widget_definition->child_widgets.count;
    if(children_count > 0) {
        for(size_t i = 0; i < children_count; i++) {
            ChildWidgetReference *child_ref = &widget_definition->child_widgets.elements[i];
            int16_t controller_index = widget->local_player_index;
            if(child_ref->flags.use_custom_controller_index) {
                if(child_ref->custom_controller_index > -1 && child_ref->custom_controller_index < 4) {
                    controller_index = child_ref->custom_controller_index;
                }
            }
            
            if(!HANDLE_IS_NULL(child_ref->widget_tag.tag_handle)) {
                Widget *child = ui_widget_load_by_name_or_tag(NULL, child_ref->widget_tag.tag_handle, widget, controller_index, NULL_HANDLE, NULL_HANDLE, -1);
                if(child == NULL) {
                    result = false;
                    break;
                }

                child->position.x = child_ref->horizontal_offset + widget->position.x;
                child->position.y = child_ref->vertical_offset + widget->position.y;
                Widget *last_child = ui_widget_get_last_child(widget);
                if(last_child != NULL) {
                    last_child->next = child;
                    child->previous = last_child;
                }
                else {
                    widget->child = child;
                }
            }
        }
    }

    if(ui_widget_is_list(widget)) {
        if(!HANDLE_IS_NULL(widget_definition->extended_description_widget.tag_handle)) {
            Widget *extended_description = ui_widget_load_by_name_or_tag(NULL, widget_definition->extended_description_widget.tag_handle, widget, widget->local_player_index, NULL_HANDLE, NULL_HANDLE, -1);
            widget->list_parameters.extended_description = extended_description;
            if(extended_description != NULL) {
                if(extended_description->previous != NULL) {
                    extended_description->previous->next = extended_description;
                }
                extended_description->next = NULL;
                extended_description->parent = NULL;
            }
        }
    }

    if(widget_definition->flags.dont_focus_a_specific_child_widget == false) {
        if(ui_widget_is_list(widget)) {
            widget->list_parameters.current_list_item_index = 0;
            widget->list_parameters.last_list_tab_direction = 0;
            widget->list_parameters.spin_activated = 0;
        }
        else if(widget_definition->flags.pass_unhandled_events_to_focused_child == false) {
            return result;
        }

        Widget *child = widget->child;
        if(child != NULL) {
            UIWidgetDefinition *child_definition = tag_get_data(TAG_GROUP_UI_WIDGET_DEFINITION, child->definition_tag_handle);
            while(!ui_widget_is_list(widget) && (widget->child->never_receive_events || 
                  (child_definition->event_handlers.count == 0 && !ui_widget_is_list(child)))) {
                
                child = child->next;
                if(child == NULL) {
                    return result;
                }
            }
            widget->focused_child = child;
        }
    }

    return result;
}

Widget *ui_widget_get_topmost_parent(Widget *widget) {
    Widget *parent = widget;
    for(Widget *aux = widget->parent; aux != NULL; aux = aux->parent) {
        parent = aux;
    }
    return parent;
}

void ui_widget_instance_give_focus_directly(Widget *widget, Widget *child) {
    Widget *topmost_parent = ui_widget_get_topmost_parent(widget);
    Widget *focused = topmost_parent->focused_child;

    // If the child is set to never receive events, find the next/previous child that can receive events
    if(child->never_receive_events) {
        Widget *aux;
        for(aux = child->next; aux != NULL; aux = aux->next) {
            UIWidgetDefinition *definition = tag_get_data(TAG_GROUP_UI_WIDGET_DEFINITION, aux->definition_tag_handle);
            if(aux->never_receive_events == false) {
                if(definition->event_handlers.count > 0 || ui_widget_is_list(aux)) {
                    child = aux;
                    break;
                }
            }
        }
        if(child != aux) {
            Widget *first_child = child->parent;
            if(first_child != NULL) {
                for(aux = first_child->child; aux != NULL; aux = aux->next) {
                    UIWidgetDefinition *definition = tag_get_data(TAG_GROUP_UI_WIDGET_DEFINITION, aux->definition_tag_handle);
                    if(aux->never_receive_events == false && definition->event_handlers.count > 0 || ui_widget_is_list(aux)) {
                        break;
                    }
                }
                if(aux == first_child->focused_child) {
                    for(aux = child->previous; aux != NULL; aux = aux->previous) {
                        UIWidgetDefinition *definition = tag_get_data(TAG_GROUP_UI_WIDGET_DEFINITION, aux->definition_tag_handle);
                        if(aux->never_receive_events == false && definition->event_handlers.count > 0 || ui_widget_is_list(aux)) {
                            if(aux != NULL) {
                                child = aux;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    if(focused != NULL) {
        if(child != NULL) {
            if(focused->parent == child->parent && focused->parent != NULL) {
                child->parent->focused_child = child;
                return;
            }
        }
        while(focused != NULL) {
            focused->parent->focused_child = NULL;
            focused = focused->focused_child;
        }
    }
    
    Widget *aux = child->parent;
    while(aux != NULL) {
        child->parent->focused_child = child;
        child = child->parent;
        aux = child->parent;
    }
}

void ui_widget_new_history_node(WidgetHistoryNode *history_node_data, WidgetHistoryNode **history_top_node) {
    WidgetHistoryNode *new_node = memory_pool_new_block(*ui_widget_memory_pool, sizeof(WidgetHistoryNode));
    if(new_node == NULL) {
        CRASHF_DEBUG("failed to allocate memory for history node");
    }
    new_node->previous_menu = history_node_data->previous_menu;
    new_node->previous_menu_list = history_node_data->previous_menu_list;
    new_node->focused_item_index = history_node_data->focused_item_index;
    new_node->local_player_index = history_node_data->local_player_index;
    new_node->previous = *history_top_node;
    *history_top_node = new_node;
}


