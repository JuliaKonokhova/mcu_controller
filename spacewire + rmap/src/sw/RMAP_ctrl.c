// Original author Andrey Maksyutin
// Modification author Mikhail Shomov

#include "K1874VE7T.h"
#include "common.h"
#include "RMAP_ctrl.h"
#include "spacewire.h"

// RMAP allowed bound
const uint8_t bound_high[] = _RMAP_BOUND_H;
const uint8_t bound_low[] = _RMAP_BOUND_L;

const uint8_t rmap_crc_table[256] = {
    0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75,
    0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
    0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69,
    0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
    0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D,
    0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
    0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51,
    0x2a, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,
    0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05,
    0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
    0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19,
    0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
    0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D,
    0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
    0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21,
    0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,

    0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95,
    0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
    0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89,
    0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
    0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD,
    0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
    0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1,
    0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBf,
    0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5,
    0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
    0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9,
    0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
    0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3e, 0x4C, 0xDD,
    0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
    0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1,
    0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF
};

uint16_t crc8(uint8_t *data, uint16_t size) {
    uint16_t crc = 0x00;
    uint16_t i;
    for (i = 0; i < size; i++)
        crc = rmap_crc_table[crc ^ data[i]];

    return crc;
}

uint8_t get_bit(uint8_t byte, uint8_t bit_number) {
    return (byte >> bit_number) & 1;
}

uint8_t get_bits(uint8_t byte, uint8_t lim_high, uint8_t lim_low) {
    uint16_t i;
    uint8_t result = 0;
    for (i = lim_low; i <= lim_high; i++)
        result |= (get_bit(byte, i) << (i - lim_low));

    return result;
}

uint8_t check_bounds(uint8_t* addr, uint8_t* data_length) {
    int8_t i;
    uint8_t carry = 0;

    for (i = sizeof(bound_high)-1; i >= 0; i--)
        if (i >= sizeof(bound_high)-sizeof(((RMAP_t *)0)->data_lenght))
            carry = (bound_high[i] - addr[i] - data_length[i-sizeof(bound_high)+sizeof(((RMAP_t *)0)->data_lenght)] - carry) < 0;
        else
            carry = (bound_high[i] - addr[i] - carry) < 0;
    if (carry) 
        return carry;
    
    for (i = sizeof(bound_low)-1; i >= 0; i--)
        carry = ((addr[i] - bound_low[i] - carry) < 0);
    return carry;
}

RMAP_command_t RMAP_cmd_type(uint8_t cmd) {
    if (cmd == 0b0000 || cmd == 0b0001 || cmd == 0b0100 ||
            cmd == 0b0101 || cmd == 0b0110)
        return ILLEGAL;
    if (cmd == 0b0010 || cmd == 0b0011)
        return READ;
    if (cmd == 0b0111)
        return READ_MODIFY_WRITE;
    return WRITE;
}

