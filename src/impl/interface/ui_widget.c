#include <windows.h>
#include <math.h>
#include "../../event/events.h"
#include "../bitmap/bitmap.h"
#include "../console/console.h"
#include "../memory/pool.h"
#include "../math/math.h"
#include "../math/color.h"
#include "../tag/definitions/bitmap.h"
#include "../tag/definitions/unicode_string_list.h"
#include "../multiplayer/mode.h"
#include "../game/game_time.h"
#include "../text/text.h"
#include "../exception/exception.h"
#include "../rasterizer/rasterizer_screen.h"
#include "../rasterizer/rasterizer_screen_geometry.h"
#include "../rasterizer/rasterizer_text.h"
#include "ui_cursor.h"
#include "ui_virtual_keyboard.h"
#include "ui_widget.h"

#define UI_WIDGET_MEMORY_POOL_SIZE 0x20000
#define UI_WIDGET_MEMORY_POOL_BLOCKS_NUMBER 0x1000

extern MemoryPool **ui_widget_memory_pool;
extern WidgetGlobals *widget_globals;
extern bool *ui_widgets_unknown_1;
extern bool *is_main_menu;
extern bool *local_player_index_for_draw_string_and_hack_in_icons;
extern void (**ui_widget_game_data_input_functions)(Widget *);

