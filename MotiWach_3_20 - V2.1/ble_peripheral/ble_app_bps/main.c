
#include "MyBLE.h"   
#include "MyePaper.h"  
#include "MySoftSPI.h"
#include "W25Q_Flash.h"
#include "My_Key_lib.h"
#include "MyePaperObjektFunction.h"

#include <stdbool.h>
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"

#include "nrf_gpio.h"
#include "nrf_drv_config.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"
#include <stdint.h>


#define PIN_IN1 24  //wakeUpKey
#define PIN_IN2 25	//wakeUpKey
#define PIN_IN3 28	//wakeUpKey


#define	PinPwr 7
#define	PinPwrOn	nrf_gpio_port_set(NRF_GPIO_PORT_SELECT_PORT0,0x80);	
//#define	PinPwr 8  // 7
//#define	PinPwrOn	nrf_gpio_port_set(NRF_GPIO_PORT_SELECT_PORT1,0x01);	//nrf_gpio_port_set(NRF_GPIO_PORT_SELECT_PORT0,0x80);	


#define COMPARE_COUNTERTIME_RTC0 3600UL  //1 hour
//#define COMPARE_COUNTERTIME_RTC1  3600UL  //1 hour
#define KeyEnableTimeMc 10000
		
uint8_t test1=0;
uint16_t key_activ=KeyEnableTimeMc;
uint8_t KeyEnable = 0;
uint8_t bleRead = 0;
const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(0); /**< Declaring an instance of nrf_drv_rtc for RTC0. */
//const nrf_drv_rtc_t rtc1 = NRF_DRV_RTC_INSTANCE(1);
static void rtc_handler(nrf_drv_rtc_int_type_t int_type);
//static void rtc1_handler(nrf_drv_rtc_int_type_t int_type);
void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);



__STATIC_INLINE void nrf_task_triger(uint32_t *NRF_Base, uint32_t task)
{
	*((volatile uint32_t *)((uint8_t *)NRF_Base + task)) = NRF_CLOCK_TASK_TRIGGER;
}

static void lfclk_config(void){
		nrf_clock_xtalfreq_set(CLOCK_CONFIG_XTAL_FREQ);
    nrf_clock_lf_src_set((nrf_clock_lf_src_t)CLOCK_CONFIG_LF_SRC);
    nrf_drv_common_irq_enable(POWER_CLOCK_IRQn, 3);
		__disable_irq();  
			*((volatile uint32_t *)((uint8_t *)NRF_CLOCK + NRF_CLOCK_TASK_LFCLKSTART)) = NRF_CLOCK_TASK_TRIGGER;
		nrf_task_triger((uint32_t*)NRF_CLOCK,NRF_CLOCK_TASK_LFCLKSTART);
		__enable_irq();
}


static void rtc_config(void){
    uint32_t err_code;

    //Initialize RTC instance rtc0
	  nrf_drv_rtc_config_t p_config;
		p_config.interrupt_priority=0x03;
		p_config.prescaler=0x0fff;
		p_config.reliable=0x00;
		p_config.tick_latency=0x00;
    err_code = nrf_drv_rtc_init(&rtc, &p_config, rtc_handler);
    APP_ERROR_CHECK(err_code);

    //Enable tick event & interrupt rtc0
    nrf_drv_rtc_tick_enable(&rtc,true);
		nrf_drv_rtc_overflow_enable(&rtc,true);
	 nrf_drv_rtc_int_enable(&rtc,NRF_RTC_INT_COMPARE0_MASK);

    //Set compare channel to trigger interrupt after COMPARE_COUNTERTIME seconds
    err_code = nrf_drv_rtc_cc_set(&rtc,0,COMPARE_COUNTERTIME_RTC0*RTC0_CONFIG_FREQUENCY,true);
    APP_ERROR_CHECK(err_code);

    //Power on RTC instance
    nrf_drv_rtc_enable(&rtc);
		nrf_drv_rtc_int_enable(&rtc,NRF_RTC_INT_COMPARE0_MASK);
		
		/*
		//Initialize RTC instanc ertc1
    err_code = nrf_drv_rtc_init(&rtc1, &p_config, rtc1_handler);
    APP_ERROR_CHECK(err_code);
		
	  //Enable tick event & interrupt rtc1
    nrf_drv_rtc_tick_enable(&rtc1,true);
		nrf_drv_rtc_int_enable(&rtc1,NRF_RTC_INT_COMPARE0_MASK);

    //Set compare channel to trigger interrupt after COMPARE_COUNTERTIME seconds
    err_code = nrf_drv_rtc_cc_set(&rtc1,0,COMPARE_COUNTERTIME_RTC1*RTC1_CONFIG_FREQUENCY,true);
    APP_ERROR_CHECK(err_code);

    //Power on RTC instance
    nrf_drv_rtc_enable(&rtc1);
		nrf_drv_rtc_int_enable(&rtc1,NRF_RTC_INT_COMPARE0_MASK);	
		*/
		
		
}