uint16_t build_rmap_reply(RMAP_t* rr, RMAP_status_t status, uint8_t *temp_buf) {

    uint16_t counter = 0;
    uint16_t i;
    uint8_t reply_spw_address_len;
    uint8_t reply_apw_address_mask = 0;
    uint8_t *address;

    //////////////////////  адрес назначения  ///////////////////////
    for (i = 0; i < sizeof(rr->reply_spw_address); i++) {
        reply_apw_address_mask |= rr->reply_spw_address[i];
        if (rr->reply_spw_address[i] == -1)
            break;
        else if (reply_apw_address_mask)
            temp_buf[counter++] = rr->reply_spw_address[i];
    }
    reply_spw_address_len = counter;

    //////////////////////  логический адрес отправителя  ///////////////////////
    temp_buf[counter++] = rr->initiator_logical_address;

    //////////////////////  идентификатор протокола  ///////////////////////
    temp_buf[counter++] = rr->protocol_identifier;

    //////////////////////  инструкция  ///////////////////////
    temp_buf[counter++] = rr->inst;

    //////////////////////  статус  ///////////////////////
    temp_buf[counter++] = status;

    //////////////////////  логический адрес назначения  ///////////////////////
    temp_buf[counter++] = _SpW_ADDRESS;

    //////////////////////  идентификатор транзакции  ///////////////////////
    for (i = 0; i < sizeof(rr->transaction_identifier); i++)
        temp_buf[counter++] = rr->transaction_identifier[i];

    if ((rr->inst & RMAP_WRITE_READ) == 0x00) {

        //////////////////////  резерв  ///////////////////////
        temp_buf[counter++] = 0x00;

        //////////////////////  длина данных  ///////////////////////
        for (i = 0; i < sizeof(rr->data_lenght); i++)
            temp_buf[counter++] = rr->data_lenght[i];

    }

    //////////////////////  crc заголовка  ///////////////////////
    temp_buf[counter++] = crc8(&temp_buf[reply_spw_address_len], counter - reply_spw_address_len);

    //////////////////////  данные  ///////////////////////
    if (((rr->inst & RMAP_WRITE_READ) == 0x00) && (rr->data_lenght[2] | (rr->data_lenght[1] << 8) | (rr->data_lenght[0] << 16)) > 0) { // указание на команду чтения или RMW + данные больше 0
        address = (uint8_t *)(rr->address[4] | rr->address[3] << 8);
        for (i = 0; i < (rr->data_lenght[2] | (rr->data_lenght[1] << 8) | (rr->data_lenght[0] << 16)); i++) {
            if (rr->inst & RMAP_INCREMENT)
                temp_buf[counter++] = address[i];
            else
                temp_buf[counter++] = address[0];
        }
        //////////////////////  crc данных  ///////////////////////
        temp_buf[counter++] = crc8(&temp_buf[counter - (rr->data_lenght[2] | (rr->data_lenght[1] << 8) | (rr->data_lenght[0] << 16))], (rr->data_lenght[2] | (rr->data_lenght[1] << 8) | (rr->data_lenght[0] << 16)));

    }

    return counter;
}

RMAP_status_t parser_RMAP(RMAP_t* rmap_req, uint8_t* temp_buf, uint16_t length) {

    uint16_t i;
    uint8_t reply_spw_address_len;
    uint16_t pkg_len, data_len;
    uint8_t crc_header, crc_data, crc_calc;
    RMAP_command_t cmd_type;
    extern uint16_t spwstat_log;

    rmap_req->protocol_identifier = temp_buf[1];
    
    rmap_req->inst = temp_buf[2];
    cmd_type = RMAP_cmd_type(get_bits(rmap_req->inst, 5, 2));
    
    reply_spw_address_len = get_bits(rmap_req->inst, 1, 0) * 4;
    
    crc_calc = crc8(temp_buf, (15 + reply_spw_address_len));
    crc_header = temp_buf[15 + reply_spw_address_len];

    if (crc_calc != crc_header ||
            length < 16 + reply_spw_address_len)
        return HEADER_CRC_ERROR;

    rmap_req->key = temp_buf[3];

    for (i = 0; i < sizeof(rmap_req->reply_spw_address); i++)
        rmap_req->reply_spw_address[i] = -1;

    for (i = 0; i < reply_spw_address_len; i++)
        rmap_req->reply_spw_address[i] = temp_buf[4 + i];

    rmap_req->initiator_logical_address = temp_buf[4 + reply_spw_address_len];
    
    for (i = 0; i < sizeof(rmap_req->transaction_identifier); i++)
        rmap_req->transaction_identifier[i] = temp_buf[5 + reply_spw_address_len + i];

    for (i = 0; i < sizeof(rmap_req->address); i++)
        rmap_req->address[i] = temp_buf[7 + reply_spw_address_len + i];

    for (i = 0; i < sizeof(rmap_req->data_lenght); i++)
        rmap_req->data_lenght[i] = temp_buf[12 + reply_spw_address_len + i];
    
    if (cmd_type == WRITE || cmd_type == READ_MODIFY_WRITE){
        data_len = (rmap_req->data_lenght[2] | (rmap_req->data_lenght[1] << 8));
        pkg_len = (16 + reply_spw_address_len + data_len + 1);
    }
    else{
        data_len = 0;
        pkg_len = (16 + reply_spw_address_len);
    }

    if (temp_buf[0] != _SpW_ADDRESS)
        return INVALID_TARGET_LOGICAL_ADDRESS;

    if (cmd_type == ILLEGAL || get_bits(rmap_req->inst, 7, 6) != 0b01)
        return UNUSED_RMAP_TYPE_OR_CODE;

    if (rmap_req->key != _SpW_KEY)
        return INVALID_KEY;

    if (cmd_type == READ_MODIFY_WRITE && 
            (((rmap_req->data_lenght[0] | rmap_req->data_lenght[1]) != 0x00) || 
            (rmap_req->data_lenght[2] > 0x08) || ((rmap_req->data_lenght[2] & 0x01) == 0x01)))
        return RMW_DATA_LENGTH_ERROR;
    
    if (pkg_len > length)
        return EARLY_EOP;

    if (pkg_len < length)
        return TOO_MUCH_DATA;
    
    if (check_bounds((uint8_t *)&rmap_req->address, (uint8_t *)&rmap_req->data_lenght))
        return RMAP_CMD_NOT_IMPL_OR_AUTH;

    crc_calc = crc8((uint8_t *)&temp_buf[16 + reply_spw_address_len],
                        (uint16_t)data_len);
    crc_data = temp_buf[16 + reply_spw_address_len + data_len];

    if (crc_calc != crc_data)
        return INVALID_DATA_CRC;

    return SUCCESS;
}