WidgetGlobals *get_ui_widget_globals(void) {
    return widget_globals;
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

    WidgetGlobals *globals = widget_globals;
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
    
    TagHandle cursor_tag = lookup_tag("ui\\shell\\bitmaps\\cursor", TAG_GROUP_BITMAP);
    ui_cursor_set_bitmap_tag_handle(cursor_tag);
    *ui_widgets_unknown_1 = true;

    int16_t player_controller_index = controller_index == -1 ? 0 : controller_index;

    if(HANDLE_IS_NULL(definition_tag)) {
        if(definition_tag_path) {
            definition_tag = lookup_tag(definition_tag_path, TAG_GROUP_UI_WIDGET_DEFINITION);
            if(HANDLE_IS_NULL(definition_tag)) {
                return NULL;
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
        Widget *active_widget = widget_globals->active_widget[player_controller_index];
        int16_t local_player_index;
        if(active_widget != NULL) {
            local_player_index = active_widget->local_player_index;
            ui_widget_delete_recursive(active_widget);
        }
        else {
            local_player_index = -1;
        }
        widget_globals->active_widget[0] = widget;
        
        if(!HANDLE_IS_NULL(topmost_widget_definition_handle)) {
            UIWidgetDefinition *topmost_definition = tag_get_data(TAG_GROUP_UI_WIDGET_DEFINITION, topmost_widget_definition_handle);
            if(topmost_definition->flags.dont_push_history == false) {
                WidgetHistoryNode history_node;
                history_node.previous_menu = topmost_widget_definition_handle;
                history_node.previous_menu_list = parent_widget_definition_handle;
                history_node.focused_item_index = child_index_from_parent;
                history_node.local_player_index = local_player_index;
                ui_widget_new_history_node(&history_node, &widget_globals->history_top_node[player_controller_index]);
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

    ringworld_event_trigger(RW_EVENT_WIDGET_LOADED, widget);

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
    widget->creation_process_start_time = widget_globals->current_time_ms;
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
    
    if(widget_globals->dont_load_children_recursive == false) {
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

    if(widget->pauses_game_time == true && get_multiplayer_mode() != MULTIPLAYER_MODE_HOST) {
        widget_globals->pause_time_count++;
        bool game_paused = game_time_get_paused();
        if(*is_main_menu == false && game_paused == false) {
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
        widget_globals->dont_load_children_recursive = true;
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
        widget_globals->dont_load_children_recursive = false;
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

float ui_widget_get_widescreen_margin(void) {
    if(rasterizer_screen_widescreen_support_enabled()) {
        Widget *active_widget = widget_globals->active_widget[0];
        if(active_widget != NULL) {
            UIWidgetDefinition *definition = tag_get_data(TAG_GROUP_UI_WIDGET_DEFINITION, active_widget->definition_tag_handle);
            uint16_t widget_width = max_i32(definition->bounds.right - max_i32(definition->bounds.left, 0), RASTERIZER_SCREEN_BASE_WIDTH);
            uint16_t screen_width = rasterizer_screen_get_width();
            return (float)(screen_width - widget_width) / 2.0f;
        }
    }
    return 0.0f;
}

VectorXYInt ui_widget_get_relative_cursor_position(void) {
    VectorXYInt cursor_position = ui_cursor_get_position();
    float widescreen_margin = ui_widget_get_widescreen_margin();
    VectorXYInt relative_cursor_position = { cursor_position.x - widescreen_margin, cursor_position.y };
    return relative_cursor_position;
}

#include <stdio.h>

void ui_widget_render_root_widget(Widget *widget) {
    ASSERT(widget != NULL);
    uint16_t screen_width = rasterizer_screen_get_width();
    uint16_t screen_height = rasterizer_screen_get_height();
    Rectangle2D bounds;
    bounds.left = 0;
    bounds.top = 0;
    bounds.right = screen_width;
    bounds.bottom = screen_height;
    float widescreen_margin = ui_widget_get_widescreen_margin();
    VectorXYInt offset = { widescreen_margin, 0 };
    ui_widget_instance_render_recursive(widget, &bounds, offset, true, false);
    
    if(widget_globals->debug_show_path) {
        math_rectangle_2d_translate(&bounds, 32, 32);
        char *path = tag_get_path(widget->definition_tag_handle);
        ColorARGB color = { 1.0f, 1.0f, 1.0f, 1.0f };
        ColorARGBInt color_mask = color_encode_a8r8g8b8(&color);
        TagHandle font_tag = lookup_tag("ui\\small_ui", TAG_GROUP_FONT);
        text_set_drawing_parameters(-1, 0, 0, font_tag, &color);
        rasterizer_draw_string(&bounds, &bounds, &color_mask, 0, path);
    }
}

void ui_widget_render(int16_t local_player_index) {    
    VirtualKeyboardGlobals *virtual_keyboard_globals = ui_virtual_keyboard_get_globals();

    *local_player_index_for_draw_string_and_hack_in_icons = local_player_index == -1 ? 0 : local_player_index;

    if(virtual_keyboard_globals->active == false) {
        // @todo rewrite this to support multiple local players
        int16_t player_index = 0;
        if(local_player_index == -1) {
            player_index = -1;
        }

        bool widget_rendered = false;
        for(size_t i = 0; i < MAX_LOCAL_PLAYERS; i++) {
            Widget *active_widget = widget_globals->active_widget[i];
            if(active_widget != NULL) {
                if(active_widget->render_regardless_of_controller_index) {
                    ui_widget_render_root_widget(active_widget);
                    widget_rendered = true;
                }
                else {
                    int16_t active_widget_player_index = active_widget->local_player_index;
                    if(active_widget->is_error_dialog) {
                        if(active_widget_player_index == player_index || active_widget_player_index == -1 || player_index == -1 || *is_main_menu) {
                            ui_widget_render_root_widget(active_widget);
                            widget_rendered = true;
                        }
                    }
                    else if((active_widget_player_index == -1 && i == 0) || active_widget_player_index == player_index) {
                        ui_widget_render_root_widget(active_widget);
                        widget_rendered = true;
                    }
                }
            }
        }

        if(widget_rendered) {
            ui_cursor_render();
        }

        if(widget_globals->fade_to_black >= 0.0f && widget_globals->fade_to_black <= 1.0f) {
            Rectangle2D bounds;
            bounds.left = 0;
            bounds.top = 0;
            bounds.right = rasterizer_screen_get_width();
            bounds.bottom = rasterizer_screen_get_height();
            if(widget_globals->fade_to_black >= 0.95f) {
                widget_globals->fade_to_black = 1.0f;
            }
            uint8_t alpha = clamp_u8(widget_globals->fade_to_black * 255, 0, 255);
            rasterizer_screen_geometry_draw_quad(&bounds, alpha << 24);
        }
    }
    else {
        ui_virtual_keyboard_render();
        ui_cursor_render();
    }
}

static bool ui_widget_background_is_excluded_from_widescreen(TagHandle tag) {
    static const char *exclude_list[] = {
        "ui\\shell\\main_menu\\halo_logo",
        "ui\\shell\\main_menu\\multiplayer_type_select\\join_game\\join_game_items_list",
        "ui\\shell\\main_menu\\settings_select\\player_setup\\player_profile_edit\\controls_setup\\controls_options_menu",
        "ui\\shell\\main_menu\\settings_select\\player_setup\\player_profile_edit\\gamepad_setup\\gamepad_setup_options"
    };

    TagDataHeader *tag_data = tag_get_data_header();
    TagEntry *entry = &tag_data->tags[tag.index];
    for(size_t i = 0; i < sizeof(exclude_list) / sizeof(exclude_list[0]); i++) {
        if(strcmp(entry->path, exclude_list[i]) == 0) {
            return true;
        }
    }
    return false;
}

float ui_widget_get_cumulative_alpha_modifier(Widget *widget) {
    ASSERT(widget != NULL);
    float alpha_modifier = widget->alpha_modifier;
    Widget *current_widget = widget->parent;
    while(current_widget != NULL) {
        alpha_modifier *= current_widget->alpha_modifier;
        current_widget = current_widget->parent;
    }
    return alpha_modifier;
}

void ui_widget_instance_render_recursive(Widget *widget, Rectangle2D *bounds, VectorXYInt offset, bool is_focused, bool use_nifty_render_fx) {
    ASSERT(widget != NULL);    

    UIWidgetDefinition *definition = tag_get_data(TAG_GROUP_UI_WIDGET_DEFINITION, widget->definition_tag_handle);

    for(size_t i = 0; i < definition->game_data_inputs.count; i++) {
        GameDataInputReference *game_data_input = &definition->game_data_inputs.elements[i];
        if(game_data_input->function > UI_GAME_DATA_INPUT_REFERENCE_FUNCTION_NULL) {
            if(game_data_input->function < UI_GAME_DATA_INPUT_REFERENCE_FUNCTION_MAX) {
                ui_widget_game_data_input_functions[game_data_input->function](widget);
            }
        }
    }

    if(!widget->visible) {
        return;
    }

    float alpha_modifier = ui_widget_get_cumulative_alpha_modifier(widget);

    // The plasma effect that was used on menus in the original Xbox release
    bool should_use_nifty_render_fx = use_nifty_render_fx || definition->flags.always_use_nifty_render_fx;

    // Update offset
    offset.x += widget->position.x;
    offset.y += widget->position.y;

    if(!HANDLE_IS_NULL(definition->background_bitmap.tag_handle)) {
        TagHandle background_bitmap_tag = definition->background_bitmap.tag_handle;
        uint16_t background_frame_index = widget->animation_data.current_frame_index;
        BitmapData *background_bitmap = bitmap_group_sequence_get_bitmap_for_frame(background_bitmap_tag, 0, background_frame_index);
        if(background_bitmap != NULL) {
            Rectangle2D screen_rect, texture_rect, parent_rect;
            screen_rect.left = definition->bounds.left + offset.x;
            screen_rect.top = definition->bounds.top + offset.y;
            screen_rect.right = definition->bounds.right + offset.x;
            screen_rect.bottom = definition->bounds.bottom + offset.y;
            texture_rect = screen_rect;
            
            Rectangle2D *bounds_pointer = NULL;
            if(bounds != NULL) {
                parent_rect.left = bounds->left + offset.x;
                parent_rect.top = bounds->top + offset.y;
                parent_rect.right = bounds->right + offset.x;
                parent_rect.bottom = bounds->bottom + offset.y;
                bounds_pointer = &parent_rect;
            }
            
            if(rasterizer_screen_widescreen_support_enabled() && !ui_widget_background_is_excluded_from_widescreen(widget->definition_tag_handle)) {
                uint16_t screen_width = rasterizer_screen_get_width();
                float widescreen_margin = ui_widget_get_widescreen_margin();
                uint16_t widgets_bounds_width = screen_width - widescreen_margin * 2;
                uint16_t widget_width = min_i32(definition->bounds.right, widgets_bounds_width) - max_i32(definition->bounds.left, 0);

                if(abs(definition->bounds.left) == definition->bounds.right) {
                    widget_width = definition->bounds.right;
                }

                if(screen_width > widgets_bounds_width && widgets_bounds_width == widget_width) {
                    if(bounds_pointer) {
                        bounds_pointer->left = 0;
                        bounds_pointer->right = screen_width;
                    }
                    screen_rect.left = 0;
                    screen_rect.right = screen_width;
                }
            }

            float alpha = alpha_modifier;
            if(definition->flags.flash_background_bitmap) {
                double time = widget_globals->current_time_ms;
                if(time < 0) {
                    time += UINT32_MAX;
                }
                alpha = (cos(time * 0.003) + 1.0f) * 0.5f * alpha_modifier;
            }

            alpha = clamp_f32(alpha, 0.0f, 1.0f);
            uint32_t color = (uint8_t)round(alpha * 255.0f) << 24 | 0xFFFFFF;

            bitmap_draw_in_rect(background_bitmap, bounds_pointer, color, &screen_rect, &texture_rect);
        }
    }

    bool do_not_render_children = false;
    switch(widget->type) {
        case UI_WIDGET_TYPE_TEXT_BOX: {
            if(!definition->flags_1.dont_do_that_weird_focus_test) {
                is_focused = ui_widget_text_box_is_focused(widget);
            }
            ui_widget_render_text_box(widget, definition, bounds, offset, is_focused);
            break;
        }
        case UI_WIDGET_TYPE_SPINNER_LIST: {
            ui_widget_render_spinner_list(widget, definition, bounds, offset, is_focused);
            if(definition->flags_2.list_items_from_string_list_tag && definition->child_widgets.count == 0) {
                do_not_render_children = true;
            }
            break;
        }
        case UI_WIDGET_TYPE_COLUMN_LIST: {
            ui_widget_render_column_list(widget, definition, bounds, offset, is_focused);
            if(definition->flags_2.list_items_generated_in_code) {
                do_not_render_children = true;
            }
            break;
        }
    }

    if(!do_not_render_children) {
        for(Widget *current_widget = widget->child; current_widget != NULL; current_widget = current_widget->next) {
            bool child_is_focused = current_widget == widget->focused_child;
            if(child_is_focused && ui_widget_is_list(widget)) {
                should_use_nifty_render_fx = true;
            }
            else {
                should_use_nifty_render_fx = false;
            }
            ui_widget_instance_render_recursive(current_widget, bounds, offset, child_is_focused, should_use_nifty_render_fx);
        }
    }

    for(size_t i = 0; i < definition->event_handlers.count; i++) {
        EventHandlerReference *event_handler = &definition->event_handlers.elements[i];
        if(event_handler->event_type == UI_EVENT_TYPE_POST_RENDER) {
            UIWidgetEventRecord event_record;
            event_record.controller_index = widget->local_player_index;
            event_record.type = 0;
            event_record.unk1 = 0;
            int16_t controller_index;
            ui_widget_event_handler_dispatch(widget, definition, &event_record, event_handler, &controller_index);
        }
    }
}

/**
 * @todo Fix hooks when first parameters are registers instead of stack variables
 */
void ui_widget_render_column_list_og(UIWidgetDefinition *definition, Rectangle2D *bounds, VectorXYInt offset, bool is_focused, Widget *widget);
void ui_widget_render_column_list(Widget *widget, UIWidgetDefinition *definition, Rectangle2D *bounds, VectorXYInt offset, bool is_focused) {
    ui_widget_render_column_list_og(definition, bounds, offset, is_focused, widget);
}

void ui_widget_adjust_spinner_list_bounds(Widget *widget, Rectangle2D *bounds) {
    UIWidgetDefinition *definition = tag_get_data(TAG_GROUP_UI_WIDGET_DEFINITION, widget->definition_tag_handle);
    bool is_spinner_list = widget->type == UI_WIDGET_TYPE_SPINNER_LIST;
    TagHandle list_header_bitmap = definition->list_header_bitmap.tag_handle;
    TagHandle list_footer_bitmap = definition->list_footer_bitmap.tag_handle;
    if(is_spinner_list && definition->child_widgets.count < 2 && !HANDLE_IS_NULL(list_footer_bitmap) && !HANDLE_IS_NULL(list_header_bitmap)) {
        bounds->left = bounds->left + definition->header_bounds.left - 10;
        uint16_t right = definition->footer_bounds.right;
        if(bounds->right <= right) {
            bounds->right = right + 2;
        }
    }
}

Widget *ui_widget_find_cursor_focused_widget(Widget *widget, int cursor_x, int cursor_y, VectorXYInt offset) {
    ASSERT(widget != NULL);
    UIWidgetDefinition *definition = tag_get_data(TAG_GROUP_UI_WIDGET_DEFINITION, widget->definition_tag_handle);
    Rectangle2D bounds = definition->bounds;
    if(!widget->never_receive_events) {
        bool has_event_handlers = definition->event_handlers.count > 0;
        bool is_list = ui_widget_is_list(widget);
        bool parent_is_null_or_list = widget->parent == NULL || ui_widget_is_list(widget->parent);
        bool force_handle_mouse = definition->flags.force_handle_mouse;
        if(has_event_handlers || is_list || parent_is_null_or_list || force_handle_mouse) {
            offset.x += widget->position.x;
            offset.y += widget->position.y;
            ui_widget_adjust_spinner_list_bounds(widget, &bounds);
            math_rectangle_2d_translate(&bounds, offset.x, offset.y);
            if(math_rectangle_2d_contains_point(&bounds, cursor_x, cursor_y)) {
                Widget *current_child = widget->child;
                Widget *found_widget = NULL;
                while(current_child != NULL && found_widget == NULL) {
                    found_widget = ui_widget_find_cursor_focused_widget(current_child, cursor_x, cursor_y, offset);
                    current_child = current_child->next;
                }
                if(found_widget == NULL && !ui_widget_is_list(widget)) {
                    found_widget = widget;
                }
                return found_widget;
            }
        }
    }
    return NULL;
}

Widget *ui_widget_get_cursor_focused_widget(Widget *widget, int cursor_x, int cursor_y, VectorXYInt offset) {
    VectorXYInt cursor_position = ui_widget_get_relative_cursor_position();
    return ui_widget_find_cursor_focused_widget(widget, cursor_position.x, cursor_position.y, offset);
}

bool ui_widget_is_cursor_over(Widget *widget) {
    ASSERT(widget != NULL);
    VectorXYInt cursor_position = ui_widget_get_relative_cursor_position();
    UIWidgetDefinition *definition = tag_get_data(TAG_GROUP_UI_WIDGET_DEFINITION, widget->definition_tag_handle);
    Rectangle2D bounds = definition->bounds;
    VectorXYInt offset = { 0, 0 };
    Widget *current_widget = widget;
    while(current_widget != NULL) {
        offset.x += current_widget->position.x;
        offset.y += current_widget->position.y;
        current_widget = current_widget->parent;
    }
    ui_widget_adjust_spinner_list_bounds(widget, &bounds);
    math_rectangle_2d_translate(&bounds, offset.x, offset.y);
    bool is_over = math_rectangle_2d_contains_point(&bounds, cursor_position.x, cursor_position.y);
    return is_over;
}
