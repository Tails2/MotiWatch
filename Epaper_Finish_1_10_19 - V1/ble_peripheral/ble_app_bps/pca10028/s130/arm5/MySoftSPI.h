#define SoftSPI_Sistem_Clock 1000000

//#define Used_Stm32_M3 
#define Used_NRF51
#define SPI_Duplex

#define SCK_Port NRF_GPIO_PORT_SELECT_PORT0  //GPIOC
#define SCK_Pin 5

#define MOSI_Port NRF_GPIO_PORT_SELECT_PORT0 //GPIOC
#define MOSI_Pin 4

#define MISO_Port NRF_GPIO_PORT_SELECT_PORT0 //GPIOC
#define MISO_Pin 3

//------------------------------------------------------------

#ifdef Used_Stm32_M3 
	#include "stm32f10x.h" 
#endif	
	
#ifdef Used_NRF51 
	#include "ble.h"
#endif	

void Soft_Spi_GPIO_Init(void);
void Soft_Spi_GPIO_DeInit(void);
void Soft_Spi_Transmit(uint8_t *txData, uint16_t count);
void Soft_Spi_TransmitSameBit(uint8_t bit, uint16_t count);
void Soft_Spi_TransmitSameByte(uint8_t byte, uint16_t count);
void Spi_While_us(uint32_t p);
void Spi_While_ms(uint32_t p);

#ifdef SPI_Duplex 
	void Soft_Spi_TransmitReceive(uint8_t *txData, uint16_t txcount,uint8_t *rxData,uint16_t rxcount);
	void Soft_Spi_Receive(uint8_t *rxData, uint16_t count);
#endif	