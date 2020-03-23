//
//Interfase -------------
//#include "MySoftSPI.h"
//#define Key_click_Interfase KeyPress(keyNow);
//-----------------------

//Interfase -------------(функция задержки)
//#include "nrf_delay.h"
//#define delayInterfase nrf_delay_ms(1);
#include "MySoftSPI.h"
#define delayInterfase Spi_While_ms(1);
//-----------------------

#include "MyBLE.h" 

#define my_key_count 3
#define my_key_time 10

#define my_key1_port NRF_GPIO_PORT_SELECT_PORT3  //GPIOC
#define my_key1_pin 24

#define my_key2_port NRF_GPIO_PORT_SELECT_PORT3  //GPIOC
#define my_key2_pin 25

#define my_key3_port NRF_GPIO_PORT_SELECT_PORT3  //GPIOC
#define my_key3_pin 28

#define key_Port_array {my_key1_port,my_key2_port,my_key3_port}

#define key_Pin_array {my_key1_pin,my_key2_pin,my_key3_pin}

//#define my_key1_In (nrf_gpio_port_read(my_key1_port)>>(my_key1_pin & 0x07))&0x01
//#define my_key2_In (nrf_gpio_port_read(my_key2_port)>>(my_key2_pin & 0x07))&0x01
//#define my_key3_In (nrf_gpio_port_read(my_key3_port)>>(my_key3_pin & 0x07))&0x01

//#define key_array_read (my_key1_In | (my_key2_In<<1)|(my_key3_In<<2))


typedef struct
{
	uint8_t old_key;
	uint8_t keyNow;
	uint8_t keyEvent;
	uint8_t keyReadiOn;
	uint8_t keyReadiOff;
	int8_t keycount[my_key_count];
	uint8_t keyOncount[my_key_count];
} My_Key_TypeDef;


void my_KeyInit(void);
void my_KeyDeInit(void);
uint8_t my_KeySkan(void);
uint8_t get_my_Key(void);
uint8_t get_my_KeyEvent(uint8_t rst);
uint8_t my_KeyRead(void);
uint8_t my_KeyClk(void);

