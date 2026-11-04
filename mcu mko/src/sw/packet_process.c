#include "packet_process.h"

// переменные для обработки ДПУ
/*volatile unsigned char dpu_valid = 0;
volatile unsigned char dpu_protect = 0;
volatile unsigned char dpu_base = 0;
volatile unsigned char dpu_auth = 0;*/

volatile unsigned char scc_count;
volatile unsigned char scc_old[5] = { 0 };

unsigned short epm = 0x00;

///////////////////////////// СПРАВОЧНАЯ ИНФОРМАЦИЯ /////////////////////////////

CommandDPU dpu[DPUSize] = {
	{1, "Включение МОТД1-А", 0x0121, "Включить комплект A / отключить комплект Б модуля МОТД1", "МОТД1-А=1, МОТД1-Б=0\n\n", 1},
	{2, "Включение МОТД1-Б", 0x0129, "Включить комплект Б / отключить комплект А модуля МОТД1", "МОТД1-Б=1, МОТД1-А=0\n\n", 2},
	{3, "Включение МОУ1-А", 0x0131, "Включить комплект А / отключить комплект Б модуля МОУ1", "МОУ1-А=1, МОУ1-Б=0\n\n", 3},
	{4, "Включение МОУ1-Б", 0x0139, "Включить комплект Б / отключить комплект А модуля МОУ1", "МОУ1-Б=1, МОУ1-А=0\n\n", 4},
	{5, "Включение МОТД2-А", 0x0122, "Включить комплект А / отключить комплект Б модуля МОТД2", "МОТД2-А=1, МОТД2-Б=0\n\n", 5},
	{6, "Включение МОТД2-Б", 0x012A, "Включить комплект Б / отключить комплект А модуля МОТД2", "МОТД2-Б=1, МОТД2-А=0\n\n", 6},
	{7, "Включение МОУ2-А", 0x0132, "Включить комплект А / отключить комплект Б модуля МОУ2", "МОУ2-А=1, МОУ2-Б=0\n\n", 7},
	{8, "Включение МОУ2-Б", 0x013A, "Включить комплект Б / отключить комплект А модуля МОУ2", "МОУ2-Б=1, МОУ2-А=0\n\n", 8},
	{9, "Включение МОД(5)-А", 0x015, "Включить комплект А / отключить комплект Б (МОД(5)-Б) (резерв)", "резерв\n\n", 12},
	{10, "Включение МОД(5)-Б", 0x01B, "Включить комплект Б / отключить комплект А (МОД(5)-А) (резерв)", "резерв\n\n", 13},
	{11, "Включение МОД(6)-А", 0x016, "Включить комплект А / отключить комплект Б (МОД(6)-Б) (резерв)", "резерв\n\n", 14},
	{12, "Включение МОД(6)-Б", 0x01C, "Включить комплект Б / отключить комплект А (МОД(6)-А) (резерв)", "резерв\n\n", 15},
	{13, "Включение МОД(7)-А", 0x017, "Включить комплект А / отключить комплект Б (МОД(7)-Б) (резерв)", "резерв\n\n", 16},
	{14, "Включение МОД(7)-Б", 0x01D, "Включить комплект Б / отключить комплект А (МОД(7)-А) (резерв)", "резерв\n\n", 17},
	{15, "Включение МОД(8)-А", 0x010, "Включить комплект А / отключить комплект Б (МОД(8)-Б) (резерв)", "резерв\n\n", 18},
	{16, "Включение МОД(8)-Б", 0x01E, "Включить комплект Б / отключить комплект А (МОД(8)-А) (резерв)", "резерв\n\n", 19},

	{17, "Откл. тест ОЗУ-И1", 0x0401, "Отключение текущего режима 'ТЕСТ ОЗУ-И1'", "NТЕСТ1=0\n\n", 34},
	{18, "Откл. тест ОЗУ-И2", 0x0402, "Отключение текущего режима 'ТЕСТ ОЗУ-И2'", "NТЕСТ2=0\n\n", 39},

	{19, "НОВ БАСИД", 0x0800, "Сбросить в исходное состояние датчик времени БАСИД 'МТ1'", "СУТ=0, ЧАС=0, МИН=0, СЕК=0\n\n", 20},
	{20, "Сброс СТЕ БАСИД", 0x2003, "Сброс регистра ошибок 'CTEn' в исходное состояние", "СТЕ1=0, СТЕ2=0\n\n", 23},
	{21, "Сброс CDE БАСИД", 0x200C, "Сброс регистра ошибок 'CDEn' в исходное состояние", "CDE1=0, CDE2=0\n\n", 24},

	{22, "Разрешение защищенных ДПУ БАСИД", 0x4001, "Разрешить исполнение защищенных ДПУ (ЗКУ)", "РИЗКУ=1\n\n", 21},
	{23, "Запрет защищенных ДПУ БАСИД", 0x4002, "Запретить исполнение защищенных ДПУ", "РИЗКУ=0\n\n", 22},

	{24, "Вкл. тест К170 ОЗУ-И1", 0xFE01, "Включить режим 'ТЕСТ ОЗУ-И1 К170'", "Код 170(10) в кадре или NТЕСТ1=3\n\n", 30},
	{25, "Вкл. тест К85 ОЗУ-И1", 0xFE02, "Включить режим 'ТЕСТ ОЗУ-И1 К85'", "Код 85(10) в кадре или NТЕСТ1=5\n\n", 31},
	{26, "Вкл. тест К2 ОЗУ-И1", 0xFE04, "Включить режим 'ТЕСТ ОЗУ-И1 К2'", "См. п. 5 в кадре или NТЕСТ1=9\n\n", 32},
	{27, "Вкл. тест К3 ОЗУ-И1", 0xFE08, "Включить режим 'ТЕСТ ОЗУ-И1 К2И'", "См. п. 5 в кадре или NТЕСТ1=17\n\n", 33},
	{28, "Вкл. тест К170 ОЗУ-И2", 0xFD01, "Включить режим 'ТЕСТ ОЗУ-И2 К170'", "Код 170(10) в кадре или NТЕСТ2=3\n\n", 35},
	{29, "Вкл. тест К85 ОЗУ-И2", 0xFD02, "Включить режим 'ТЕСТ ОЗУ-И2 К85'", "Код 85(10) в кадре или NТЕСТ2=5\n\n", 36},
	{30, "Вкл. тест К3 ОЗУ-И2", 0xFD04, "Включить режим 'ТЕСТ ОЗУ-И2 К2'", "См. п. 5 в кадре или NТЕСТ2=9\n\n", 37},
	{31, "Вкл. тест К2 ОЗУ-И2", 0xFD08, "Включить режим 'ТЕСТ ОЗУ-И2 К2И'", "См. п. 5 в кадре или NТЕСТ2=17\n\n", 38},

	{32, "Включение калибровки МОТД БАСИД", 0xFB21, "Включить режим калибровки для МОТД1", "TCCS1=1\n\n", 40},
	{33, "Отключение калибровки МОТД БАСИД", 0xEF21, "Отключить режим калибровки для МОТД1", "TCCS1=0\n\n", 41},
	{34, "Включение калибровки МОТД-01 БАСИД", 0xFB22, "Включить режим калибровки для МОТД2", "TCCS2=1\n\n", 42},
	{35, "Отключение калибровки МОТД-01 БАСИД", 0xEF22, "Отключить режим калибровки для МОТД2", "TCCS2=0\n\n", 43},
	{36, "Включение калибровки МОУ БАСИД", 0xFB31, "Включить режим калибровки для МОУ1", "ACCS1=1\n\n", 64},
	{37, "Отключение калибровки МОУ БАСИД", 0xEF31, "Отключить режим калибровки для МОУ1", "ACCS1=0\n\n", 65},
	{38, "Включение калибровки МОУ-01 БАСИД", 0xFB32, "Включить режим калибровки для МОУ2", "ACCS2=1\n\n", 66},
	{39, "Отключение калибровки МОУ-01 БАСИД", 0xEF32, "Отключить режим калибровки для МОУ2", "ACCS2=0\n", 67},

	{40, "Включение калибровки МОД5 БАСИД", 0xFB5, "Включить режим калибровки для МОД5", "резерв\n\n", 68},
	{41, "Отключение калибровки МОД5 БАСИД", 0xEF5, "Отключить режим калибровки для МОД5", "резерв\n\n", 69},
	{42, "Включение калибровки МОД6 БАСИД", 0xFB6, "Включить режим калибровки для МОД6", "резерв\n\n", 70},
	{43, "Отключение калибровки МОД6 БАСИД", 0xEF6, "Отключить режим калибровки для МОД6", "резерв\n\n", 71},
	{44, "Включение калибровки МОД7 БАСИД", 0xFB7, "Включить режим калибровки для МОД7", "резерв\n\n", 72},
	{45, "Отключение калибровки МОД7 БАСИД", 0xEF7, "Отключить режим калибровки МОД7", "резерв\n\n", 73},
	{46, "Включение калибровки МОД8 БАСИД", 0xFB8, "Включить режим калибровки для МОД8", "резерв\n\n", 74},
	{47, "Отключение калибровки МОД8 БАСИД", 0xEF8, "Отключить режим калибровки для МОД8", "резерв\n\n",75},

	{48, "Разрешение записи в ППЗУ БАСИД", 0xBF01, "Разрешить запись данных в перепрограммируемое ПЗУ БАСИД", "ЗАП_ПЗУ=1\n", 128},
	{49, "Запрет записи в ППЗУ БАСИД", 0xBF02, "Запретить запись данных в перепрограммируемое ПЗУ БАСИД", "ЗАП_ПЗУ=0\n", 129},

	{50, "Ускорение счетчика", 0x4801, "Ускорение счетчика", "Команда 1\n"},
	{51, "Возвращение счетчика в нормальное состояние", 0x4802, "Возвращение счетчика в нормальное состояние", "Команда 2\n"}
};

