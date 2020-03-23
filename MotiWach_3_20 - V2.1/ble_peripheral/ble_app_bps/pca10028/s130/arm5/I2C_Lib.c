
#include "I2C_Lib.h"  

#define Stm_Sistem_Clock 10000000

void While_us(unsigned int p)
{
	uint32_t i,j;
	
	j=p*(Stm_Sistem_Clock / 1000); 
	j=j/60000;
	
	for (i=0;i<j;i++){}

}



//------------------------------------- Програмный I2c --------------------------------//

void My_TWI_Init(){
	//Конфигурация управляющих пинов
	
//	 nrf_gpio_cfg_input(5,GPIO_PIN_CNF_PULL_Pullup);

}

void My_TWI_Start(){ 
TX_SDA;	
SET_SDA;
SET_SCL;
While_us(3);
RESET_SDA;
While_us(3);
RESET_SCL;
While_us(3);
}

void My_TWI_Stop(){

SET_SCL;
While_us(3);
TX_SDA;
SET_SDA;
While_us(3);
}

unsigned char My_TWI_TxByte(unsigned char Byte){
unsigned char i,ask;

ask=0;
for (i=0;i<8;i++){
  if((Byte&0x80)==0){RESET_SDA;}else{SET_SDA;} 
	While_us(1); 
  SET_SCL;
  Byte=Byte<<1;  
  While_us(3); 
  RESET_SCL;
  While_us(3);	
 } 

  SET_SCL; 
  SET_SDA; 
  RX_SDA;         
  While_us(3);
  if(READ_SDA){ask=1;}  
  RESET_SCL;  
  TX_SDA; 
  RESET_SDA; 
  RESET_SCL; 
  While_us(3); 
 return ask;
}

unsigned char My_TWI_RxByte(unsigned char ask){
unsigned char i,Byte;
Byte=0;

RESET_SDA;
RX_SDA;

for (i=0;i<8;i++){
  SET_SCL;
  While_us(3);
	Byte=Byte << 1;	
  if(READ_SDA){Byte=Byte | 0x01;} 
  RESET_SCL; 
  While_us(3);  

 }      
 if(ask){RESET_SDA;TX_SDA;} 
  SET_SCL;   
  While_us(3);
  RESET_SCL;  
  While_us(3);
  RESET_SDA;
  TX_SDA;  
 
 return Byte;
}

void TWI_Resiwe(unsigned char adr ,unsigned char reg, unsigned char *buf ,unsigned char Rx_count)
{
unsigned char i;
adr=adr<<1;
i=0;
My_TWI_Start();   //start
My_TWI_TxByte(adr & 0xfe);    // SEND ADR write
My_TWI_TxByte(reg);    // SEND reg
My_TWI_Start();   //start
My_TWI_TxByte(adr | 0x01);    // SEND ADR read
for(i=0;i<Rx_count-1;i++){
  *buf = My_TWI_RxByte(1); 
  *buf++;
}
  *buf = My_TWI_RxByte(0); 

My_TWI_Stop();
}

void TWI_Transmit(unsigned char adr ,unsigned char reg, unsigned char *buf ,unsigned char Tx_count)
{
unsigned char i;
adr=adr<<1;
i=0;
My_TWI_Start();   //start
My_TWI_TxByte(adr & 0xfe);    // SEND ADR write
My_TWI_TxByte(reg);    // SEND reg
for(i=0;i<Tx_count;i++){
  My_TWI_TxByte(*buf++);
}

My_TWI_Stop();
}

//--------------------------------------------------------------------------------------------------------



void Max9611_InitCurent_TWI(void)
{
unsigned char cr;	
//Ram_SingleWrite_Data(I2Cx,0xE0,0x0A,0x00);
//Ram_SingleWrite_Data(I2Cx,0xE6,0x0A,0x00);
cr=0x07;	
TWI_Transmit (0x70,0x0A,&cr,1);	
TWI_Transmit (0x73,0x0A,&cr,1);	
	
//Ram_SingleWrite_Data(I2Cx,0xE0,0x0A,0x07);
//Ram_SingleWrite_Data(I2Cx,0xE6,0x0A,0x07);		
}

unsigned char Max9611_ReadCurent_TWI(unsigned char Slave_Adres)
{
unsigned char datarx[2];	
unsigned int Current;
	
	TWI_Resiwe(Slave_Adres,0,&datarx[0],2);
	Current=(datarx[0] << 4)+(datarx[1]>>4);
  Current=(Current*10)/47; 
	return (unsigned char)Current;	
}

unsigned char Max9611_ReadVolt_TWI(unsigned char Slave_Adres)
{
unsigned char datarx[2];	
uint32_t Volt;
	
	TWI_Resiwe(Slave_Adres,2,&datarx[0],2);	
	Volt=(datarx[0] << 8)+datarx[1];
	Volt = (Volt*10) / 1106;	
	return (unsigned char)Volt;
}

unsigned char Max9611_ReadTerm_TWI(unsigned char Slave_Adres)
{
unsigned char Term;
	
	TWI_Resiwe(Slave_Adres,0x08,&Term,1);	
	if ((Term & 0x80)==0x80){Term=0xA7-(Term&0x80);}else{Term=Term+50;} 
	return (unsigned char)Term;
}

unsigned int Max9611_ReadInt_TWI(unsigned char Slave_Adres,unsigned char Adr_byte )
{
unsigned char datarx[2];	
unsigned int Current;
//	Current=Ram_SingleRead_Data(I2Cx,Slave_Adres,0x02)<<4;
//  Current=Current+(Ram_SingleRead_Data(I2Cx,Slave_Adres,0x03)>>4);
	TWI_Resiwe(Slave_Adres,Adr_byte,&datarx[0],2);	
	Current=(datarx[0] << 8)+datarx[1];
		
	return Current;
}


void Wake_Up_MPU6050(void){
	uint8_t data[3] = {0,0xc0,0};
	uint8_t Slave_Adres=0x68;
	uint8_t Adr_byte=0x6b;
	TWI_Transmit(Slave_Adres,Adr_byte,&data[0],2);
	Adr_byte=0x1a;
	data[1]=0x18;
	TWI_Transmit(Slave_Adres,Adr_byte,&data[0],3);
}

void Read_MPU6050(uint8_t *data){
	uint8_t Slave_Adres=0x68;
	uint8_t Adr_byte=0x3b;
	TWI_Resiwe(Slave_Adres,Adr_byte,&data[0],14);

}

void mpu_poverOn(void){
	nrf_gpio_cfg_output(30);	
	nrf_gpio_port_set(NRF_GPIO_PORT_SELECT_PORT3,0x40); 
}

void mpu_init(void){

	nrf_gpio_cfg_output(5);
	nrf_gpio_cfg_output(6);	
	nrf_gpio_port_set(NRF_GPIO_PORT_SELECT_PORT0,0x60);		
	Wake_Up_MPU6050();	 
}
