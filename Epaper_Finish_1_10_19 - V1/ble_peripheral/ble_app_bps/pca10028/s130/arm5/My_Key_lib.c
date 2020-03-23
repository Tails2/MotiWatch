#include "My_Key_lib.h"


My_Key_TypeDef My_Keydata;

void my_KeyInit(void){
		My_Keydata.old_key=0;
		My_Keydata.keyNow=0;
		My_Keydata.keyEvent=0;
		My_Keydata.old_key=0;
		uint8_t initarray[my_key_count] = key_Pin_array;
		for(uint8_t i=0;i<my_key_count;i++){
			nrf_gpio_cfg_input(initarray[i],GPIO_PIN_CNF_PULL_Pullup);	
		}
		nrf_gpio_cfg_output(29);
		nrf_gpio_port_set(NRF_GPIO_PORT_SELECT_PORT3,0x20);	
	for(uint8_t i=0;i<my_key_count;i++){
		My_Keydata.keycount[i]=0;
		My_Keydata.keyOncount[i]=0;		
	}
}

void my_KeyDeInit(void){
	uint8_t initarray[my_key_count] = key_Pin_array;
		for(uint8_t i=0;i<my_key_count;i++){
			nrf_gpio_cfg_input(initarray[i],GPIO_PIN_CNF_PULL_Disabled);	
		}
	nrf_gpio_cfg_input(29,GPIO_PIN_CNF_PULL_Disabled);	
	nrf_gpio_port_set(NRF_GPIO_PORT_SELECT_PORT3,0x20);

}

void my_KeyAktivate(void){ 
	uint8_t key=0;
	for(uint8_t i=0;i<my_key_count;i++){					
			if(My_Keydata.keyOncount[i]>=(my_key_time-1)){
				key = key|(1<< i);
				My_Keydata.keyOncount[i]=0;
			}		
	}
	
	if(key){
				My_Keydata.keyNow	=key;	
				My_Keydata.keyEvent = My_Keydata.keyNow;
				#ifdef Key_click_Interfase
						Key_click_Interfase; 
				#endif
				}			
}


void my_KeyDualSkan(void){  
//	uint8_t key=key_array_read;
	uint8_t keyedit=0;
	uint8_t i;
	uint8_t pinarray[my_key_count] = key_Pin_array;
	uint8_t portarray[my_key_count] = key_Port_array;
	uint8_t key=0;
	
	for(i=0;i<my_key_count;i++){
			key=key<<1;
			uint8_t pin=!((nrf_gpio_port_read(portarray[i])>>(pinarray[i] & 0x07))&0x01);
			key=key | pin;
		}
	
	uint8_t keymask = My_Keydata.old_key;
	My_Keydata.old_key=key;
		
	keyedit = key ^ keymask;
	for(i=0;i<my_key_count;i++){
		if(keyedit&0x01){
				if(key&0x01){
					My_Keydata.keyReadiOn|=1<<i;My_Keydata.keyReadiOff&=~(1<<i);
				}else{
					My_Keydata.keyReadiOff|=1<<i;My_Keydata.keyReadiOn&=~(1<<i);
				}						
		}
		if(key&0x01){
				if((My_Keydata.keyReadiOn>>i)&0x01){
					My_Keydata.keycount[i]++;
					if(My_Keydata.keycount[i]>=my_key_time-1){
						My_Keydata.keyReadiOn&=~(1<<i);
						My_Keydata.keyOncount[i]=My_Keydata.keycount[i];
						My_Keydata.keycount[i]=my_key_time;
						//my_KeyAktivate();				//on key down			
					}
				}
		}else{
				if((My_Keydata.keyReadiOff>>i)&0x01){
					My_Keydata.keycount[i]--;
					if(My_Keydata.keycount[i]<=1){
						My_Keydata.keyReadiOff&=~(1<<i);
						My_Keydata.keycount[i]=0;
						my_KeyAktivate();				//on key up		
					}
				}
		}	
				
		key=key>>1;
		keyedit=keyedit>>1;			
	}
}



uint8_t my_KeySkan(void){  
//	uint8_t key=key_array_read;
	uint8_t keyedit=0;
	uint8_t i;
	uint8_t pinarray[my_key_count] = key_Pin_array;
	uint8_t portarray[my_key_count] = key_Port_array;
	uint8_t key=0;
	for(i=0;i<my_key_count;i++){
			key=key<<1;
			uint8_t pin=!((nrf_gpio_port_read(portarray[i])>>(pinarray[i] & 0x07))&0x01);
			key=key | pin;
		}
	uint8_t keymask = My_Keydata.old_key;
	My_Keydata.old_key=key;
	if(key!=keymask){		
		keyedit = key ^ keymask;
			for(i=0;i<my_key_count;i++){
				if(keyedit&0x01){
						My_Keydata.keycount[i]=0;
						
				}
				keyedit=keyedit>>1;
			}
		keyedit=1;	
	}else{
			for(i=0;i<my_key_count;i++){					
					if(My_Keydata.keycount[i]<my_key_time){My_Keydata.keycount[i]++;}			
					if(My_Keydata.keycount[i]==my_key_time){
						My_Keydata.keycount[i]++;
						My_Keydata.keyNow = (My_Keydata.keyNow & (!(1<< i)))| (key& (1<< i));
						My_Keydata.keyEvent = My_Keydata.keyNow;
						#ifdef Key_click_Interfase
							Key_click_Interfase; 
						#endif
					}			
			}
	}
	return keyedit;
}
uint8_t get_my_Key(void){
	return My_Keydata.keyNow;  
}

uint8_t get_my_KeyEvent(uint8_t rst){
	uint8_t ret = My_Keydata.keyEvent;
	My_Keydata.keyEvent=rst;
	return ret; 	
}

uint8_t my_KeyClk(void){
	uint8_t i;
	uint8_t pinarray[my_key_count] = key_Pin_array;
	uint8_t portarray[my_key_count] = key_Port_array;
	uint8_t key=0;
	for(i=0;i<my_key_count;i++){
			key=key<<1;
			uint8_t pin=!((nrf_gpio_port_read(portarray[i])>>(pinarray[i] & 0x07))&0x01);
			key=key | pin;
		}
	return key;
}

uint8_t my_KeyRead(void){
	uint8_t key;
	uint8_t i = my_key_time<<1; 
		while (i){
			delayInterfase;
			i--;
			//my_KeySkan();
			my_KeyDualSkan();
		}
		key=My_Keydata.keyNow;
		My_Keydata.keyNow=0;
		return key;  
}
