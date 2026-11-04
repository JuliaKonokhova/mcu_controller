#include "bsi_rt.h"

// Запись двух байт данных из адреса по другому адресу
void write_addr_to_addr(unsigned short addr_from, unsigned short addr_to) {
	volatile unsigned short temp;
	temp = *(volatile unsigned short*)(addr_from);
	*(volatile unsigned short*)(addr_to) = temp;
}

// Запись двух байт данных по адресу
void write_data_to_addr(unsigned short data, unsigned short addr) {
	*(volatile unsigned short*)(addr) = data;
}

// инициализация МКО
void init_bsi_terminal(unsigned char taddr) {
	
	unsigned char tmp;
	
	BSICONFIG = (1 << 14) | (BSI_TXD_LEVEL << 8) | ((unsigned char) (FXTALL_MHz / 4) & 0x3F); // BSI_TXD_LEVEL = 1 для приемо-передатчика EL-12, MODE = 1 (удаленный терминал)
	
	tmp = WSR;                //store WSR
	WSR = 0x0F;               //swap to HWindow15
	INT_MASK1 |= 0x02; 
	WSR = tmp;
	
	BSICON_TERM = (taddr << 11) | BSICON_TERM_BCMSGEN_Msk | BSICON_TERM_TON_Msk;
}

// инициализация МКО
void init_bsi_terminal_alt() {
	
	BSICONFIG  = 0x4105;
	INT_MASK1 |= 0x02; //Enable BSI interrupt
	BSICON_TERM = 0x0801;  //Terminal_ADDR = 1;
}

// передача данных по МКО (опрос)
void bsi_tx(const unsigned char saddr, unsigned char count, unsigned short* data) {
	unsigned char i, cnt;
	volatile unsigned short msg_addr = RT_TRANSMIT_BASE + RT_SUBADDR_SIZE * saddr + 2; 
	if (count == 32)
		cnt = count - 1;
	else
	  cnt = count;

	for (i = 0; i < cnt; i++) {
		data[i] = *(volatile unsigned short*)(msg_addr);
		msg_addr += 2;
	}
	if (count == 32)
		data[31] = *(volatile unsigned short*)(RT_TRANSMIT_BASE + RT_SUBADDR_SIZE * saddr);
}

// прием данных по МКО (конфигурации, ДПУ, ВД второй вариант)
void bsi_rx(const unsigned char saddr, unsigned char count, unsigned short* data) {
	unsigned char i,cnt;
	unsigned short msg_addr = RT_RECEIVE_BASE + RT_SUBADDR_SIZE * saddr;
	if (count == 32) {
		*(volatile unsigned short*)(msg_addr) = data[31]; 
		cnt = count - 1;
	} 
  else
	cnt = count;  
  msg_addr += 2;
  
	for (i = 0; i < cnt; i++) {
		*(volatile unsigned short*)(msg_addr) = data[i];
		msg_addr += 2;
	}
}

// начальная инициализация памяти ОЗУ МКО по подадресам
void bsi_subaddr_init(unsigned char subaddr) {
	unsigned char i;
	unsigned short data[32];

	if (subaddr >= 5 && subaddr <= 8) {
		for (i = 0; i < 32; i++)
			data[i] = 0;
		bsi_rx(subaddr, 32, data);
	} 
	else if (subaddr == 9) {
		for (i = 0; i < 18; i++)
			data[i] = 0;
	store_at_address(0x1240, 15, 18, data);
	} 
	else if (subaddr >= 10 && subaddr <= 11) {
		for (i = 0; i < 32; i++)
			data[i] = 0;
		bsi_rx(subaddr, 32, data);
	}
	else if (subaddr >= 12 && subaddr <= 16) {
		for (i = 0; i < 32; i++)
			data[i] = 0;
		bsi_rx(subaddr, 32, data);
	} 
	else if (subaddr >= 17 && subaddr <= 29) {
		for (i = 0; i < 32; i++)
			data[i] = 0;
		bsi_rx(subaddr, 32, data);
	} 
	else {
		for (i = 0; i < 32; i++)
			data[i] = (subaddr << 8) | i;
		bsi_rx(subaddr, 32, data);  
	}
}

unsigned short bsi_read_special(const unsigned char cod) {
	return *(volatile unsigned short*)(RT_RECEIVE_SPECIAL_ADDR+(cod << 1));
}

void bsi_write_special(const unsigned char cod,unsigned short data) {
	*(volatile unsigned short*)(RT_TRANSMIT_SPECIAL_ADDR+(cod << 1)) = (unsigned short)data;
}

void store_at_address(unsigned short start_addr, unsigned char num_sd, unsigned char count, unsigned short* data) {
	unsigned char i;
	unsigned short msg_addr = start_addr + num_sd * 2;

	for (i = 0; i < count; i++) {
		*(volatile unsigned short*)(msg_addr) = data[i];
		msg_addr += 2;
	}
}

void bsi_data_init() {
	unsigned char i;
	for (i = 0; i <= 31; i++)
		bsi_subaddr_init(i);
}

