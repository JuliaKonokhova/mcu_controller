#include "stdio.h"
#include "stdint.h"
#include "K1874VE7T.h"
#include "common.h"
#include "spacewire.h"
#include "RMAP_ctrl.h"

#pragma interrupt 6 IRQ_SpW_handler

uint8_t buffer_in[_SpW_BUFFER_SIZE];
uint8_t buffer_out[_SpW_BUFFER_SIZE];
uint16_t spw_buff_pointer = 0;
uint16_t spwstat_log = 0;

void SpW_Init(void) {
    _di_();
    IOPORT1=~SWCAPSTAT_Bits.CAPRX;
    IOPORT1=~SWCAPSTAT_Bits.CAPTX;

    WSR = 0x01;         // Hwindow 1
    IOC3 = IOC3_SPWS_SEL_Msk | IOC3_SPWD_SEL_Msk;

    WSR = 0x0F;         // Hwindow 15
    INT_MASK |= INT_PEND_SER_PEND_Msk;
    WSR = 0x00;         // Hwindow 0

    SWINTMASK = SWINTMASK_DATAINT_Msk | SWINTMASK_EEPINT_Msk | SWINTMASK_EOPINT_Msk;
    SWCON = 0x0800 | SWCON_LINKAUTO_Msk | SWCON_LINKEN_Msk;
    _ei_();
}

void IRQ_SpW_handler(void) {
    SpW_Get_Bytes();
    SWSTAT |= (SWSTAT_EEPREC_Msk | SWSTAT_EOPREC_Msk | SWSTAT_DATAREC_Msk | SWSTAT_RBUFFULL_Msk);
}

uint8_t SpW_Get_Bytes() {
    uint16_t spw_word;
    while (SWCAPSTAT & 0xFF && spw_buff_pointer <= _SpW_BUFFER_SIZE){
        spw_word = SWDATAR;
        buffer_in[spw_buff_pointer++] = spw_word & 0xFF;
        spwstat_log |= SWSTAT;
    }
    if (spw_buff_pointer > _SpW_BUFFER_SIZE) {
        SpW_erase_fifo();
        spw_buff_pointer = 0;
        spwstat_log = 0;
        SWSTAT |= (SWSTAT_EEPREC_Msk | SWSTAT_EOPREC_Msk | SWSTAT_DATAREC_Msk | SWSTAT_RBUFFULL_Msk);
        return 1;
    }
    return 0;
}

void SpW_erase_fifo(void) {
    uint16_t *ZERO_REG = 0x0000;
    while (!(SWSTAT & SWSTAT_RBUFEMPTY_Msk))
        *ZERO_REG = SWDATAR;
    SWSTAT |= (SWSTAT_EEPREC_Msk | SWSTAT_EOPREC_Msk | SWSTAT_DATAREC_Msk | SWSTAT_RBUFFULL_Msk);
}

void SpW_Send_Packet(uint8_t* array, uint8_t num) {
    uint8_t i = 0;
    while (i != num)
        if ((SWCAPSTAT & 0xFF00) != 0xFF)
            SWDATAT = array[i++];
    if (num != 0)
        SWDATAT = 0x100;      // EOP
}
