#ifndef RINGWORLD__NETGAME__NETWORK_TRANSPORT_H
#define RINGWORLD__NETGAME__NETWORK_TRANSPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../memory/memory.h"
#include "../memory/circular_queue.h"
#include "gamespy.h"

enum {
    MAX_NUM_OF_MACHINE_CONNECTIONS = 16
};

typedef struct NetworkTransportAddressIPv4 {
    uint8_t class_a;
    uint8_t class_b;
    uint8_t class_c;
    uint8_t class_d;
} NetworkTransportAddressIPv4;

typedef struct NetworkTransportAddressIPv6 {
    uint16_t class_a;
    uint16_t class_b;
    uint16_t class_c;
    uint16_t class_d;
    uint16_t class_e;
    uint16_t class_f;
    uint16_t class_g;
    uint16_t class_h;
} NetworkTransportAddressIPv6;

typedef struct NetworkTransportAddress {
    union {
        NetworkTransportAddressIPv4 ipv4;
        NetworkTransportAddressIPv6 ipv6;
    };
    int16_t address_length;
    int16_t port;
    int32_t unk1;
} NetworkTransportAddress; 
_Static_assert(sizeof(NetworkTransportAddress) == 0x18);

typedef struct NetworkTransportEndpoint {
    NetworkGamespyConnection *gs_connection;
    bool is_listening_endpoint; 
    bool unk1;
    SOCKET socket; 
    uint8_t flags; 
    uint8_t type; 
    int16_t error_code; 
    CircularQueue *received_data_queue;
    int32_t gamespy_log_column;
    uint32_t rejection_code;
} NetworkTransportEndpoint; 
_Static_assert(sizeof(NetworkTransportEndpoint) == 0x1C);

typedef struct NetworkTransportEndpointSet {
    fd_set select_set;
    NetworkTransportEndpoint **ep_array;
    int32_t max_endpoints;
    int32_t count;
    int32_t array_cursor;
} NetworkTransportEndpointSet; 
_Static_assert(sizeof(NetworkTransportEndpointSet) == 0x114);

struct NetworkTranportServerConnection;

typedef void (*proc_connection_reject)(NetworkTransportEndpoint *endpoint, uint32_t rejection_code);

typedef struct NetworkConnection {
    NetworkTransportEndpoint *reliable_endpoint;
    uint32_t keep_alive_time;
    proc_connection_reject connection_rejection_proc;
    CircularQueue *incoming_queue;
    char reliable_outgoing_sled[0x534];	// add this struct later
    char unreliable_outgoing_sled[0x534]; 
    char prioritization_buffer[0x10]; 
    uint32_t connection_class; 
    uint32_t flags; 
    int32_t unk1;
    struct NetworkTranportServerConnection *server_connection;
    bool is_local_connection;
} NetworkConnection; 
_Static_assert(sizeof(NetworkConnection) == 0xA9C);

typedef struct NetworkTranportServerConnection {
    NetworkConnection connection;
    NetworkTransportEndpointSet *endpoint_set;
    NetworkConnection *client_list[MAX_NUM_OF_MACHINE_CONNECTIONS];
    bool unk1;
    bool has_local_connection;
} NetworkTranportServerConnection; 
_Static_assert(sizeof(NetworkTranportServerConnection) == 0xAE4);

#ifdef __cplusplus
}
#endif

#endif