void AD_to_BSI(unsigned char saddr) {
  
	volatile unsigned char i;
	volatile unsigned short temp_value;
	volatile unsigned short bsi_addr, ad_addr;

	bsi_addr = RT_TRANSMIT_BASE + RT_SUBADDR_SIZE * saddr;
	
	if (saddr < 8 || saddr > 10) { // ДТИ, резерв
		if (saddr < 8) { // ДТИ
			switch (saddr) {
				case 2:
					ad_addr = saddr * RT_SUBADDR_SIZE + 128 + AD_BASE_T; // 3ий подадрес -> 5ый подадрес
					break;
				case 3:
					ad_addr = saddr * RT_SUBADDR_SIZE + 128 + AD_BASE_T; // 4ый подадрес -> 6ой подадрес
					break;
				case 4:
					ad_addr = saddr * RT_SUBADDR_SIZE - 128 + AD_BASE_T; // 5ый подадрес -> 3ий подадрес
					break;
				case 5:
					ad_addr = saddr * RT_SUBADDR_SIZE + 64 + AD_BASE_T; // 6ой подадрес -> 7ой подадрес
					break;
				case 6:
					ad_addr = saddr * RT_SUBADDR_SIZE - 192 + AD_BASE_T; // 7ой подадрес -> 4ый подадрес
					break;
				default:
					ad_addr = saddr * 0x40 + AD_BASE_T; // любой другой подадрес
					break;
			}
		}
		else // резерв
			ad_addr = 0x0580 + (saddr - 11) * 0x40 + AD_BASE_T;
	}
	else // СТМИ
		ad_addr = 0x0400 + (saddr - 8) * 0x40 + AD_BASE_T;
	
	for (i = 0; i < 31; i++) { // непосредственная запись памяти
		if ((ad_addr + i * 2) != 0x42C + AD_BASE_T && (ad_addr + i * 2) != 0x42E + AD_BASE_T) { // обычные адреса
			temp_value = *(volatile unsigned short*)(ad_addr + i * 2);
			*(volatile unsigned short*)(bsi_addr + i * 2 + 2) = temp_value;
		}
		else { // исключительные адреса
			if ((ad_addr + i * 2) == 0x42C + AD_BASE_T) {
				temp_value = *(volatile unsigned short*)(0x1000 + AD_BASE_T);
				*(volatile unsigned short*)(bsi_addr + i * 2 + 2) = temp_value;
			}
			else {
				temp_value = *(volatile unsigned short*)(0x1002 + AD_BASE_T);
				*(volatile unsigned short*)(bsi_addr + i * 2 + 2) = temp_value;
			}
		}
	}
	// Учет 32ого СД
	temp_value = *(volatile unsigned short*)(ad_addr + 31 * 2);
	*(volatile unsigned short*)(bsi_addr) = temp_value;

}

// Передача данных из буфера МКО по шине AD
void BSI_to_AD(unsigned char saddr, unsigned char count) {
	
	volatile unsigned char i;
	volatile unsigned short bsi_addr, ad_addr;
	
	bsi_addr = RT_RECEIVE_BASE + RT_SUBADDR_SIZE * saddr + 4;
	ad_addr = AD_BASE_R + (RT_SUBADDR_SIZE - 4) * saddr;
	
	for (i = 0; i < count; i++)
		*(volatile unsigned short*)(ad_addr + i * 2) = *(volatile unsigned short*)(bsi_addr + i * 2); 
}

// cd 2. 844 + 0 * 2 = 844 
// cd 3. 844 + 1 * 2 = 846
// cd 4. 844 + 2 * 2 = 848
// cd 5. 844 + 3 * 2 =  84A
// cd 6. 844 + 4 * 2 = 84C
// cd 7. 844 + 5 * 2 = 84E
// cd 8. 844 + 6 * 2 = 850
// cd 9. 844 + 7 * 2 = 852
// cd 10. 844 + 8 * 2 = 854
// cd 11. 844 + 9 * 2 = 856
// cd 12. 844 + 10 * 2 = 858
// cd 13. 844 + 11 * 2 = 85A
// cd 14. 844 + 12 * 2 = 85C
// cd 15. 844 + 13 * 2 = 85E
// cd 16. 844 + 14 * 2 = 860
// cd 17. 844 + 15 * 2 = 862
// cd 18. 844 + 16 * 2 = 864
// cd 19. 844 + 17 * 2 = 866
// cd 20. 844 + 18 * 2 = 868
// cd 21. 844 + 19 * 2 = 86A
// cd 22. 844 + 20 * 2 = 86C
// cd 23. 844 + 21 * 2 = 86E
// cd 24. 844 + 22 * 2 = 870
// cd 25. 844 + 23 * 2 = 872
// cd 26. 844 + 24 * 2 = 874
// cd 27. 844 + 25 * 2 = 876
// cd 28. 844 + 26 * 2 = 878
// cd 30. 844 + 27 * 2 = 87A
// cd 31. 844 + 28 * 2 = 87C
