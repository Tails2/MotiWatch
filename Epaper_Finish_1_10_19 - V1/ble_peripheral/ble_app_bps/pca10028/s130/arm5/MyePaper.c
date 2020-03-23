//
//  @filename   :   epd1in54.cpp
//

#include "MyePaper.h"
#include "nrf_gpio.h"
//#include "nrf_delay.h"
#include "font.h"

		
#define Lcd_Send_Data nrf_gpio_port_set(DC_Port,1<<(DC_Pin & 0x07));	
#define Lcd_Send_Command nrf_gpio_port_clear(DC_Port,1<<(DC_Pin & 0x07));

#define Lcd_Reset_Off nrf_gpio_port_set(Reset_Port,1<<(Reset_Pin & 0x07));
#define Lcd_Reset_On nrf_gpio_port_clear(Reset_Port,1<<(Reset_Pin & 0x07));

#define Lcd_Off nrf_gpio_port_set(CS_Port,1<<(CS_Pin & 0x07));
#define Lcd_On nrf_gpio_port_clear(CS_Port,1<<(CS_Pin & 0x07));

#define Lcd_Busy_In (nrf_gpio_port_read(Busy_Port) & (1<<(Busy_Pin & 0x07))) == 1<<(Busy_Pin & 0x07)



const uint8_t lut_full_update[] =
{
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22, 
    0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88, 
    0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51, 
    0x35, 0x51, 0x51, 0x19, 0x01, 0x00
};

