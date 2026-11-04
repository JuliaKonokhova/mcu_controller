#include "stdint.h"
#include "common.h"

#ifndef __UART_H
    #define __UART_H
    //---------------------------Defines-------------------------
    #define  TI_RI_MASK        0x3    //Transmit/Receive interrupt enable
    #define  SP_MODE_0         0x8    //Mode0
    #define  SP_MODE_1         0x9    //Mode1
    #define  SP_MODE_2         0xA    //Mode2
    #define  SP_MODE_3         0xB    //Mode3
    //--------------------------Defines Baud_Rate----------------
    #define BitRate 9600UL
    #define BAUD_LO_VAL ((uint8_t) (MCU_CLOCK/(BitRate*16)-1))
    #define BAUD_HI_VAL  0x80 //Clock UART from External clock of XTAL1

    void init_uart0(void);

#endif
