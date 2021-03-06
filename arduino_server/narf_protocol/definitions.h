#ifndef NARF_WIRELESS_PROTOCOL_DEFINITIONS_H
#define NARF_WIRELESS_PROTOCOL_DEFINITIONS_H

// core protocol definitions
#define NARF_PROT_VER_MAX               1
#define NARF_PROT_VER_MIN               0
#define NARF_PROT_PORT_NUM             12
#define NARF_PROT_HEADER_SIZE          10
#define NARF_PROT_MAX_MSG_DATA_SIZE    47
#define NARF_PROT_BYTE_WAIT_TIMEOUT   750
#define NARF_PROT_REC_HEAD_MAX        100
#define NARF_PROT_PIN_MIN_NUM           2
#define NARF_PROT_PIN_MAX_NUM          13

// AP mode 
#define NARF_AP_SSID        "narf2408"
#define NARF_AP_PASSWORD   "Nesto8812"

//////////////////
// PROTOCOL CMD //
//////////////////
#define NARF_CMD_READ_PINS_D     0x08
#define NARF_CMD_WRITE_PINS_D    0x16

////////////////////////////////////////////
// PROTOCOL RESPONSE - SERVER SIDE ERRORS //
////////////////////////////////////////////
#define NARF_RES_OK                     0x08
#define NARF_RES_ERROR_MSG_SIZE_C       0x16 
#define NARF_RES_ERROR_CMD_UNKNOWN      0x24
#define NARF_RES_ERROR_TIMEOUT_C        0x32
#define NARF_RES_ERROR_INVALID_DATA     0x40
#define NARF_RES_ERROR_COMMUNICATION_C  0x48
#define NARF_RES_ERROR_UNKNOWN_C        0xAA 

////////////////////////
// CLIENT SIDE ERRORS //
////////////////////////
#define NARF_CLIENT_ERROR_MSG_SIZE_C      0x64
#define NARF_CLIENT_ERROR_TIMEOUT_C       0x72
#define NARF_CLIENT_ERROR_COMMUNICATION_C 0x80

#endif // NARF_WIRELESS_PROTOCOL_DEFINITIONS_H