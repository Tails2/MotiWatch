//
//Interfase -------------
#include "MySoftSPI.h"
#define SpRxTx_Interfase Soft_Spi_TransmitReceive(Txdata,Txlength,Rxdata,Rxlength);
#define While_us_Interfase Spi_While_us(us);
//-----------------------




#define CS_Mem_Port NRF_GPIO_PORT_SELECT_PORT1  //GPIOC
#define CS_Mem_Pin 9 //10


#define    W25_WRITE_DISABLE  0x04
#define    W25_WRITE_ENABLE  0x06  
#define    W25_READ_STATUS_1  0x05
#define    W25_READ_STATUS_2  0x35
#define    W25_READ_STATUS_3  0x15 
#define    W25_WRITE_STATUS_1  0x01
#define    W25_WRITE_STATUS_2  0x31
#define    W25_WRITE_STATUS_3  0x11  
#define    W25_CHIP_ERASE  0xc7 //0x60   
#define    W25_GET_DEV_ID  0x90
#define    W25_GET_JEDEC_ID  0x9f    
#define    W25_ENABLE_RESET  0x66
#define    W25_RESET  0x99
#define    W25_PAGE_PROGRAMM  0x02
#define    W25_READ  0x03
#define		 W25_IDENT_ERROR 1
#define		 W25_OK 0

#define busy  1
#define write_enable  1
#define block_protect  3
#define top_bot_ptotect  1
#define sector_protect  1
#define status_reg_protect0  1

void W25Q_DeInit();
void W25Q_Flash_init();
void W25Q_Flash_reset();
void W25Q_Flash_erase();
uint8_t W25Q_Flash_read_status_1();
uint8_t W25Q_WriteEnadle(void);
void whileStatus1_busy();
void W25Q_ReadData(uint8_t *data,uint32_t Adr,uint16_t Rxlength);
void W25Q_WritePage(uint8_t *txdata,uint32_t Adr,uint16_t length);
void W25Q_SendByte(uint8_t byte);
void W25Q_Flash_Sektor_erase(uint32_t Adres);