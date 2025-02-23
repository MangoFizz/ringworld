#ifndef RINGWORLD__NETWORKING__GAMESPY_H    
#define RINGWORLD__NETWORKING__GAMESPY_H    

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <winsock.h>
#include "../memory/memory.h"

typedef struct NetworkGamespySocket {
    SOCKET socket;
    IN_ADDR address;
    int16_t port;
    void *connections;
    void *closed_connections;
    BOOL close;
    BOOL error;
    void *connect_attempt_callback;
    void *socket_error_callback;
    void *send_dump_callback;
    void *receive_dump_callback;
    void *unrecognized_message_callback;
    void *user_data; 
    int32_t incoming_buffer_size;
    int32_t outgoing_buffer_size;
    int32_t protocol_offset;
    int32_t pad1;
    BOOL broadcast_enabled;
} NetworkGamespySocket; 
_Static_assert(sizeof(NetworkGamespySocket) == 0x48);

typedef struct NetworkGamespyBuffer {
    byte *buffer;
    uint32_t buffer_size;
    uint32_t length;
} NetworkGamespyBuffer;
_Static_assert(sizeof(NetworkGamespyBuffer) == 0xC);

typedef struct NetworkGamespyConnection {
    IN_ADDR address;
    int16_t port;
    NetworkGamespySocket *socket;
    uint32_t state;
    BOOL initiated;
    BOOL free_at_accept_reject;
    uint32_t connection_result;
    uint32_t start_time;
    uint32_t timeout;
    int32_t callback_level;
    void *connected_callback;
    void *received_callback;
    void *closed_callback;
    void *ping_callback;
    char *initial_message;
    int32_t initial_message_len;
    void *user_data;
    NetworkGamespyBuffer incoming_buffer;
    NetworkGamespyBuffer outgoing_buffer;
    void *incoming_buffer_messages;
    void *outgoing_buffer_messages;
    uint16_t serial_number, expected_serial_number;
    char response[32];
    uint32_t last_send;
    uint32_t challenge_time;
    char pad1[16];
    void *send_filters;
    void *receive_filters;
    char pad2[4];
    char pad3[28];
    char pad4[32];
    char pad5[32];
    char pad6[32];
    char pad7[20];
    char pad8[16];
    int32_t unk1;
} NetworkGamespyConnection; 
_Static_assert(sizeof(NetworkGamespyConnection) == 0x150);

#ifdef __cplusplus
}
#endif

#endif
