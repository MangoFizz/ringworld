#include <time.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "../exception/exception.h"
#include "../debug/log.h"
#include "../debug/stacktrace.h"
#include "../memory/table.h"

enum {
    MAX_ASSERTIONS_TRACKED = 32,
    ASSERTION_CONTEXT_LEN = 128,
    ASSERTION_REPEAT_THRESHOLD = 10 // seconds
};

typedef struct TrackedAssertion {
    uintptr_t caller_address;
    char context[ASSERTION_CONTEXT_LEN]; // truncate context to 128 characters if necessary
    time_t last_time;
    int repeat_count;
} TrackedAssertion;

MAKE_TABLE_STRUCT(AssertionTable, TrackedAssertion);

#ifdef DEBUG
bool assertion_enforcing = true;
#else
bool assertion_enforcing = false;
#endif

static AssertionTable *assertion_tracking_table = NULL;

/**
 * @todo Check for repeated assertions every tick, if anyone exceeds the repeat threshold, log it
 */
void assertion_failed_throw_exception(const char *expr, const char *file, int line, const char *assertion_context_fmt, ...) {
    char assertion_context[ASSERTION_CONTEXT_LEN] = {0};
    if(assertion_context_fmt) {
        va_list args;
        va_start(args, assertion_context_fmt);
        vsnprintf(assertion_context, ASSERTION_CONTEXT_LEN, assertion_context_fmt, args);
        va_end(args);
    }

    const char *assertion_context_message = "no info";
    if(strlen(assertion_context) > 0) {
        assertion_context_message = assertion_context;
    }

    // Capture context from the current thread
    CONTEXT context;
    RtlCaptureContext(&context);
    uintptr_t caller_address = (uintptr_t)stacktrace_get_caller_address(&context);

    if(assertion_enforcing) {
        exception_throw_runtime_error("Assertion failed at 0x%.8X: %s, file %s, line %d, context: %s", caller_address, expr, file, line, assertion_context_message);
        return;
    }

    if(!assertion_tracking_table) {
        assertion_tracking_table = table_new("assertions", MAX_ASSERTIONS_TRACKED, sizeof(TrackedAssertion));
    }

    // Find existing entry
    TrackedAssertion *found = NULL;
    TABLE_FIND_ELEMENT(assertion_tracking_table, found, found->caller_address == caller_address && strcmp(found->context, assertion_context) == 0);

    if(found) {
        found->repeat_count++;
        time_t now = time(NULL);
        if(difftime(now, found->last_time) >= 10) {
            double elapsed = difftime(now, found->last_time);
            log_debug("Assertion failed at 0x%.8X (repeated %d times in %.0f seconds): %s, file: %s, line: %d, context: %s",
                caller_address, found->repeat_count, elapsed, expr, file, line, assertion_context_message);
            found->repeat_count = 0;
            found->last_time = now;
        }
        return;
    }

    // If not found, add new entry
    TrackedAssertion *entry = (TrackedAssertion *)table_add_element(assertion_tracking_table);
    if(!entry) {
        // table full; find the oldest and replace
        TrackedAssertion *oldest = NULL;
        TABLE_FIND_ELEMENT(assertion_tracking_table, oldest, true);
        if(oldest) {
            entry = oldest;
        }
    }

    if(entry) {
        entry->caller_address = caller_address;
        entry->last_time = time(NULL);
        entry->repeat_count = 0;
        entry->context[0] = '\0';
        strncpy(entry->context, assertion_context, ASSERTION_CONTEXT_LEN);
    }

    // log normally the first time
    log_debug("Assertion failed at 0x%.8X: %s, file %s, line %d, context: %s", caller_address, expr, file, line, assertion_context_message);
    exception_print_stack_trace(&context);
}
