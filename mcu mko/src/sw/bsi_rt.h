#ifndef __BSI_RT
#define __BSI_RT

#define BSI_TXD_LEVEL   1    //��� ������������� ������-����������� EL-12 TXDLEV = 1, ��� ������ - 0

#ifndef dXTAL
  #define dXTAL 20000000UL
#endif

#define FXTALL_MHz      (unsigned char)(dXTAL/1000000UL)
#include "K1874VE7T.h"

// ����������� �������� ������� �������� ����������
//--------------------------------------------------------------------------------------------------------
/*#define CORRECT_DPU_ADDRESS             0x8800 // 0x2880
#define COMMON_DPU_ADDRESS              0x8240 // 0x22C0
#define ERROR_CONFIG_ADDRESS            0x8234 // 0x22B4
#define SEMAPHORE_ADDRESS        		0x8802 // 0x2882*/

// установить данные значения как основные
#define AD_BASE_T               0x8000 // 0x2080
#define AD_BASE_R               0x8000 // 0x2080

#define RT_SUBADDR_SIZE                 0x40
#define RT_RECEIVE_BASE                 0x0840
#define RT_TRANSMIT_BASE                0x1040
#define RT_RECEIVE_SPECIAL_ADDR         0x0FC0
#define RT_TRANSMIT_SPECIAL_ADDR        0x17C0

#define delays __NOP();__NOP();__NOP();__NOP();__NOP();
// ���� ������ ����������

enum _CODE_COM {
        SYNCR = 1,              //01h   �������������
        SEND_REPLY_WORD,        //02h   �������� �������� �����
        START_SELF_CTRL,        //03h   ������ ������������
        BLOCK_TRANSMITTER,      //04h   ����������� ����������
        UNLOCK_TRANSMITTER,     //05h   �������������� ����������
        BLOCK_SYMPTOM_RT,       //06h   ����������� ������� ������������� ���������� ���������
        UNLOCK_SYMPTOM_RT,      //07h   �������������� ������� ������������� ���������� ���������
        SET_RT_INITIAL_STATE,   //08h   ���������� ��������� �������� � �������� ���������
        SEND_VECTOR_WORD = 16,  //10h   �������� ��������� �����
        SYNCR_WORD_DATA,        //11h   ������������� �� ������ ������
        SEND_LAST_COM,          //12h   �������� ��������� �������
        SEND_WORD_SD,            //13h   �������� ����� ���������� ������� �������� (���) ���������� ��������� � ����������� ����
        BLOCK_SELECT_TRANSMITTER,//14h  ����������� ��������� ����������
        UNLOCK_SELECT_TRANSMITTER//15h  �������������� ��������� ����������

};

void write_addr_to_addr(unsigned short addr_from, unsigned short addr_to);
void write_data_to_addr(unsigned short data, unsigned short addr);
//������� ������������� ���� � ������ ���������� ���������
void init_bsi_terminal(unsigned char taddr);
//������� ������ ���� ������ �� ��������� saddr � ������ data
void bsi_rx(const unsigned char saddr,unsigned char count, unsigned short* data);
//������� ������ ������� ���� ������ � �������� saddr
void bsi_tx(const unsigned char saddr,unsigned char count, unsigned short* data);
void bsi_subaddr_init(unsigned char subaddr);
//������� ��������� ����� ������, ��������������� ���� ������� �� ������������ �����
unsigned short bsi_read_special(const unsigned char sbcode);
//������� ���������� ����� ������ � ����������� ���� � �������, ��������������� ���� �������
void bsi_write_special(const unsigned char cod,unsigned short data);
// ������� ��������� ����� ������ ��� ��������� subaddr. � ������� ���� ������������ ��������, � ������� - ����� ����� ������
void store_at_address(unsigned short start_addr, unsigned char num_sd, unsigned char count, unsigned short* data);
//������� ��������� ����� ������ ��� ���� ����������.
void bsi_data_init();
void pre_write_memory(const unsigned char saddr,unsigned char count, unsigned short data);
void AD_to_BSI(unsigned char saddr);
void BSI_to_AD(unsigned char saddr, unsigned char count);


void init_bsi_terminal_alt() ;

#endif /* __BSI_RT */

/* END OF FILE bsi_rt.h */

