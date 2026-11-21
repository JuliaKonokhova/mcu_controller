#include "stdio.h"
#include "stdint.h"

#ifndef __RMAP_CTRL_H
    #define __RMAP_CTRL_H

    #define RMAP_WRITE_READ 0b00100000
    #define RMAP_VERIFY     0b00010000
    #define RMAP_REPLY      0b00001000
    #define RMAP_INCREMENT  0b00000100

    typedef struct {
        int8_t          target_spw_address[12];
        uint8_t         protocol_identifier;
        uint8_t         inst;
        uint8_t         key;
        int8_t          reply_spw_address[12];
        uint8_t         initiator_logical_address;
        uint8_t         transaction_identifier[2];
        uint8_t         address[5]; // inc extended
        uint8_t         data_lenght[3];
    } RMAP_t;

    // RMAP Status
    typedef enum {
        SUCCESS                        =   0
        ,GENERAL_ERROR                  =   1
        ,UNUSED_RMAP_TYPE_OR_CODE       =   2
        ,INVALID_KEY                    =   3
        ,INVALID_DATA_CRC               =   4
        ,EARLY_EOP                      =   5
        ,TOO_MUCH_DATA                  =   6
        ,EEP                            =   7
        ,RESERVED                       =   8
        ,VERIFY_BUFFER_OVERRUN          =   9
        ,RMAP_CMD_NOT_IMPL_OR_AUTH      =   10
        ,RMW_DATA_LENGTH_ERROR          =   11
        ,INVALID_TARGET_LOGICAL_ADDRESS =   12

        // unspec
        ,HEADER_CRC_ERROR = 13
    } RMAP_status_t;

    // RMAP command
    typedef enum {
        ILLEGAL
        ,WRITE
        ,READ
        ,READ_MODIFY_WRITE
    } RMAP_command_t;

    uint16_t crc8(uint8_t *, uint16_t);

    uint8_t get_bit(uint8_t, uint8_t);

    uint8_t get_bits(uint8_t, uint8_t, uint8_t);

    uint8_t check_bounds(uint8_t *, uint8_t *);

    RMAP_command_t RMAP_cmd_type(uint8_t);

    uint16_t build_rmap_reply(RMAP_t *, RMAP_status_t, uint8_t *);

    RMAP_status_t parser_RMAP(RMAP_t *, uint8_t *, uint16_t);

    RMAP_status_t SpW_RMAP_target(void);

#endif