///////////////////////////// ОБРАБОТКА ПРИНЯТЫХ ДАННЫХ /////////////////////////////

void update_scc(unsigned short new_code) {	
	scc_count++;

	*(volatile unsigned short*)(0x8406) = new_code << 8;
	*(volatile unsigned short*)(0x8408) = (scc_old[1] << 8) | scc_old[0];
	*(volatile unsigned short*)(0x840A) = (scc_old[3] << 8) | scc_old[2];
	*(volatile unsigned short*)(0x840C) = (scc_count << 8) | scc_old[4];

	scc_old[4] = scc_old[3]; 
	scc_old[3] = scc_old[2]; 
	scc_old[2] = scc_old[1]; 
	scc_old[1] = scc_old[0]; 
	scc_old[0] = new_code; 
}

// Обработка ДПУ
void handle_dpu() {

	unsigned char i;
	volatile unsigned short dpu_value;
	dpu_value = *(volatile unsigned short*)(0x0F00 + 2);
	
	for (i = 0; i < DPUSize; i++)
		if (dpu_value == dpu[i].code) {
			*(volatile unsigned short*)(COMMON_DPU_ADDRESS) = dpu[i].id;
			update_scc(dpu[i].scc_code);
			break;
		}

	if (dpu_value == 0x4001) 
		epm = 1 << 7;
	else if (dpu_value == 0x4002) 
		epm = 0 << 7;

	if (dpu_value == 0xBF01) 
		epm = 1 << 1;
	else if (dpu_value == 0xBF02)
		epm = 0 << 1;

	*(volatile unsigned short*)(EPM_ADDRESS) = epm;
}

