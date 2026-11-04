#include "K1874VE7T.h"
#include "common.h"
#include "uart.h"

void init_uart0() {
    uint8_t tmp;

    tmp = WSR;                 //store WSR
    WSR = (tmp & 0x80);        //swap to HWindow0
    __NOP();
    __NOP();
    SP_CON0 = SP_MODE_1 ;      //Clear SP_CON
    BAUD_RATE0 = BAUD_LO_VAL;  //write Baud rate value
    BAUD_RATE0 = BAUD_HI_VAL;
    __NOP();
    __NOP();
    WSR = tmp;                 //Restore HWindow
}
