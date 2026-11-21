#define DEBUG_ENABLE 0
#define BSI_RT_ADDR 8	// BSI Remote Terminal_ADDR

#include "uart.h"
#include "packet_process.h"
#include "spacewire.h"
#include "common.h"
#include "RMAP_ctrl.h"

#pragma CCB 0x2E // 0x2E

#pragma interrupt 0 IRQ_timer
#pragma interrupt 8 IRQ_uart_rx_tx
#pragma interrupt 9 IRQ_bsi

void IRQ_bsi(void);
void IRQ_timer(void);
void build_mko_status(void);

// переменные для работы с таймером
volatile unsigned char timer_signal;
volatile unsigned short timer_count = 0;
volatile unsigned short timer_value;

volatile unsigned char bsi_int = 0;
volatile unsigned char bsi_int_cnt = 0;
volatile unsigned short bsi_rt_stat;
unsigned char trm_buf[TRM_BUF_SIZE];
unsigned char tmp;

volatile unsigned short bsi_int_addr = 0x8202;
unsigned short ct_mko = 0;

// инициализация таймера
void init_timer(void) {
	unsigned char tmp;
	tmp = WSR;					//store WSR
	WSR = 0x0F;					//swap to HWindow15
	INT_MASK |= 0x01; 			//allow transmit interrupt
	WSR = 0x00;					//swap to HWindow0
	__NOP();
	__NOP();
	IOC1 = 0x25;
	__NOP();
	__NOP();
	WSR = tmp;					//Restore HWindow
}

void trm_init() {
	tmp = WSR;                //store WSR
	WSR = 0x0F;               //swap to HWindow15
	INT_MASK1 |= 0x01;        //allow transmit interrupt // 0x03 works
	WSR = 0x00;               //swap to HWindow0
	__NOP();
	__NOP();
	SP_CON0 = 0x09;           //Clear SP_CON
	BAUD_RATE0 = BAUD_LO_VAL; //write Baud rate value
	BAUD_RATE0 = BAUD_HI_VAL;
	__NOP();
	__NOP();
	WSR = tmp;                 //Restore HWindow
}

void trm_send(char *buff){
	unsigned char i = 0;
	_di_();
	while ((buff[i] != 0) && (i < TRM_BUF_SIZE)){
		while(!(SP_STAT0 & 0x08));
		SBUF_TX0 = buff[i];
		i++;
	}
	_ei_();
}

void trm_send_byte(char c) {
	_di_();
	while(!(SP_STAT0 & 0x08));
	SBUF_TX0 = c;
	_ei_();
}

void timer_irq_handler(unsigned short counter) {
	volatile unsigned char i;
	if (timer_count == counter) {
		build_mko_status();
		timer_count = 0;
		write_data_to_addr(0xFFFF, SEMAPHORE_ADDRESS);
		for (i = 0; i < 8; i++)
			AD_to_BSI(i);
		write_data_to_addr(0x0000, SEMAPHORE_ADDRESS);
		for (i = 8; i < 16; i++)
			AD_to_BSI(i);
	}
}

void build_mko_status(void) {
	unsigned short status, datacnt, sa;

	status = BSISTAT_TERM;
	datacnt = status & 0x1F;
	sa = (status >> 5) & 0x1F;

	if (status & BSISTAT_TERM_CONCMD_Msk) 
		ct_mko |= 1 << 0;

	if (status & BSISTAT_TERM_DATAERR_Msk) 
		ct_mko |= 1 << 1;

	if (status & BSISTAT_TERM_T_R_Msk) 
		ct_mko |= 1 << 2;

	ct_mko |= (sa << 3);
	ct_mko |= (datacnt << 8);

	ct_mko |= (1 << 13) | (1 << 14) | (1 << 15); // reserv
	
	store_at_address(0x1240, 14, 1, &ct_mko);
}

void bsi_rt_irq_handler(unsigned short status) {
	unsigned char subaddr;

	bsi_int = 0;
	subaddr = (status >> 5) & 0x1F;

	build_mko_status();
		
	if (!(status & BSISTAT_TERM_T_R_Msk))
		handle_mko_packet(subaddr - 1);

	BSICON_TERM &= ~BSICON_TERM_ABUSY_Msk;
}

void main() {
	
	unsigned char bsi_rt_addr;
	extern uint16_t spwstat_log;

	if (IOPORT2_Bits.PIN2 == 0)
		bsi_rt_addr = IOPORT2_Bits.PIN7 | (IOPORT2_Bits.PIN6 << 1) | (IOPORT1_Bits.PIN6 << 2) | (IOPORT2_Bits.PIN4 << 3) | (IOPORT2_Bits.PIN3 << 4);
	else
		bsi_rt_addr = BSI_RT_ADDR;
	
	// Запуск таймера

	init_timer();
	tmp = WSR;
	WSR = 0x0F;             //swap to HWindow15
	timer_value = 26518; // 26518 - 5 ms // 20268 - 10 ms
	TIMER1 = timer_value + 32767; // TIMER1 |= timer_value; // В качестве рабочего варианта
	WSR = tmp;
	_ei_();

	while (timer_count < 560); // 2.8 с. Счетчик по 5 мс // корректное значение 2800 / 5, т.е. 560
	timer_count = 0;
	bsi_int = 0;

	_ei_();
	trm_init(); 
	SpW_Init();
	bsi_data_init();
	init_bsi_terminal(bsi_rt_addr);
	update_scc(0xFF);
	_ei_();

	while(1) {
		 if (spwstat_log & (SWSTAT_EOPREC_Msk | SWSTAT_EEPREC_Msk)) {
		 	SpW_RMAP_target();
        	SWSTAT |= (SWSTAT_EEPREC_Msk | SWSTAT_EOPREC_Msk | SWSTAT_DATAREC_Msk | SWSTAT_RBUFFULL_Msk);
        }

		if (bsi_int) {
			bsi_int = 0;
			bsi_rt_irq_handler(bsi_rt_stat);
		}
		if (timer_signal) {
			timer_signal = 0;
			timer_irq_handler(39); // счетчик 195 мс - 39
		}
	}
}

void IRQ_uart_rx_tx() {
    char byte_rx;
    char status_uart = SP_STAT0;
    _di_();
    if (status_uart&0x40)
    {
        byte_rx = SBUF_RX0;
        IOPORT1 = ~byte_rx;
        SBUF_TX0 = byte_rx;
    }
    _ei_();
}

void IRQ_timer(void) { // Interrupt of timer (WSR = 15)
	unsigned char tmp;
	tmp = WSR;
	WSR = 0x0F;
	timer_signal = 1;
	timer_count++;
	TIMER1 = timer_value + 32767; // TIMER1 |= timer_value; // В качестве рабочего варианта
	WSR = tmp;
}

void IRQ_bsi() //Interrupt of bsi
{
	bsi_int = 1;
	bsi_rt_stat = BSISTAT_TERM;
	bsi_int_cnt++;
}