unsigned char calculate_crc8(unsigned char* data, unsigned char length) {
	unsigned char crc = 0x00;
	unsigned char i;
	for (i = 0; i < length; i++)
		crc = crc8_table[crc ^ data[i]];
	return crc;
}

unsigned char check_identifier_address(unsigned short address) {
	volatile unsigned short id_ptr = *(volatile unsigned short*)(address + 2);

	if (address == MOD1_ID_SUBADDR1 || address == MOD1_ID_SUBADDR2)
		return (id_ptr == MOD1_ID) ? 1 : 0;
	else if (address == MOD2_ID_SUBADDR3 || address == MOD2_ID_SUBADDR4)
		return (id_ptr == MOD2_ID) ? 1 : 0;
	else if (address == MOD3_ID_SUBADDR5 || address == MOD3_ID_SUBADDR6)
		return (id_ptr == MOD3_ID) ? 1 : 0;
	else if (address == MOD4_ID_SUBADDR7 || address == MOD4_ID_SUBADDR8)
		return (id_ptr == MOD4_ID) ? 1 : 0;
	return 0;
}

// Обработка конфигураций
void handle_config_packet(unsigned char subaddr) {
	
	volatile unsigned short address;
	volatile unsigned short temp;
	volatile unsigned char i;
	unsigned char calculated_crc;
	unsigned char received_crc;
	unsigned char data[62];
	
	address = RT_RECEIVE_BASE + RT_SUBADDR_SIZE * subaddr;
	for (i = 0; i < 31; i++) {
		temp = *(volatile unsigned short*)(address + i * 2 + 2); // С учетом 32 СД
		data[i * 2] = (unsigned char)(temp >> 8);
		data[i * 2 + 1] = (unsigned char)(temp & 0x00FF);
	}
	
	temp = *(volatile unsigned short*)(address); // 32 СД по начальному адресу
	calculated_crc = calculate_crc8(data, 31 * 2);
	received_crc = (unsigned char)(temp & 0x00FF);

	if (calculated_crc == received_crc && check_identifier_address(address) == 1)
		BSI_to_AD(subaddr, 30); // СД2 - СД31 (СД1 и СД 32 пропускается внутри функции)
	else
		write_data_to_addr(0x0001, ERROR_CONFIG_ADDRESS); // не следует ли прибавить значение?
}

