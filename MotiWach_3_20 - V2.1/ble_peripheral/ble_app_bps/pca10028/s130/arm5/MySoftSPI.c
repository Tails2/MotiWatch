#include "MySoftSPI.h"


#ifdef Used_Stm32_M3 
		
		#include "stm32f10x_gpio.h"
		
		#define Lcd_Off nrf_gpio_port_set(CS_Port,1<<(CS_Pin & 0x07));
		#define Lcd_On nrf_gpio_port_clear(CS_Port,1<<(CS_Pin & 0x07));
		
		#define Soft_SPI_Clk_Set SCK_Port->BSRR= 1<<SCK_Pin;
		#define Soft_SPI_Clk_Reset SCK_Port->BSRR=1<<(SCK_Pin+16);

		#define Soft_SPI_Mosi_Set MOSI_Port->BSRR=1<<MOSI_Pin; 
		#define Soft_SPI_Mosi_Reset MOSI_Port->BSRR=1<<(MOSI_Pin+16);
		
		#ifdef SPI_Duplex
			#define Soft_SPI_Miso_In  (MISO_Port->IDR & (1<<MISO_Pin)) == 1<<MISO_Pin 
		#endif
		
#endif

#ifdef Used_NRF51 
	
		#include  "nrf_gpio.h"

		#define Soft_SPI_Clk_Set NRF_GPIO->OUTSET=1<<SCK_Pin;  //nrf_gpio_port_set(SCK_Port,1<<(SCK_Pin & 0x07));	
		#define Soft_SPI_Clk_Reset NRF_GPIO->OUTCLR=1<<SCK_Pin;  //nrf_gpio_port_clear(SCK_Port,1<<(SCK_Pin & 0x07));

		#define Soft_SPI_Mosi_Set NRF_GPIO->OUTSET=1<<MOSI_Pin;  //nrf_gpio_port_set(MOSI_Port,1<<(MOSI_Pin & 0x07));
		#define Soft_SPI_Mosi_Reset NRF_GPIO->OUTCLR=1<<MOSI_Pin;  //nrf_gpio_port_clear(MOSI_Port,1<<(MOSI_Pin & 0x07));
		
		#ifdef SPI_Duplex
			#define Soft_SPI_Miso_In (nrf_gpio_port_read(MISO_Port) & (1<<(MISO_Pin & 0x07))) == 1<<(MISO_Pin & 0x07)
		#endif
		
#endif

void Spi_While_us(uint32_t p)
{
	uint32_t i,j;
	
	//j=p*(SoftSPI_Sistem_Clock / 1000000); 
	for (i=0;i<p;i++){}
}
void Spi_While_ms(uint32_t p)
{
	uint32_t i;
	for (i=0;i<p;i++){
	Spi_While_us(1000);
	}
}

//------------------------------------------------
void Soft_Spi_GPIO_Init(void)
{

#ifdef Used_Stm32_M3 	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	//Configure GPIO pin 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
	GPIO_InitStructure.GPIO_Pin = (1<<SCK_Pin) | (1<<MOSI_Pin) ;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	#ifdef SPI_Duplex 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;         
		GPIO_InitStructure.GPIO_Mode = 1<<MISO_Pin;
		GPIO_Init(GPIOC, &GPIO_InitStructure);		
	#endif	
	
#endif	
	 
	 
#ifdef Used_NRF51 	
	//Configure GPIO pin 
	nrf_gpio_cfg_output(SCK_Pin);
	nrf_gpio_cfg_output(MOSI_Pin);
	
	#ifdef SPI_Duplex 
		nrf_gpio_cfg_input(MISO_Pin,GPIO_PIN_CNF_PULL_Pullup);	
	#endif	
	
#endif		 
 
}

//------------------------------------------------
void Soft_Spi_GPIO_DeInit(void)
{	 
#ifdef Used_NRF51 	
	//Configure GPIO pin 
	nrf_gpio_cfg_input(SCK_Pin,GPIO_PIN_CNF_PULL_Disabled);
	nrf_gpio_cfg_input(MOSI_Pin,GPIO_PIN_CNF_PULL_Disabled);	
	#ifdef SPI_Duplex 
		nrf_gpio_cfg_input(MISO_Pin,GPIO_PIN_CNF_PULL_Disabled);	
	#endif	
	
#endif		 
 
}

//------------------------------------------------
void Soft_Spi_TransmitSameBit(uint8_t bit, uint16_t count){
uint16_t i,j;
uint8_t	outByte; 
	Soft_SPI_Clk_Reset;
	count=count<<3;
	if(bit){Soft_SPI_Mosi_Set}else{Soft_SPI_Mosi_Reset};
	for (i=0;i<count;i++){
			Soft_SPI_Clk_Set
			__NOP;
			Soft_SPI_Clk_Reset
			
		}
	Soft_SPI_Mosi_Reset
}

//------------------------------------------------
void Soft_Spi_TransmitSameByte(uint8_t byte, uint16_t count){
uint16_t i,j;
uint8_t	outByte; 
	Soft_SPI_Clk_Reset;		
	for (i=0;i<count;i++){
		outByte=byte;
		j=8;
		while(j){
			if(outByte&0x80){Soft_SPI_Mosi_Set}else{Soft_SPI_Mosi_Reset};
			outByte=outByte<<1;
			Soft_SPI_Clk_Set
			Soft_SPI_Clk_Reset
			j--;
		}
	}
	Soft_SPI_Mosi_Reset
}

//------------------------------------------------
void Soft_Spi_Transmit(uint8_t *txData, uint16_t count){
	
uint16_t i,j;
uint8_t	outByte; 
	Soft_SPI_Clk_Reset;	 
	for (i=0;i<count;i++){
		outByte=*txData++;
		j=8;
		while(j){
			if(outByte&0x80){Soft_SPI_Mosi_Set}else{Soft_SPI_Mosi_Reset};
			outByte=outByte<<1;
			Soft_SPI_Clk_Set
			Soft_SPI_Clk_Reset
			j--;
		}
	}
	Soft_SPI_Mosi_Reset
}

#ifdef SPI_Duplex 
	//------------------------------------------------
	void Soft_Spi_TransmitReceive(uint8_t *txData, uint16_t txcount,uint8_t *rxData,uint16_t rxcount)
	{
	uint16_t i,j,allcount;
	uint8_t	outByte,inByte; 

		Soft_Spi_Transmit(txData,txcount);
		
		for (i=0;i<rxcount;i++){	
			Soft_SPI_Clk_Reset;	
			Soft_SPI_Mosi_Reset
			for(j=0;j<8;j++){
				inByte=inByte<<1;
				if(Soft_SPI_Miso_In){inByte |= 0x01;}				
			//	Spi_While_us(2);
				Soft_SPI_Clk_Set
				//Spi_While_us(4);
				Soft_SPI_Clk_Reset
			//	Spi_While_us(2);
			}
		rxData[i]=inByte;	
		}
		Soft_SPI_Mosi_Reset
	}

	//------------------------------------------------
	void Soft_Spi_Receive(uint8_t *rxData, uint16_t count)
	{		
		Soft_Spi_TransmitReceive(rxData,0,rxData,count);
	}

#endif	