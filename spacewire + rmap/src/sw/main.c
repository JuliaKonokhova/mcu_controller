#include "stdlib.h"
#include "stddef.h"
#include "stdio.h"
#include "K1874VE7T.h"
#include "common.h"
#include "uart.h"
#include "spacewire.h"
#include "RMAP_ctrl.h"

void main(void) {
    extern uint16_t spwstat_log;

    #ifdef _DEBUG
        init_uart0();
    #endif

    SpW_Init();

    while (1){
        if (spwstat_log & (SWSTAT_EOPREC_Msk | SWSTAT_EEPREC_Msk)) {
            SpW_RMAP_target();
            SWSTAT |= (SWSTAT_EEPREC_Msk | SWSTAT_EOPREC_Msk | SWSTAT_DATAREC_Msk | SWSTAT_RBUFFULL_Msk);
        }
        __NOP();
    }
}