const uint8_t lut_New_update[] =
{
    0x50, 0xAA, 0x55, 0xAA, 0x11, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0xFF, 0x1F, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t lut_partial_update[] =
{
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void MyePaper_GPIO_DeInit(){
	nrf_gpio_cfg_input(CS_Pin,GPIO_PIN_CNF_PULL_Pullup);
	nrf_gpio_cfg_input(DC_Pin,GPIO_PIN_CNF_PULL_Pullup);
	nrf_gpio_cfg_input(Reset_Pin,GPIO_PIN_CNF_PULL_Pullup);
	nrf_gpio_cfg_input(Busy_Pin,GPIO_PIN_CNF_PULL_Pullup);
}

 
void MyePaper_GPIO_Init(void)
{
	//Configure GPIO pin 
	nrf_gpio_cfg_output(DC_Pin);
	nrf_gpio_cfg_output(CS_Pin);
	nrf_gpio_cfg_output(Reset_Pin);
	nrf_gpio_cfg_input(Busy_Pin,GPIO_PIN_CNF_PULL_Pulldown);	
}

void SpiSendSameBit(uint8_t data,uint16_t length){
Lcd_Send_Data;
Lcd_On;
SpiSendSameBit_Interfase;
Lcd_Off;	
}

void SpiSendSameByte(uint8_t data,uint16_t length){
Lcd_Send_Data;
Lcd_On;
SpiSendSameByte_Interfase;
Lcd_Off;	
}

void Reset(void) {
		Lcd_Reset_Off;
    Spi_While_ms(200);   
    Lcd_Reset_On             //module reset    
    Spi_While_ms(200);
    Lcd_Reset_Off;
    Spi_While_ms(200);    
}

void SpiSendData(uint8_t *data,uint8_t length){
Lcd_Send_Data;
Lcd_On;	
	SpiSend_Interfase;
Lcd_Off;		
}

void SpiSendByte(uint8_t byte){
	uint8_t *data = & byte;
	uint8_t length = 1;
	SpiSend_Interfase;
}

uint8_t WaitUntilIdle(void) {
  uint16_t i=0;  
	while((Lcd_Busy_In)&(i<1000)) {      //LOW: idle, HIGH: busy
    Spi_While_ms(1);
		i++;
		}   
if(i>999)return 0;	
		else return 1;
}

void SendByte(unsigned char data) {
	  Lcd_Send_Data;
    SpiSendByte(data);
}

void SendCommand(uint8_t command) {
    Lcd_Send_Command;
		Lcd_On;
    SpiSendByte(command);
		Lcd_Off;
}

void SendData(uint8_t data) {
	  Lcd_Send_Data;
		Lcd_On;
    SpiSendByte(data);
		Lcd_Off;
}

void EPD_1IN54_TurnOnDisplay(void)
{
	  if(WaitUntilIdle()){
			SendCommand(0x22); // DISPLAY_UPDATE_CONTROL_2
			SendData(0xC4);//C4
			SendCommand(0x20); // MASTER_ACTIVATION
			SendCommand(0xff);
		}
}

static void EPD_1IN54_SetCursor(uint16_t Xstart, uint16_t Ystart)
{
	  if(WaitUntilIdle()){
			SendCommand(0x4E); // SET_RAM_X_ADDRESS_COUNTER
			SendData((Xstart >> 3) & 0xFF);
			SendCommand(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
			SendData(Ystart & 0xFF);
			SendData((Ystart >> 8) & 0xFF);		
		}
}

void EPD_1IN54_SetWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
		if(WaitUntilIdle()){
			SendCommand(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
			SendData((Xstart >> 3) & 0xFF);
			SendData((Xend >> 3) & 0xFF);
			SendCommand(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
			SendData(Ystart & 0xFF);
			SendData((Ystart >> 8) & 0xFF);
			SendData(Yend & 0xFF);
			SendData((Yend >> 8) & 0xFF);	
		}
}

uint16_t EPD_1IN54_SetFrame(uint16_t x,uint16_t y,uint16_t width,uint16_t height){
    uint16_t x_end,y_end,count;

    if (x < 0 || width < 0 ||
        y < 0 || height < 0
    ) {
        return 0;
    }
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    
		x &= 0xF8;
    width &= 0xF8;
    if (x + width >= EPD_WIDTH) {
        x_end = 0;//x_end = EPD_WIDTH - 1;
    } else {
        x_end = EPD_WIDTH - x - width;//x_end = x + width - 1; //для переворота по х
    }
    if (y + height >= EPD_HEIGHT) {
        y_end = EPD_HEIGHT - 1;
    } else {
        y_end = y + height - 1;
    }
		x = EPD_WIDTH - x - 1;//для переворота по х
			
		EPD_1IN54_SetWindow(x, y,x_end, y_end);
    EPD_1IN54_SetCursor(x, y);
		
		count=(height)*((width) >>3);			
		return count;
}

void EPD_1IN54_Clear(void)
{
	//EPD_1IN54_PrintFillRectagle(0x00,0,0,200,200);    //FuulScreen Clear
	//EPD_1IN54_TurnOnDisplay();                        //FuulScreen Clear
	EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);
	EPD_1IN54_TurnOnDisplay();
}


void EPD_1IN54_PrintFillRectagle(uint8_t color,uint16_t x,uint16_t y,uint16_t fill_width,uint16_t fill_height) {
		uint16_t count;	
		count=EPD_1IN54_SetFrame(x,y,fill_width,fill_height);
		if(count>0){ 
			SendCommand(WRITE_RAM);
			SpiSendSameBit(color,count);
		}
}


void EPD_1IN54_PrintFrame(uint8_t* image_buffer,uint16_t x,uint16_t y,uint16_t image_width,uint16_t image_height) {
		uint16_t count;	
		count=EPD_1IN54_SetFrame(x,y,image_width,image_height);
		if(count>0){ 
			SendCommand(WRITE_RAM);
			SpiSendData(image_buffer,count);
		}
    
}

void EPD_1IN54_LineX(uint8_t color,uint8_t thicknes,uint16_t x,uint16_t y,uint16_t we ) {	
		uint16_t count,cnt,dvcnt;	
		uint8_t dw = (x+we)%8; 
		uint8_t xd = x%8;
		uint8_t clrStart=~(0xff>>(xd+1));
		uint8_t clrStop=~(0xff<<(dw));
		//if(dw){we=we+8;};
		if(color){
			clrStart=~clrStart;
			clrStop=~clrStop;
		}
		count=EPD_1IN54_SetFrame(x,y,we,thicknes);
		if(count>0){ 
			dvcnt=(count/thicknes)-2;
			SendCommand(WRITE_RAM);
			cnt=0;
			for(uint8_t i= 0;i<thicknes;i++){
				if(cnt>(count-3))break;
				SpiSendSameByte(clrStart,1);
				SpiSendSameBit(color,dvcnt);
				SpiSendSameByte(clrStop,1);
				cnt+=dvcnt+2;
			}
		}
}

void EPD_1IN54_LineY(uint8_t color,uint8_t thicknes,uint16_t x,uint16_t y,uint16_t he) {	
		uint16_t count;	
		uint8_t clr;
		uint8_t mask=(0xff<<(8-thicknes))&0xff;
		if((x%8)<thicknes){
			clr = mask>>((x%8)+1);
		}else{
			clr = mask>>((x%8)-thicknes+1);
		}
		if(!color){clr=~clr;}
		count=EPD_1IN54_SetFrame(x,y,8,he);
		if(count>0){ 
			SendCommand(WRITE_RAM);
			SpiSendSameByte(clr,count);
		}
}


uint16_t EPD_1IN54_PrintPicture(uint8_t x,uint8_t y,uint16_t dx,uint16_t dy,uint8_t color,const uint8_t *picture){
	uint8_t Data;
	uint16_t count=EPD_1IN54_SetFrame(x,y,dx,dy);
	if(count>0){ 
		SendCommand(WRITE_RAM);
		for(uint8_t i = 0;i<(dx>>3);i++)
			for(uint8_t j = 0;j<dy;j++){
				Data = *picture++;
				if(color){Data = ~Data;}
				SendData(Data);				
			}
	}
	return count;
}

void EPD_1IN54_PrintRectagle(uint8_t color,uint8_t thicknes,uint16_t x,uint16_t y,uint16_t we,uint16_t he) {
	
		EPD_1IN54_LineY(color,thicknes,x,y,he);
		EPD_1IN54_LineY(color,thicknes,x+we,y,he);
		EPD_1IN54_LineX(color,thicknes,x,y,we);		
		EPD_1IN54_LineX(color,thicknes,x,y+he,we);	
}



const uint8_t *FontCange(uint8_t fontNumber) {
	const uint8_t *font;
	if(fontNumber==1){font = &ArialNarrow18_dinamic_table_Adr[0];} 
	//if(fontNumber==2){font = &Arial_Narrow_24_dinamic_table_Adr[0];}//&Century19_dinamic_table_Adr[0];} 
	if(fontNumber==2){font = &MS_Mincho16_dinamic_table_Adr[0];}
	if(fontNumber==3){font = &TimesNewRoman28_dinamic_table_Adr[0];}   
		
	return font;
}

FontSettings_MyTypeDef FontSettings(uint8_t fontNumber) {
	const uint8_t *font=FontCange(fontNumber);
	FontSettings_MyTypeDef fs;
	fs.heigth = font[0];
	fs.spacing = font[1];
	fs.offsetDinamic = (font[4]<<8)+font[5];
	fs.offsetTable = (font[6]<<8)+font[7];		
	return fs;
}


uint8_t EPD_1IN54_outIntString(uint32_t OutInt,const uint8_t *font,uint8_t color,uint16_t *cn,uint16_t count,uint8_t xin,uint8_t btMax){
		uint8_t n=0;
		uint32_t i,si,k,z;
		uint8_t c,Data;
		uint16_t width,offset;
		uint64_t tempOut;
		uint16_t offsetDinamic = (font[4]<<8)+font[5];
		uint16_t offsetTable = (font[6]<<8)+font[7];
		uint8_t spacing = font[1];

		uint8_t buf[11];
		buf[10]='#';
		if(OutInt){
			si = 9;
			while(OutInt>0){
				buf[si]= (OutInt%10)+48;
				OutInt = OutInt/10;
				si--;
			}
			si++;
		}else{
			buf[9]='0';
			si = 9;
		}
		
    while((buf[si]!='#')&(buf[si]!=0)){	
			  n++;
	      c=(buf[si]-32)<<1;
        offset = ((font[offsetDinamic+c])<<8)+((font[offsetDinamic+c+1])); //  Arial_16_dinamic_table_Adr[c];
			  k=0;
        width = ((font[offsetDinamic+c+2])<<8)+((font[offsetDinamic+c+3]))-offset; //Arial_16_dinamic_table_Adr[c+1]-offset; 
        if (width > 100){width=100;}
				z=0;				
				for (i = 0; i < width; i+=font[0]) { 
					
						uint8_t h=0;					
					  tempOut=0;
						while(h<(font[0])){
							tempOut=(tempOut<<8)|font[offset+i+h];
							h++;
						}
						
						
						
						tempOut <<=((btMax-font[0])<<3);
						tempOut >>=xin;
						
						h=((btMax)*8);
						while(h>0){
							h-=8;
							Data=tempOut>>h;
							if(color){Data = ~Data;}
							if(cn[0]>count)break;
							SendData(Data);
							cn[0]++;
						}										
				} 	
				for (i = 0; i < spacing; i++){
					for(uint8_t h=0; h<btMax;h++){
							if(cn[0]>count)break;
							SendData(color);
							cn[0]++;
					}				
				}
				si++;;	
		}	
return n;
}

uint8_t EPD_1IN54_PrintRamString(String_MyTypeDef setting,uint8_t color){
	
		uint8_t n=0;
		uint32_t i,k,z;
		uint8_t c,Data;
		uint16_t width,offset;
		uint64_t tempOut;
		const uint8_t *font=FontCange(setting.Font);
		FontSettings_MyTypeDef fs = FontSettings(setting.Font);

//	uint32_t  j,l,m,dp;
	uint16_t count,cnt;

	uint16_t dx;//=(font[0]<<3)+xin+xout;
	uint16_t dy= EPD_WIDTH - setting.y-1;
	uint8_t btMax;
	
	if((setting.xin+setting.xout)>0){btMax=font[0]+((setting.xin+setting.xout-1)>>3)+1;} else {btMax=font[0];}
	dx=btMax<<3;
	
	if(setting.yout){count=EPD_1IN54_SetFrame(setting.x,setting.y,dx,dy);}
	else{count=EPD_1IN54_SetFrame(setting.x,setting.y,dx,setting.dy);}
	if(count>0){ 	

		SendCommand(WRITE_RAM);
		cnt=0;
		
		for (i = 0; i < setting.yin; i++){
					for(uint8_t h=0; h<btMax;h++){
							if(cnt>count)break;
							SendData(color);
							cnt++;
			}				
		}
		uint8_t j = 0;
		while((setting.String[j]!=0)){	
			  n++;
	      c=(setting.String[j]-32)<<1;
        offset = ((font[fs.offsetDinamic+c])<<8)+((font[fs.offsetDinamic+c+1])); //  Arial_16_dinamic_table_Adr[c];
			  k=0;
        width = ((font[fs.offsetDinamic+c+2])<<8)+((font[fs.offsetDinamic+c+3]))-offset; //Arial_16_dinamic_table_Adr[c+1]-offset; 
        if (width > 100){width=100;}
				z=0;				
				for (i = 0; i < width; i+=fs.heigth) { 
					
						uint8_t h=0;					
					  tempOut=0;
						while(h<(fs.heigth)){
							tempOut=(tempOut<<8)|font[offset+i+h];
							h++;
						}
						
						tempOut <<=((btMax-fs.heigth)<<3);
						tempOut >>=setting.xin;
						
						h=((btMax)*8);
						while(h>0){
							h-=8;
							Data=tempOut>>h;
							if(color){Data = ~Data;}
							if(cnt>count)break;
							SendData(Data);
							cnt++;
						}										
				} 	
				for (i = 0; i < fs.spacing; i++){
					for(uint8_t h=0; h<btMax;h++){
							if(cnt>count)break;
							SendData(color);
							cnt++;
					}				
				}
				j++;	
		}	
	
		if(setting.yout){	
				for (i = 0; i < setting.yout; i++){
					for(uint8_t h=0; h<btMax;h++){
							if(cnt>count)break;
							SendData(color);
							cnt++;
				}				
			}
			}else{
				while(cnt<count){
					SendData(color);
					cnt++;
				}	
		}	
	}
return n;
}



uint8_t EPD_1IN54_outString(const uint8_t *str,const uint8_t *font,uint8_t color,uint16_t *cn,uint16_t count,uint8_t xin,uint8_t btMax){
		uint8_t n=0;
		uint32_t i,k,z;
		uint8_t c,Data;
		uint16_t width,offset;
		uint64_t tempOut;
		uint16_t offsetDinamic = (font[4]<<8)+font[5];
		uint16_t offsetTable = (font[6]<<8)+font[7];
		uint8_t spacing = font[1];

	
    while((*str!='#')&(*str!=0)){	
			  n++;
	      c=(*str-32)<<1;
        offset = ((font[offsetDinamic+c])<<8)+((font[offsetDinamic+c+1])); //  Arial_16_dinamic_table_Adr[c];
			  k=0;
        width = ((font[offsetDinamic+c+2])<<8)+((font[offsetDinamic+c+3]))-offset; //Arial_16_dinamic_table_Adr[c+1]-offset; 
        if (width > 100){width=100;}
				z=0;				
				for (i = 0; i < width; i+=font[0]) { 
					
						uint8_t h=0;					
					  tempOut=0;
						while(h<(font[0])){
							tempOut=(tempOut<<8)|font[offset+i+h];
							h++;
						}
						
						tempOut <<=((btMax-font[0])<<3);
						tempOut >>=xin;
						
						h=((btMax)*8);
						while(h>0){
							h-=8;
							Data=tempOut>>h;
							if(color){Data = ~Data;}
							if(cn[0]>count)break;
							SendData(Data);
							cn[0]++;
						}										
				} 	
				for (i = 0; i < spacing; i++){
					for(uint8_t h=0; h<btMax;h++){
							if(cn[0]>count)break;
							SendData(color);
							cn[0]++;
					}				
				}
				str++;	
		}	
return n;
}



uint16_t EPD_1IN54_StringRamPixelLength(uint8_t *str,const uint8_t Font){
		uint8_t c;
		uint16_t width,offset;
		const uint8_t *font=FontCange(Font);
		FontSettings_MyTypeDef fs = FontSettings(Font);
	
		uint16_t length =0;
	
    while(*str!=0){	
	      c=(*str-32)<<1;
        offset = ((font[fs.offsetDinamic+c])<<8)+((font[fs.offsetDinamic+c+1])); //  Arial_16_dinamic_table_Adr[c];
        width = ((font[fs.offsetDinamic+c+2])<<8)+((font[fs.offsetDinamic+c+3]))-offset; //Arial_16_dinamic_table_Adr[c+1]-offset; 
        if (width > 100){width=100;}
				width/=fs.heigth;
				length+=width+fs.spacing;
				str++;	
		}	
return length;
}

uint8_t EPD_1IN54_myDrawStringPixelLength(const uint8_t *str,const uint8_t Font){
		uint8_t c;
		uint16_t width,offset;
		const uint8_t *font=FontCange(Font);
		FontSettings_MyTypeDef fs = FontSettings(Font);
	
		uint8_t length =0;
	
    while((*str!='#')&(*str!=0)){	
	      c=(*str-32)<<1;
        offset = ((font[fs.offsetDinamic+c])<<8)+((font[fs.offsetDinamic+c+1])); //  Arial_16_dinamic_table_Adr[c];
        width = ((font[fs.offsetDinamic+c+2])<<8)+((font[fs.offsetDinamic+c+3]))-offset; //Arial_16_dinamic_table_Adr[c+1]-offset; 
        if (width > 100){width=100;}
				width/=fs.heigth;
				length+=width+fs.spacing;
				str++;	
		}	
return length;
}

uint8_t EPD_1IN54_myDrawString(My_StringObg_TypeDef setting) {
	//const uint8_t *str=setting.String;
	const uint8_t *font=FontCange(setting.Font);
		
	uint32_t i, j,l,m,dp;
	uint16_t count,cnt;
  uint16_t n=0;

	uint16_t dx;//=(font[0]<<3)+xin+xout;
	uint16_t dy= EPD_WIDTH - setting.y;
	uint8_t btMax;
	
	if((setting.xin+setting.xout)>0){btMax=font[0]+((setting.xin+setting.xout-1)>>3)+1;} else {btMax=font[0];}
	dx=btMax<<3;
	
	if(setting.yout){count=EPD_1IN54_SetFrame(setting.x,setting.y,dx,dy);}
	else{count=EPD_1IN54_SetFrame(setting.x,setting.y,dx,setting.dy);}
	if(count>0){ 	

 
		SendCommand(WRITE_RAM);
		cnt=0;
		
		for (i = 0; i < setting.yin; i++){
					for(uint8_t h=0; h<btMax;h++){
							if(cnt>count)break;
							SendData(setting.color);
							cnt++;
			}				
		}
		if(setting.check&0x01){
			n+=EPD_1IN54_outString(setting.checkString,font,setting.color,&cnt,count,setting.xin,btMax);
		}
		
		if(setting.check&0x04){
			n+=EPD_1IN54_outIntString(setting.intString,font,setting.color,&cnt,count,setting.xin,btMax);
		}else{
			n+=EPD_1IN54_outString(setting.String,font,setting.color,&cnt,count,setting.xin,btMax);
		}
		
		
		if(setting.check&0x02){
			n+=EPD_1IN54_outString(setting.checkString,font,setting.color,&cnt,count,setting.xin,btMax);
		}
		
		
		if(setting.yout){	
				for (i = 0; i < setting.yout; i++){
					for(uint8_t h=0; h<btMax;h++){
							if(cnt>count)break;
							SendData(setting.color);
							cnt++;
				}				
			}
			}else{
				while(cnt<count){
					SendData(setting.color);
					cnt++;
				}	
		}	
	}

return n;
}


void EPD_1IN54_DrawString(uint8_t x, uint8_t y,uint8_t *str,uint8_t color,const uint16_t *font) {
        uint32_t i, j,l,k,n,m,dp;
	      uint16_t offset,width,count,xcount;
        uint16_t Data;
	      uint8_t c;

	
	uint16_t dx=(font[0]>>8)<<3;
	uint16_t dy= EPD_WIDTH - x-1;
	uint8_t spacing = font[0]&0xff;
	uint16_t offsetDinamic = font[2];
	uint16_t offsetTable = font[3]; 
	
	
	count=EPD_1IN54_SetFrame(x,y,dx,dy);
	if(count>0){ 	

 
		SendCommand(WRITE_RAM);
		xcount=0;
    while((*str!='#')&(*str!=0)){	
			  n++;
	      c=*str-32;
        offset = font[offsetDinamic+c]; //  Arial_16_dinamic_table_Adr[c];
			  k=0;
        width = font[offsetDinamic+c+1]-offset; //Arial_16_dinamic_table_Adr[c+1]-offset; 
        if (width > 50){width=50;} 
				for (i = 0; i < width; i++) {  
						Data = font[offsetTable+offset+i];////Arial_16_data_table[offset+i];
						if(color){Data = ~Data;}
						SendData((Data>>8)&0xff);
						SendData(Data&0xff);			
				} 	
				for (i = 0; i < spacing; i++){
							SendData(color);
							SendData(color);				
				}
				str++;
		}
	}
}



void MyePaperInit(uint8_t mode) {
		MyePaper_GPIO_Init();
    Reset();
    SendCommand(DRIVER_OUTPUT_CONTROL);
    SendData((EPD_HEIGHT - 1) & 0xFF);
    SendData(((EPD_HEIGHT - 1) >> 8) & 0xFF);
    SendData(0x00);                     // GD = 0; SM = 0; TB = 0;
    SendCommand(BOOSTER_SOFT_START_CONTROL);
    SendData(0xD7);
    SendData(0xD6);
    SendData(0x9D);
    SendCommand(WRITE_VCOM_REGISTER);
    SendData(0x7c); //a8   0x9b                 // VCOM 7C
    SendCommand(SET_DUMMY_LINE_PERIOD);
    SendData(0x1A);                     // 4 dummy lines per gate
    SendCommand(SET_GATE_TIME);
    SendData(0x08);                     // 2us per line
    SendCommand(DATA_ENTRY_MODE_SETTING);
    SendData(0x02);                     // X decrement; Y increment
	
		EPD_1IN54_SetWindow(0, 0,200, 200); 
    EPD_1IN54_SetCursor(0, 0);

		SendCommand(0x32); // TERMINATE_FRAME_READ_WRITE
		if(mode == 0){
			for(uint8_t i=0;i<30;i++){
				SendData(lut_full_update[i]);
			}
		} 
		
		if(mode == 1){
			for(uint8_t i=0;i<30;i++){
				SendData(lut_partial_update[i]);
			}
		}	
		if(mode == 2){
			for(uint8_t i=0;i<30;i++){
				SendData(lut_New_update[i]);
			}
		SendCommand(0x22);
		SendData(0xC0); 	
		SendCommand(0x20);	
		}
}

void MyePaperDualInit(){
	/*
	MyePaperInit(2);
	WaitUntilIdle();
	SendCommand(DATA_ENTRY_MODE_SETTING);
  SendData(0x02);                     // X decrement; Y increment
	EPD_1IN54_PrintFillRectagle(0xff,0,0,199,199);
	WaitUntilIdle();
  SendCommand(0x22); // DISPLAY_UPDATE_CONTROL_2
  SendData(0xC4);
  SendCommand(0x20); // MASTER_ACTIVATION
	*/
	MyePaperInit(1);
	if(WaitUntilIdle()){
		SendCommand(DATA_ENTRY_MODE_SETTING);
		SendData(0x02);                     // X decrement; Y increment
		EPD_1IN54_PrintFillRectagle(0x00,0,0,200,200);	
		EPD_1IN54_TurnOnDisplay();
		EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);	
		EPD_1IN54_TurnOnDisplay();
	}
}



void Sleep() {
    SendCommand(DEEP_SLEEP_MODE);
    if(WaitUntilIdle()){
			Lcd_Reset_On;
		}
}

/* END OF FILE */


