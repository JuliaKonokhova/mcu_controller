
#ifndef __TRM
#define __TRM

#include <stdio.h>

#ifndef dXTAL
        #define dXTAL 20000000UL
#endif
#ifndef BIT_RATE
        #define BIT_RATE 9600UL
#endif
#ifndef BAUD_LO_VAL
        #define BAUD_LO_VAL ((unsigned char)(dXTAL/(BIT_RATE<<4)-1))
#endif
#ifndef BAUD_HI_VAL
        #define BAUD_HI_VAL ((unsigned char)(0x80))
#endif
#ifndef TRM_BUF_SIZE
        #define TRM_BUF_SIZE 128
#endif


void trm_init();
void trm_send(char *buff);
void trm_send_byte(char c);

//void IRQ_timer(void);
//void init_timer(void);


#endif