static void hendler_gpio_init(void){
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);

   //err_code = nrf_drv_gpiote_out_init(PIN_OUT, &out_config);
   // APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull = NRF_GPIO_PIN_NOPULL;

    err_code = nrf_drv_gpiote_in_init(PIN_IN1, &in_config, in_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(PIN_IN1, true);
	
	  err_code = nrf_drv_gpiote_in_init(PIN_IN2, &in_config, in_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(PIN_IN2, true);
		
		err_code = nrf_drv_gpiote_in_init(PIN_IN3, &in_config, in_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(PIN_IN3, true);
}
static void hendler_gpio_deinit(void){	
    nrf_drv_gpiote_in_uninit(PIN_IN1);
		nrf_drv_gpiote_in_event_disable(PIN_IN1);
	
	  nrf_drv_gpiote_in_uninit(PIN_IN2);
    nrf_drv_gpiote_in_event_disable(PIN_IN2);
		
		nrf_drv_gpiote_in_uninit(PIN_IN3);
    nrf_drv_gpiote_in_event_disable(PIN_IN2);
}

void powerDeInit(){
		nrf_gpio_cfg_input(PinPwr,GPIO_PIN_CNF_PULL_Disabled);
		Soft_Spi_GPIO_DeInit(); 
		MyePaper_GPIO_DeInit();	
		W25Q_DeInit();	
	//	Spi_While_ms(500);
}

void powerInit(){	 
	//Configure GPIO pin 
		nrf_gpio_cfg_output(PinPwr);
		PinPwrOn;
		Soft_Spi_GPIO_Init(); 
		MyePaperDualInit();
		W25Q_Flash_init();	
}


void Key_timeout(void){
		uint8_t key_now;
	//	hendler_gpio_deinit();
		my_KeyInit();
		while(my_KeyClk()){
			key_now = my_KeyRead();
		}
		
		if(key_now){
			bleRead=0;
			BLE_enable=0;
			key_activ=KeyEnableTimeMc;
			powerInit();
			LoadRamParams(); 
			KeyReaction(key_now,1);
			key_now=0;
			while(key_activ){//while(1){ //
				key_activ--;
				if(key_now){
					KeyReaction(key_now,0);
					key_activ=KeyEnableTimeMc;
					key_now=0;
				}
				if(my_KeyRead()){
					key_now=get_my_KeyEvent(0);				
				}	
				Spi_While_ms(10);
				if((BLE_enable)&&(key_activ>500))key_activ=500;
		}
		Save_params();
		powerDeInit();
	}
	my_KeyDeInit();	
//	hendler_gpio_init();	
}



static void rtc_handler(nrf_drv_rtc_int_type_t int_type){

    if (int_type == NRF_DRV_RTC_INT_COMPARE0)
    {
				//Save Counter
				nrf_drv_rtc_counter_clear(&rtc);
				powerInit();
			  LoadRamParams();
				for(uint8_t i = 0;i<16;i++){	
					if(CounterArray[i]!=0xffffffff){
						CounterArray[i]+=3600;	
					}
				}	
				EPD_1IN54_DrawObjStartScrin();	
				Save_params();
				powerDeInit();
    }
}

void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action){
	if(!KeyEnable){
		KeyEnable=1;
	//	Key_timeout();		
	}
}

/*int main(void){
	
	uint8_t buf[4];
	W25Q_Flash_init();	
	W25Q_ReadData(&buf[0],0,4);
	
	Ble_Init();
	    for (;;){
			BLE_power_manage();
			
			}
}
*/
int main(void)
{
	
	/*powerInit();
	MyePaperInit(1);	
	EPD_1IN54_Clear(0xff);
	
	EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);
  EPD_1IN54_TurnOnDisplayPart(); 	
	
	EPD_1IN54_DrawObjScrin(getScrin(0)); 
	EPD_1IN54_TurnOnDisplayPart(); 	
	Spi_While_ms(10000);
	EPD_1IN54_DrawObjScrin(getScrin(1)); 
	EPD_1IN54_TurnOnDisplayPart(); 
	Spi_While_ms(10000);
	EPD_1IN54_DrawObjScrin(getScrin(2)); 
	EPD_1IN54_TurnOnDisplayPart(); 	
	Spi_While_ms(10000);
	EPD_1IN54_DrawObjScrin(getScrin(3)); 
	EPD_1IN54_TurnOnDisplayPart(); 	
	Spi_While_ms(10000);
	EPD_1IN54_DrawObjScrin(getScrin(4)); 
	EPD_1IN54_TurnOnDisplayPart(); 
	
	while(1){};*/
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	currentScrinPosition=0;
	stackScrin[0]=1;
	paramsArray[0]=0;
	paramsArray[1]=0;
	paramsArray[2]=0;
	uint8_t buf[4];
	Soft_Spi_GPIO_Init(); 
	W25Q_Flash_init();	
	W25Q_ReadData(&buf[0],0,4);
	Soft_Spi_GPIO_DeInit(); 
	W25Q_DeInit();
	if(buf[0]!=0xff) {bleRead=buf[1];}
	else{
		my_KeyInit();
		nrf_gpio_cfg_output(PinPwr);
		PinPwrOn;
		Soft_Spi_GPIO_Init(); 	
		currentScrinPosition=0;
		stackScrin[currentScrinPosition]=0;	
	//	KeyReaction(0,1);
	//	Spi_While_ms(1000);
		EPD_1IN54_DrawObjStartScrin();
		
		bleRead=0;
		W25Q_Flash_init();	
		Save_params();	
	//	powerDeInit();
	}
	
	if(bleRead){
		my_KeyInit();
		if(my_KeyClk()){
				Key_timeout();
		}
		my_KeyDeInit();		
		Ble_Init();
	}else{
		lfclk_config();
		rtc_config();		
	}
  // Enter main loop.	
	//	
    for (;;)
    {
			
	if(bleRead){
		BLE_power_manage();
	}else{
		my_KeyInit();
		if(my_KeyClk()){
				Key_timeout();
		}
		my_KeyDeInit();	
    if(BLE_enable){
			NVIC_SystemReset();
		}		
		__WFI();
	}			
				//__SEV();
				//__WFE();
				//__WFE();
			
    }
}
