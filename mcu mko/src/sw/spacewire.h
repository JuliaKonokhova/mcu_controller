#include "stdint.h"

#ifndef __SPACEWIRE_H
    #define __SPACEWIRE_H

    uint8_t SpW_Get_Bytes();
    void SpW_erase_fifo(void);
    void SpW_Send_Packet(uint8_t*, uint8_t);
    void IRQ_SpW_handler(void);
    void SpW_Init(void);

#endif
