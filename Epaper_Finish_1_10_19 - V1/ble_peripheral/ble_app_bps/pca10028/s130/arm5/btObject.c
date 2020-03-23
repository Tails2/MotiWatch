#include "MyePaper.h"
#include "btObject.h"


// global params
My_ObjKey_TypeDef ObjKey1;
My_ObjKey_TypeDef ObjKey2;
My_ObjKey_TypeDef ObjKey3;

uint8_t currentScrinPosition;
uint16_t stackScrin[30];
uint16_t paramsArray[30];
//----------------



void KeyReaction(uint8_t key){
	EPD_1IN54_Clear();

	//W25Q_ReadData(&frame[0],0,10);
	
//	if(test1){
	//	EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);
		
//		EPD_1IN54_PrintRectagle(0xff,2,40,40,100,100); 
//	EPD_1IN54_PrintRectagle(0,2,1,97,97,100);
//	EPD_1IN54_PrintRectagle(0,2,97,1,100,97);
//	EPD_1IN54_PrintRectagle(0xff,2,97,97,104,104);
		
		
		EPD_1IN54_PrintRectagle(0,2,27,22,60,100);
		//EPD_1IN54_PrintRectagle(0,2,0,0,60,100);
		//EPD_1IN54_PrintRectagle(0,2,100,100,99,99);
		
		//EPD_1IN54_LineX(0,2,27,22,60);
		//EPD_1IN54_LineY(0,2,27,22+2,100-2);
		//EPD_1IN54_LineX(0,2,27,122,60);
		//EPD_1IN54_LineY(0,2,87,22+2,100-2);
		
		
//		test1=0;
//	}else{
		EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);
	//  EPD_1IN54_DrawString(0,0,&frame[0],0xff,&Arial_16_dinamic_table_Adr[0]);	
		
		//EPD_1IN54_DrawObjectString(&ObgHellou[0]);//Century19_dinamic_table_Adr[0]);
			
		//EPD_1IN54_DrawObjectMenu(&ObgMemu[0]); 
		
		//EPD_1IN54_DrawObjectPicture(&ObgPict1[0]); 
		
		//EPD_1IN54_DrawObjectString(&Obgkeytext1[0]);
		//EPD_1IN54_DrawObjectKeyRigth(&ObgkeyRect1[0]);

			EPD_1IN54_DrawObjScrin(&ObjScrinLanguage[0]);
		
		//	EPD_1IN54_PrintFillRectagle(0,0,0,97,97);
//	EPD_1IN54_PrintFillRectagle(0xff,0,97,97,104);
//	EPD_1IN54_PrintFillRectagle(0xff,97,0,104,97);
//	EPD_1IN54_PrintFillRectagle(0,97,97,104,104);	
	//	test1=1;
//	}
	EPD_1IN54_TurnOnDisplay();
	//nrf_delay_ms(5000);
}

uint8_t EPD_1IN54_DrawObjectKeyRigth(const uint8_t *object) {
	uint16_t x=object[0];
	uint16_t y=object[1];
	uint16_t we=object[2];
	uint16_t he=object[3];
	uint8_t color=object[4];
	uint8_t thick=object[5];
	
	EPD_1IN54_LineY(color,thick,x,y,he);
	EPD_1IN54_LineX(color,thick,x,y,we);	
return 6;	
}

uint8_t EPD_1IN54_DrawObjectKeyLeft(const uint8_t *object) {
	uint16_t x=object[0];
	uint16_t y=object[1];
	uint16_t we=object[2];
	uint16_t he=object[3];
	uint8_t color=object[4];
	uint8_t thick=object[5];
	
	EPD_1IN54_LineY(color,thick,x,y,he);		
	EPD_1IN54_LineX(color,thick,x,y+he,we);
	return 6;
}

void EPD_1IN54_DrawObjectRect(const uint8_t *object) {
	uint16_t x1=object[0];
	uint16_t y1=object[1];
	uint16_t x2=object[2];
	uint16_t y2=object[3];
	uint8_t color=object[4];
	uint8_t thick=object[5];
	EPD_1IN54_PrintRectagle(color,thick,x1,y1,x2,y2);
}

