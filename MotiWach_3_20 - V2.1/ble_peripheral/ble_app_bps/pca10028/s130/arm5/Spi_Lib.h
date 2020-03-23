#include "stm32f10x_spi.h" 
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

void My_Spi1_Init();
void My_Spi2_Init();
void Send_Spi(SPI_TypeDef* SPIx , unsigned char data);
void Transmit_Spi(SPI_TypeDef* SPIx , unsigned char *data, unsigned char count);
unsigned char Read_Spi(SPI_TypeDef* SPIx);
void Send_Spi_ForAnalogDAC(SPI_TypeDef* SPIx , unsigned short int DataLeft,unsigned short int DataRigth);
void Exchange_Spi(SPI_TypeDef* SPIx , unsigned char *data, unsigned char count);