RMAP_status_t SpW_RMAP_target(void) {
    extern uint8_t buffer_in[_SpW_BUFFER_SIZE];
    extern uint8_t buffer_out[_SpW_BUFFER_SIZE];
    extern uint16_t spw_buff_pointer;
    extern uint16_t spwstat_log;

    RMAP_t rmap_query;
    uint16_t out_num = 0;
    RMAP_status_t RMAP_pkg_status;
    RMAP_command_t cmd_type;
    uint8_t reply_spw_address_len;
    
    uint8_t i_wr;
    uint8_t* addr_wr;

    RMAP_pkg_status = parser_RMAP((RMAP_t*)&rmap_query, (uint8_t*)&buffer_in, spw_buff_pointer-1);
    reply_spw_address_len = get_bits(rmap_query.inst, 1, 0) * 4;

    if (RMAP_pkg_status != HEADER_CRC_ERROR) {
        if (spwstat_log & SWSTAT_EEPREC_Msk)
            RMAP_pkg_status = EEP;

        if (rmap_query.inst & RMAP_REPLY &&
                !((spwstat_log & SWSTAT_EEPREC_Msk) == SWSTAT_EEPREC_Msk && (16 + reply_spw_address_len == spw_buff_pointer)))
            out_num = build_rmap_reply((RMAP_t*)&rmap_query, RMAP_pkg_status, (uint8_t*)&buffer_out);
        
        cmd_type = RMAP_cmd_type(get_bits(rmap_query.inst, 5, 2));
        if ((cmd_type == WRITE || cmd_type == READ_MODIFY_WRITE) &&
                ((RMAP_pkg_status == SUCCESS) || ((rmap_query.inst & RMAP_VERIFY) == 0x00 && 
                (RMAP_pkg_status == INVALID_DATA_CRC || RMAP_pkg_status == EARLY_EOP || RMAP_pkg_status == TOO_MUCH_DATA || RMAP_pkg_status == EEP)))) {
            addr_wr = (uint8_t *)(rmap_query.address[4] | rmap_query.address[3] << 8);
            for (i_wr = 0; i_wr < rmap_query.data_lenght[2]; i_wr++)
                if (rmap_query.inst & RMAP_INCREMENT)
                    addr_wr[i_wr] = buffer_in[16 + reply_spw_address_len + i_wr];
                else
                    addr_wr[0] = buffer_in[16 + reply_spw_address_len + i_wr];
        }
        SpW_Send_Packet(buffer_out, out_num);
    }
    
    spwstat_log = 0;
    spw_buff_pointer = 0;
    SpW_erase_fifo();
    
    return RMAP_pkg_status;
}
