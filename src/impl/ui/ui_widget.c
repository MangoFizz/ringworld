#include <windows.h>

#include <ringworld/ui/ui_widget.h>

#define UI_WIDGET_MEMORY_POOL_SIZE 0x20000

extern WidgetMemoryPool **ui_widget_memory_pool;
extern WidgetGlobals *ui_widget_globals;

void ui_widgets_initialize(void) {
    HGLOBAL allocated_memory = GlobalAlloc(GMEM_FIXED, UI_WIDGET_MEMORY_POOL_SIZE);
    WidgetMemoryPool *memory_pool = *ui_widget_memory_pool;
    if(allocated_memory != NULL) {
        memory_pool->allocated_memory = allocated_memory;
        memory_pool->allocated_size = UI_WIDGET_MEMORY_POOL_SIZE;
    }
    memset(memory_pool->unk_2, 0, sizeof(memory_pool->unk_2));

    void *ui_widget_memory_pool_unk_1 = (void *)(memory_pool + 1);
    memset(ui_widget_memory_pool_unk_1, 0, memory_pool->unk_1 * 4);

    WidgetGlobals *globals = ui_widget_globals;
    memset(globals, 0, sizeof(WidgetGlobals));
    globals->error_message_index = -1;
    globals->priority_warning.error_string = -1;
    globals->enqueued_errors[0].error_string = -1;
    globals->deferred_for_cinematic_errors[0].error_string = -1;
    globals->unk_1 = -1.0f;
    globals->initialized = allocated_memory != NULL;
}
