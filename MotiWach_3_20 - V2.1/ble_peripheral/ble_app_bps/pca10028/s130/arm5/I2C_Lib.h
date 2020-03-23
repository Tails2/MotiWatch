
#include  "nrf_gpio.h"

#define SET_SDA        nrf_gpio_port_set(NRF_GPIO_PORT_SELECT_PORT0,0x20);		//MDR_PORTC->RXTX |= PORT_Pin_1;  //GPIOB->BSRR = GPIO_BSRR_BS7;
#define RESET_SDA      nrf_gpio_port_clear(NRF_GPIO_PORT_SELECT_PORT0,0x20);		//MDR_PORTC->RXTX &= (~PORT_Pin_1);  //GPIOB->BSRR = GPIO_BSRR_BR7;

#define SET_SCL        nrf_gpio_port_set(NRF_GPIO_PORT_SELECT_PORT0,0x40);		//MDR_PORTC->RXTX |= PORT_Pin_0;  //GPIOB->BSRR = GPIO_BSRR_BS6;
#define RESET_SCL      nrf_gpio_port_clear(NRF_GPIO_PORT_SELECT_PORT0,0x40);		//MDR_PORTC->RXTX &= (~PORT_Pin_0);  //GPIOB->BSRR = GPIO_BSRR_BR6;

#define TX_SDA         nrf_gpio_cfg_output(5);   //MDR_PORTC->OE |= 0x0002;  //GPIOB->CRL  = (GPIOB->CRL & 0x0fffffff)| 0x20000000;  
#define RX_SDA         nrf_gpio_cfg_input(5,GPIO_PIN_CNF_PULL_Pullup);       // MDR_PORTC->OE &= 0xfffd;  //GPIOB->CRL  = (GPIOB->CRL & 0x0fffffff)| 0x80000000;  

#define READ_SDA       nrf_gpio_pin_read(5)					//(MDR_PORTC->RXTX & 0x0002)==0x0002  //(GPIOB->IDR & 0x0080)==0x0080
		
		

			
			


//void My_I2C1_Init();
void My_TWI_Init();
//unsigned char Ram_SingleWrite_Data(I2C_TypeDef* I2Cx,unsigned char Slave_Adres,unsigned char Reg_Adres, unsigned char Data);
//unsigned char Ram_SingleRead_Data(I2C_TypeDef* I2Cx,unsigned char Slave_Adres,unsigned char Reg_Adres);
void Max9611_InitCurent_TWI(void);
unsigned char Max9611_ReadCurent_TWI(unsigned char Slave_Adres);
unsigned char Max9611_ReadVolt_TWI(unsigned char Slave_Adres);
unsigned int Max9611_ReadInt_TWI(unsigned char Slave_Adres,unsigned char Adr_byte );
unsigned char Max9611_ReadTerm_TWI(unsigned char Slave_Adres);
void Read_MPU6050(uint8_t *data);
void Wake_Up_MPU6050(void);
void mpu_poverOn(void);
void mpu_init(void);