#include "stm32f10x_spi.h" 
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "Uart_Lib.h" 
#include "System_Lib.h" 

void My_Spi1_Init()
{
  /* GPIO Ports Clock Enable */
	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//Configure GPIO pin 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7 ;	//MOSI,SCK	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	//	MISO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 10;
	
	SPI_Init(SPI1,&SPI_InitStructure);
	
	SPI_Cmd(SPI1, ENABLE);
	
	// Настройка дополнительных выводов (CS,WR, и прочих если нужно) 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;	              //NSS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	#define NSS_SET       GPIOA->BSRR=GPIO_BSRR_BS4;	
  #define NSS_RESET     GPIOA->BSRR=GPIO_BSRR_BR4;	
	#define CLK_SET       GPIOA->BSRR=GPIO_BSRR_BS5;	
  #define CLK_RESET     GPIOA->BSRR=GPIO_BSRR_BR5;	
	
	#define WS_SET       GPIOB->BSRR=GPIO_BSRR_BS0;	
  #define WS_RESET     GPIOB->BSRR=GPIO_BSRR_BR0;	
		
}


void My_Spi2_Init()
{
  /* GPIO Ports Clock Enable */
	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//Configure GPIO pin 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15 ;	//MOSI, SCK	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;	//	MISO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 10;
	
	SPI_Init(SPI2,&SPI_InitStructure);
	
	SPI_Cmd(SPI2, ENABLE);
}



void Send_Spi(SPI_TypeDef* SPIx , unsigned char data)
{
	
while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_BSY) == SET){}
SPI_I2S_SendData(SPIx,data);	
while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_BSY) == SET){}	
}

void Transmit_Spi(SPI_TypeDef* SPIx , unsigned char *data, unsigned char count)
{
unsigned char i;

for(i=0;i<count;i++){	
while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_BSY) == SET){}
SPI_I2S_SendData(SPIx,*data++);
	}
}

unsigned char Read_Spi(SPI_TypeDef* SPIx)
{
while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_BSY) == SET){}

return  SPI_I2S_ReceiveData(SPIx);
}

void Exchange_Spi(SPI_TypeDef* SPIx , unsigned char *data, unsigned char count)
{
unsigned char i,j;
	
while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_BSY) == SET){}	

	
NSS_RESET;
	
	
for(i=0;i<count-1;i++){	
SPI_I2S_SendData(SPIx,*data);	
while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_BSY) == SET){}
*data = SPI_I2S_ReceiveData(SPIx);
*data++;	
 for(j=0;j<20;j++)	__NOP;		
NSS_SET;	
for(j=0;j<50;j++)	__NOP;	
NSS_RESET;		
	}

SPI_I2S_SendData(SPIx,*data);	
while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_BSY) == SET){}
*data = SPI_I2S_ReceiveData(SPIx);
*data++;		
for(j=0;j<20;j++)	__NOP;
			
NSS_SET;	
for(j=0;j<100;j++)	__NOP;	
			
NSS_RESET;	
}


void Send_Spi_ForAnalogDAC(SPI_TypeDef* SPIx , unsigned short int DataLeft,unsigned short int DataRigth)
{
	
unsigned short int l,r;

r=(DataRigth & 0x8000)|(0x7fff-(DataRigth & 0x7fff));
l=(DataLeft  & 0x8000)|(0x7fff-(DataLeft  & 0x7fff));	
			
while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_BSY) == SET){}			
//for(i=0;i<10;i++)	__NOP;	
WS_RESET;
SPI_I2S_SendData(SPIx,r);
	
while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_BSY) == SET){}
//for(i=0;i<10;i++)	__NOP;	
WS_SET;		
SPI_I2S_SendData(SPIx,l);	
while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_BSY) == SET){}	
		
//for(i=0;i<10;i++)	__NOP;	
WS_RESET;
SPI_I2S_SendData(SPIx,r);
	
while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_BSY) == SET){}
//for(i=0;i<10;i++)	__NOP;	
WS_SET;		
SPI_I2S_SendData(SPIx,l);	
while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_BSY) == SET){}	
}
