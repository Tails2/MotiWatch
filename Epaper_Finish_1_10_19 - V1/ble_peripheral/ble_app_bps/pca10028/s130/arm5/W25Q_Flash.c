//В качестве контроллера я использовал stm32f4 discovery.
//SPI был настроен вот так.

#include "W25Q_Flash.h"
#include "nrf_gpio.h"

#define Mem_Off nrf_gpio_port_set(CS_Mem_Port,1<<(CS_Mem_Pin & 0x07));
#define Mem_On nrf_gpio_port_clear(CS_Mem_Port,1<<(CS_Mem_Pin & 0x07));

void W25Q_DeInit(){
	nrf_gpio_cfg_input(CS_Mem_Pin,GPIO_PIN_CNF_PULL_Pullup);
}

void W25Q_SpiSendData(uint8_t * Txdata,uint16_t Txlength){
	Mem_On;
	uint8_t * Rxdata=Txdata;
	uint16_t Rxlength = 0;
	SpRxTx_Interfase;
	Mem_Off;
}

uint8_t W25Q_SpiReadReg(uint8_t Reg){
	uint8_t rxbyte;
	uint8_t *Txdata = &Reg;
	uint8_t *Rxdata = &rxbyte;
	uint16_t Txlength = 1;
	uint16_t Rxlength = 1;	
	Mem_On;
	SpRxTx_Interfase;
	Mem_Off;
	return rxbyte;
}

void whileStatus1_busy(){
	uint8_t stt=1;
	uint8_t us=100;
	while(stt&0x01){
		stt=W25Q_SpiReadReg(W25_READ_STATUS_1);
		While_us_Interfase
	}
}

uint8_t W25Q_WriteEnadle(void){
		
		W25Q_SendByte(W25_WRITE_ENABLE);
	  return((W25Q_SpiReadReg(W25_READ_STATUS_1)&0x02)>>1);
}

void W25Q_ReadData(uint8_t *Rxdata,uint32_t Adr,uint16_t Rxlength){
	uint8_t Txdata[4];

	Txdata[0] = W25_READ;
	Txdata[1] = (Adr>>16)&0xff;
	Txdata[2] = (Adr>>8)&0xff;
	Txdata[3] = Adr&0xff;
	
	uint16_t Txlength =4;
	whileStatus1_busy();
	Mem_On;
	SpRxTx_Interfase;   
	Mem_Off;
}

void W25Q_WritePage(uint8_t *txdata,uint32_t Adr,uint16_t length){
	uint8_t adrdata[4];
	uint16_t Txlength=4;
	uint16_t Rxlength=0;
	uint8_t *Rxdata=&adrdata[0];
	uint8_t *Txdata=&adrdata[0];;
		whileStatus1_busy(); 
		if(W25Q_WriteEnadle()){ 
		
			adrdata[0] = W25_PAGE_PROGRAMM;
			adrdata[1] = (Adr>>16)&0xff;
			adrdata[2] = (Adr>>8)&0xff;
			adrdata[3] = Adr&0xff;

			Mem_On;
			SpRxTx_Interfase;
			Txlength=length;
			Txdata = txdata;
			SpRxTx_Interfase;
			Mem_Off;
		}
}

void W25Q_SendByte(uint8_t byte){
	Mem_On;
	uint8_t *Rxdata=&byte;
	uint8_t *Txdata=&byte;
	uint16_t Txlength = 1;
	uint16_t Rxlength = 0;
	SpRxTx_Interfase;
	Mem_Off;
}

void W25Q_Flash_init(void) {
		nrf_gpio_cfg_output(CS_Mem_Pin);
	  W25Q_Flash_reset();
}

//Сброс производится в два присеста. Сначала нужно разрешить сброс, а уже только после этого сбрасывать.
void W25Q_Flash_reset(void)
{   
		uint8_t data[2] = {W25_ENABLE_RESET,W25_RESET};
		W25Q_SpiSendData(&data[0],2);
}
//Перед программированием нужно стереть память. Это можно сделать при помощи команды стирания сектора 4 Кбайт.
void W25Q_Flash_Sektor_erase(uint32_t Adres)
{   
		uint8_t data[4];
	
		whileStatus1_busy(); 
		if(W25Q_WriteEnadle()){ 
			data[0] = 0x20;
			data[1] = (Adres>>16)&0xff;
			data[2] = (Adres>>8)&0xf0;
			data[3] = 0;	
			whileStatus1_busy(); 
			W25Q_SpiSendData(&data[0],4);
		}
}

//Перед программированием нужно стереть память. Это можно сделать при помощи команды стирания чипа.
void W25Q_Flash_erase(void)
{
	whileStatus1_busy(); 
	if(W25Q_WriteEnadle()){   
   W25Q_SendByte(W25_CHIP_ERASE);
	}
}