uint16_t EPD_1IN54_DrawObjectPicture(const uint8_t *object) {
	uint8_t x=object[0];
	uint8_t y=object[1];
	uint16_t dx=object[2];
	uint16_t dy=object[3];
	uint8_t color=object[4];
	uint8_t Data;
	const uint8_t *picture=object+5;
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

uint16_t EPD_1IN54_DrawObjectMenu(const uint8_t *object) {
	My_StringObg_TypeDef setting;
	setting.x=object[0];
	setting.y=object[1];
	setting.xin=0;
	setting.xout=0;
	setting.yin=object[2];
	setting.yout=object[3];
	uint8_t pos=object[4];
	uint8_t vievpos=object[5];
	uint8_t countpos =object[6];
	setting.color=object[7];
	setting.Font=object[8];
	const uint8_t *strarrr = &object[9];
	const uint8_t *font=FontCange(setting.Font);
	uint8_t strwe=font[0];
	uint8_t startpos=0;
	uint16_t n=8;
	if(pos>(vievpos-1)){
		if(pos>(countpos-vievpos)){
			startpos=countpos-vievpos;
		}else{
			startpos=pos;
		}
	}
	uint8_t poscnt=0;
	uint8_t chcnt=0;
	uint8_t j=0;
	
	for(uint8_t i = 0;i<countpos;i++){
		if(i>=startpos){
			if(i==pos){setting.color=0x00;}else{setting.color=0xff;}
			setting.String=&strarrr[chcnt];
			n+=EPD_1IN54_myDrawString(setting);
			setting.x+=(strwe<<3);
			poscnt++;
			if(poscnt>=vievpos)break;
		}
		j=0;
		while((strarrr[chcnt+j] != '#')&&(j<50)){
			j++;
		}
		chcnt+=j+1;
	}	
	return n;
}

uint32_t findhash(const uint8_t *buf){
	uint32_t i=0;
	uint32_t hash=0;
	while(i<10000){
		hash=(buf[i]<<16)|(buf[i+1]<<8)|(buf[i+2]);
		if(hash==0x00002323)return i;
		if(hash==0x00232323)return 10001;
		i++;		
	}
	return 10001;
}


void EPD_1IN54_DrawObjScrin(const uint8_t *object){
	uint32_t objstart;
	uint32_t find = 0;
	
	ObjKey1.connand=object[0];
	ObjKey1.objektadres=(object[1]<<8)+object[2];
	ObjKey1.objektadres=(object[3]<<8)+object[4];
	ObjKey1.connand=object[5];
	
	ObjKey2.connand=object[6];
	ObjKey2.objektadres=(object[7]<<8)+object[8];
	ObjKey2.objektadres=(object[9]<<8)+object[10];
	ObjKey2.connand=object[11];
	
	ObjKey3.connand=object[12];
	ObjKey3.objektadres=(object[13]<<8)+object[14];
	ObjKey3.objektadres=(object[15]<<8)+object[16];
	ObjKey3.connand=object[17];
	
	objstart = 18;
	
	while (find<10000){
		find=findhash(&object[objstart]);
		objstart+=find+3;
		
		if(object[objstart]==1){objstart+=EPD_1IN54_DrawObjectString(&object[objstart+1]);}
		if(object[objstart]==2){objstart+=EPD_1IN54_DrawObjectMenu(&object[objstart+1]);}
		if(object[objstart]==3){objstart+=EPD_1IN54_DrawObjectPicture(&object[objstart+1]);}
		if(object[objstart]==4){objstart+=EPD_1IN54_DrawObjectKeyRigth(&object[objstart+1]);}	
		if(object[objstart]==5){objstart+=EPD_1IN54_DrawObjectKeyLeft(&object[objstart+1]);}	

	}	
}

uint8_t EPD_1IN54_DrawObjectString(const uint8_t *object){
	
	My_StringObg_TypeDef setting;
	setting.x=object[0];
	setting.y=object[1];
	setting.xin=object[2];
	setting.xout=object[3];
	setting.yin=object[4];
	setting.yout=object[5];
	setting.color=object[6];
	setting.Font=object[7];
	setting.String=object+8;
	
	
	return 8+EPD_1IN54_myDrawString(setting);
}
