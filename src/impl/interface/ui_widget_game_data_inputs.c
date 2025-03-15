#include "../exception/exception.h"
#include "../saved_games/player_profile.h"
#include "ui_widget.h"

void ui_widget_update_multiplayer_type_menu_extended_description(Widget *widget) {
    ASSERT(widget != NULL);

    ASSERT_OR_RETURN(widget->focused_child != NULL);

    Widget *extended_desc = widget->list_parameters.extended_description;
    ASSERT_OR_RETURN(extended_desc != NULL);

    Widget *extended_desc_pic = extended_desc->child;
    ASSERT_OR_RETURN(extended_desc_pic != NULL);

    Widget *extended_desc_text = extended_desc_pic->next;
    ASSERT_OR_RETURN(extended_desc_text != NULL);

    Widget *extended_desc_profile_name = extended_desc_text->next;
    ASSERT_OR_RETURN(extended_desc_profile_name != NULL);

    int16_t widget_index = ui_widget_get_index_for_child(widget, widget->focused_child);
    if(widget_index != -1) {
        switch(widget_index) {
            case 1:
            case 2:
            case 3:
                extended_desc_pic->animation_data.current_frame_index = 0;
                extended_desc_text->text_box_parameters.string_list_index = widget_index - 1;
                break;
            case 5:
            case 6:
                extended_desc_pic->animation_data.current_frame_index = 1; // this was originally set to 0
                extended_desc_text->text_box_parameters.string_list_index = widget_index - 2;
                break;
            case 7:
                extended_desc_pic->animation_data.current_frame_index = 2;
                extended_desc_text->text_box_parameters.string_list_index = widget_index - 2;
                break;
        }
    }

    // Disable "JOIN GAME" and "CREATE GAME" label widgets
    Widget *child = widget->child;
    for(size_t i = 0; child != NULL && i <= 4; i++) {
        switch(i) {
            case 0:
            case 4:
                child->never_receive_events = true;
                if(child == widget->focused_child && child->next != NULL) {
                    widget->focused_child = child->next;
                }
                break;
        }
        child = child->next;
    }

    // Update profile name widget
    const PlayerProfile *player_profile = saved_games_get_player_profile(widget->local_player_index == -1 ? 0 : widget->local_player_index);
    ASSERT(player_profile != NULL);
    ui_widget_update_player_profile_text(player_profile->player_details.player_name, extended_desc_profile_name);
}