void handle_VD_option_2() {
	
	volatile unsigned short MKO_VD2_addr, ad_addr;
	volatile unsigned short temp_value;
	volatile unsigned char i;

	MKO_VD2_addr = RT_TRANSMIT_BASE + RT_SUBADDR_SIZE * 29; // 30ый подедрес МКО для последующего размещения данных
	ad_addr = AD_BASE_T + *(volatile unsigned short*)(RT_RECEIVE_BASE + RT_SUBADDR_SIZE * 29 + 2); // значение адреса
	
	if (ad_addr < 0x200 + AD_BASE_T) // ДТИ
		write_data_to_addr(0xFFFF, SEMAPHORE_ADDRESS); // установка семафора
    
	for (i = 0; i < 31; i++) { // непосредственная запись памяти
		if (ad_addr != 0x42C + AD_BASE_T && ad_addr != 0x42E + AD_BASE_T) { // обычные адреса
			temp_value = *(volatile unsigned short*)(ad_addr + i * 2);
			*(volatile unsigned short*)(MKO_VD2_addr + i * 2 + 2) = temp_value;
		}
		else { // исключительные адреса
			if (ad_addr == 0x42C + AD_BASE_T) {
				temp_value = *(volatile unsigned short*)(0x1000 + AD_BASE_T);
				*(volatile unsigned short*)(MKO_VD2_addr + i * 2 + 2) = temp_value;
			}
			else {
				temp_value = *(volatile unsigned short*)(0x1002 + AD_BASE_T);
				*(volatile unsigned short*)(MKO_VD2_addr + i * 2 + 2) = temp_value;
			}
		}
	}

	// Учет 32ого СД
	temp_value = *(volatile unsigned short*)(ad_addr + 31 * 2);
	*(volatile unsigned short*)(MKO_VD2_addr) = temp_value;

	if (*(volatile unsigned short*)(SEMAPHORE_ADDRESS) == 0xFFFF)
		write_data_to_addr(0x0000, SEMAPHORE_ADDRESS); // сброс семафора
}

void handle_mko_packet(unsigned char subaddr) {
	
	switch (subaddr) {
		case 27:	// ДПУ
			handle_dpu();
			break;
		case 29:	// 2ой вариант работы в режиме ВД
			handle_VD_option_2();
			break;
		default:		// Конфигурации
			handle_config_packet(subaddr);
			break;
	}
}
