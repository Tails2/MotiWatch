#include "MyePaper.h"
#include "btObject.h"
#include "MyePaperObjektFunction.h"
#include "nrf_drv_rtc.h"
#include "W25Q_Flash.h"


// global params
My_ObjKey_TypeDef ObjKey1;
My_ObjKey_TypeDef ObjKey2;
My_ObjKey_TypeDef ObjKey3;

const uint8_t* oldPageTextList=0;
uint16_t RandomCounter=0;

uint8_t currentScrinPosition;
uint16_t stackScrin[128];
uint16_t paramsArray[128];
uint32_t CounterArray[16];
uint8_t BLE_enable=0;
uint32_t CounterBackArray[80];

//----------------
uint8_t currentScrinPositionUnChenged;
uint16_t ScrinUnChenged;
uint16_t paramsArrayUnChenged[128];
uint32_t CounterArrayUnChenged[16];
//----------------


void ClearStack(void){
	currentScrinPosition= 0;	
	stackScrin[0]=1;
	paramsArray[0]=0;
	paramsArray[1]=0;
	paramsArray[2]=0;
	currentScrinPositionUnChenged=0xff;	
	ScrinUnChenged=0xffff;	
	paramsArrayUnChenged[0]=0xffff;	
	for(uint8_t i=0;i<80;i++){
		CounterBackArray[i]=0xffffffff;
		CounterArray[i]=0xffffffff;
	}
}

void LoadRamParams(void){
	uint32_t adr=0;
	uint8_t buf[68];
	W25Q_ReadData(&buf[0],adr,68);
	adr+=256;
	if(buf[0]<128){
		uint8_t j=0;
		for(uint8_t i = 4;i<68;i+=4){	
			CounterArray[j] = (buf[i]<<24)|(buf[i+1]<<16)|(buf[i+2]<<8)|(buf[i+3]);	
			CounterArrayUnChenged[j]=CounterArray[j];
			j++;
		}		
		currentScrinPosition=buf[0];
		uint8_t *pRamAdr= (uint8_t*)(&stackScrin);
		W25Q_ReadData(pRamAdr,adr,256);
		adr+=256;
		pRamAdr= (uint8_t*)(&paramsArray);
		W25Q_ReadData(pRamAdr,adr,256);
		adr+=256;	
		W25Q_ReadData((uint8_t *)(&CounterBackArray[0]),adr,256);			
		adr+=256;
		W25Q_ReadData((uint8_t *)(&CounterBackArray[64]),adr,64);		
		if(stackScrin[currentScrinPosition]!=0xFFFF){
			currentScrinPositionUnChenged=currentScrinPosition;
			ScrinUnChenged=stackScrin[currentScrinPosition];
			for(uint8_t i = 0;i<128;i++){
				paramsArrayUnChenged[i]=paramsArray[i];	
			}
		}else{ClearStack();}
	}else{ClearStack();}	
}
void Save_params(){
uint8_t chengedParams=0;
	
	if(currentScrinPositionUnChenged!=currentScrinPosition){chengedParams=1;}
	if(ScrinUnChenged!=stackScrin[currentScrinPosition]){chengedParams=1;}
	for(uint8_t i = 0;i<128;i++){
		if(paramsArrayUnChenged[i]!=paramsArray[i]){chengedParams=1;}	
	}
	for(uint8_t i = 0;i<16;i++){
		if(CounterArray[i]!=CounterArrayUnChenged[i]){chengedParams=1;}	
	}
if(BLE_enable){chengedParams=1;}	
	if(chengedParams){
		uint8_t buf[68];
		uint32_t adr=0;
		buf[0]=currentScrinPosition;
		buf[1]=BLE_enable;
		uint8_t j=0;
		for(uint8_t i = 4;i<68;i+=4){	
			buf[i]=CounterArray[j]>>24;
			buf[i+1]=CounterArray[j]>>16;
			buf[i+2]=CounterArray[j]>>8;
			buf[i+3]=CounterArray[j];
			j++;
		}	
		W25Q_Flash_Sektor_erase(0);		
		W25Q_WritePage(&buf[0],adr,68);
		adr+=256;
		W25Q_WritePage((uint8_t *)(&stackScrin[0]),adr,256);	
		adr+=256;
		W25Q_WritePage((uint8_t *)(&paramsArray[0]),adr,256);	
		adr+=256;
		W25Q_WritePage((uint8_t *)(&CounterBackArray[0]),adr,256);			
		adr+=256;
		W25Q_WritePage((uint8_t *)(&CounterBackArray[64]),adr,64);	
	}
}

void saveCounter(uint32_t counter){
for(uint8_t i=1;i<5;i++){
CounterBackArray[i+counter*5-1]=CounterBackArray[i+counter*5];
}	
CounterBackArray[4+counter*5]=CounterArray[counter];
CounterArray[counter]=0xffffffff-(NRF_RTC0->COUNTER>>3);
}

const uint8_t *getScrin(uint8_t numberScrin){
	const uint8_t *scrin;
	
	if(numberScrin==0){scrin=&ObjScrinStart[0];}
	if(numberScrin==1){scrin=&ObjScrinLanguage[0];}
	if(numberScrin==2){scrin=&ObjScrinLogo[0];}
	if(numberScrin==3){scrin=&ObjScrinGoals[0];}
	
	if(numberScrin==4){scrin=&ObjScrinGoalsSmokingAktivate[0];}
	if(numberScrin==5){scrin=&ObjScrinGoalsAlcoholAktivate[0];}
	if(numberScrin==6){scrin=&ObjScrinGoalsDugsAktivate[0];}
	if(numberScrin==7){scrin=&ObjScrinGoalJunkFoodAktivate[0];}
	if(numberScrin==8){scrin=&ObjScrinGoalsPornAktivate[0];}
	if(numberScrin==9){scrin=&ObjScrinGoalsSugarAktivate[0];}
	if(numberScrin==10){scrin=&ObjScrinGoalsSocialMediaAktivate[0];}
	if(numberScrin==11){scrin=&ObjScrinGoalsImpulsiveBuyingAktivate[0];}
	if(numberScrin==12){scrin=&ObjScrinGoalsRedMeatAktivate[0];}
	if(numberScrin==13){scrin=&ObjScrinGoalsSingleUsePlasticAktivate[0];}
	
	if(numberScrin==14){scrin=&ObjScrinGoalsSmoking[0];}
	if(numberScrin==15){scrin=&ObjScrinGoalsAlcohol[0];}
	if(numberScrin==16){scrin=&ObjScrinGoalsDugs[0];}
	if(numberScrin==17){scrin=&ObjScrinGoalJunkFood[0];}
	if(numberScrin==18){scrin=&ObjScrinGoalsPorn[0];}
	if(numberScrin==19){scrin=&ObjScrinGoalsSugar[0];}
	if(numberScrin==20){scrin=&ObjScrinGoalsSocialMedia[0];}
	if(numberScrin==21){scrin=&ObjScrinGoalsImpulsiveBuying[0];}
	if(numberScrin==22){scrin=&ObjScrinGoalsRedMeat[0];}
	if(numberScrin==23){scrin=&ObjScrinGoalsSingleUsePlastic[0];}
	
	if(numberScrin==24){scrin=&ObjScrinGoalsMenu[0];}
	if(numberScrin==25){scrin=&ObjRunGouToSelectedGoal[0];}
	if(numberScrin==26){scrin=&ObjScrinResetGoals[0];}
	if(numberScrin==27){scrin=&ObjScrinRecordsGoals[0];}
	if(numberScrin==28){scrin=&ObjScrinDeleteGoals[0];}
	if(numberScrin==29){scrin=&ObjRunDeleteGoals[0];};
	if(numberScrin==30){scrin=&ObjScrinGoalsPanic[0];};
	if(numberScrin==31){scrin=&ObjRunStartMotiv[0];};
	if(numberScrin==32){scrin=&ObjScrinGoalsMotiv[0];};
	if(numberScrin==33){scrin=&ObjRunNextMotiv[0];};
	
	if(numberScrin==41){scrin=&ObjScrinGoalsFinishMotiv[0];};
	if(numberScrin==42){scrin=&ObjRunGoutoGoalsStackDefoult[0];};
	if(numberScrin==43){scrin=&ObjRunResetGoals[0];};
	if(numberScrin==44){scrin=&ObjRunFinishResetGoals[0];};
	
	
	
	if(numberScrin==200){scrin=&ObjRunGoutooGoals[0];};
	
	if(numberScrin==249){scrin=&ObjScrinSettings[0];};
	if(numberScrin==250){scrin=&ObjScrinBluetooth[0];};
	if(numberScrin==251){scrin=&ObjScrinReset[0];};
	if(numberScrin==252){scrin=&ObjRunReset[0];};
	
	
return scrin;
}

const uint8_t *getCurentScrin(){
return getScrin(stackScrin[currentScrinPosition]);;
}

void GoToCommand(My_ObjKey_TypeDef key){
	if(key.reserv&0x80){																		//переходы все без увеличения стека >127
		currentScrinPosition--;	
	}
	uint8_t reserv = key.reserv&0x7f;
	
	if(reserv==1){																		//переход со смещением на выбранный пункт меню
		key.objektadres+=paramsArray[key.paramadres]; 
	}	
	if(reserv==2){																		//переход со смещением на выбранный пункт меню если установлен другой
		uint16_t mask=paramsArray[key.reservAdres2];
		uint16_t chMenu=paramsArray[key.paramadres];
		if(mask&(1<<chMenu)){
			key.objektadres=key.reservAdres1;//+paramsArray[key.paramadres]; 
		}else{
			key.objektadres+=paramsArray[key.paramadres];
		} 
	}	
	
	if(reserv==3){																		//переход связанный с меню Check
		uint8_t crMenu=key.reservAdres1>>8;
		uint8_t cntMenu=key.reservAdres1;
		uint16_t mask = paramsArray[key.paramadres];
		uint8_t i=0;
		crMenu++;
		while (i<cntMenu){
			if(crMenu>(cntMenu-1)){crMenu=0;}
			if(mask&(1<<crMenu)){
				key.objektadres+=crMenu;
				paramsArray[key.reservAdres2]=crMenu;
				i=cntMenu;	
			}	
			i++;
			crMenu++;
		}	
	}
	if(reserv==4){																		//переход c установкой бита Check
		paramsArray[key.paramadres]|=key.reservAdres1;
	}
	if(reserv==5){																		//переход cо сбросом бита Check
		paramsArray[key.paramadres]&=~key.reservAdres1;
	}	
	
		currentScrinPosition++;
		stackScrin[currentScrinPosition]=key.objektadres;			
	
	
	
	if(reserv==6){																		//переход если значение == со сбросом
		paramsArray[key.paramadres]++;
		if(paramsArray[key.paramadres]==key.reservAdres1){
				stackScrin[currentScrinPosition]=key.reservAdres2;	
		}
	}	
		if(reserv==7){																		//переход на стек если значение == 
		if(paramsArray[key.paramadres]==key.reservAdres1){
				currentScrinPosition=key.reservAdres2;	
		}
	}	
		if(reserv==8){																		//переход на стек если значение != 
		if(paramsArray[key.paramadres]!=key.reservAdres1){
				currentScrinPosition=key.reservAdres2;	
		}
	}	
		if(reserv==9){																		//переход новый если значение == 
		if(paramsArray[key.paramadres]==key.reservAdres1){
				stackScrin[currentScrinPosition]=key.reservAdres2;	
		}
	}	
		if(reserv==10){																		//переход новый если значение != 
		if(paramsArray[key.paramadres]!=key.reservAdres1){
				stackScrin[currentScrinPosition]=key.reservAdres2;	
		}
	}
		if(reserv==11){																		//переход c инкриментом параметра 
				paramsArray[key.reservAdres1]++;	
	}
		if(reserv==12){																		//переход назад на objektadres шагов 
				currentScrinPosition=currentScrinPosition-key.objektadres;	
	}	

//	EPD_1IN54_Clear();
//	EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);
	EPD_1IN54_DrawObjScrin(getCurentScrin());
//	EPD_1IN54_TurnOnDisplay();
	
}

void KeyRun(My_ObjKey_TypeDef key){
	
if(key.command==0x01){	
	const uint8_t * adrcurentScrin=getCurentScrin();
	
	if(key.reserv==0){
		paramsArray[key.paramadres]++;
	}
	if(key.reserv==1){
		paramsArray[key.paramadres]--;
	}
	//EPD_1IN54_DrawObjScrin(adrcurentScrin);
	//EPD_1IN54_ClearObjectMenu(&adrcurentScrin[key.objektadres]);
	//EPD_1IN54_TurnOnDisplay();
	
	//EPD_1IN54_Clear(); // FuulScreen Update
	EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);
	EPD_1IN54_DrawObjScrin(adrcurentScrin);
	EPD_1IN54_TurnOnDisplay();

}
if(key.command==0x02){


}
if(key.command==0x03){
	EPD_1IN54_Clear();
	EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);
	GoToCommand(key);
	EPD_1IN54_TurnOnDisplay();
}

if(key.command==0x04){
	currentScrinPosition--;
	EPD_1IN54_Clear();
	EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);
	EPD_1IN54_DrawObjScrin(getCurentScrin());
	EPD_1IN54_TurnOnDisplay();
}

}

void KeyReaction(uint8_t key,uint8_t newKey){
	
	
	RandomCounter +=NRF_RTC0->COUNTER;
	if(newKey){
		EPD_1IN54_Clear();
		EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);
		EPD_1IN54_DrawObjScrin(getCurentScrin());
		EPD_1IN54_TurnOnDisplay();	
			
	}// else{
		if(key==0x04){
			KeyRun(ObjKey1);
		}
		if(key==0x02){
			KeyRun(ObjKey2);
		}
		if(key==0x01){
			KeyRun(ObjKey3);
		}	
	/*	if(key==0x06){		
				currentScrinPosition++;
				stackScrin[currentScrinPosition]=249;	
				EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);
				EPD_1IN54_DrawObjScrin(getCurentScrin());
				EPD_1IN54_TurnOnDisplay();	
		}	*/
		if(key==0x05){		
				currentScrinPosition++;
				stackScrin[currentScrinPosition]=249;	
				EPD_1IN54_Clear();
				EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);
				EPD_1IN54_DrawObjScrin(getCurentScrin());
				EPD_1IN54_TurnOnDisplay();	
		}	
	//}

	

	
		//paramsArray[0]++;
		//if(paramsArray[0]>5){paramsArray[0]=0;}
		
			//W25Q_ReadData(&frame[0],0,10);
	
	//	EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);
//		EPD_1IN54_PrintRectagle(0xff,2,40,40,100,100); 
//	EPD_1IN54_PrintRectagle(0,2,1,97,97,100);
//	EPD_1IN54_PrintRectagle(0,2,97,1,100,97);
//	EPD_1IN54_PrintRectagle(0xff,2,97,97,104,104);				
	//EPD_1IN54_PrintRectagle(0,2,27,22,60,100);
		//EPD_1IN54_PrintRectagle(0,2,0,0,60,100);
		//EPD_1IN54_PrintRectagle(0,2,100,100,99,99);	
		//EPD_1IN54_LineX(0,2,27,22,60);
		//EPD_1IN54_LineY(0,2,27,22+2,100-2);
		//EPD_1IN54_LineX(0,2,27,122,60);
		//EPD_1IN54_LineY(0,2,87,22+2,100-2);
	//  EPD_1IN54_DrawString(0,0,&frame[0],0xff,&Arial_16_dinamic_table_Adr[0]);			
		//EPD_1IN54_DrawObjectString(&ObgHellou[0]);//Century19_dinamic_table_Adr[0]);			
		//EPD_1IN54_DrawObjectMenu(&ObgMemu[0]); 		
		//EPD_1IN54_DrawObjectPicture(&ObgPict1[0]); 	
		//EPD_1IN54_DrawObjectString(&Obgkeytext1[0]);
		//EPD_1IN54_DrawObjectKeyRigth(&ObgkeyRect1[0]);
		//	EPD_1IN54_PrintFillRectagle(0,0,0,97,97);
//	EPD_1IN54_PrintFillRectagle(0xff,0,97,97,104);
//	EPD_1IN54_PrintFillRectagle(0xff,97,0,104,97);
//	EPD_1IN54_PrintFillRectagle(0,97,97,104,104);	
	
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


void EPD_1IN54_ClearObjectMenu(const uint8_t *object) {
	uint8_t x=object[0];
	uint8_t y=object[1];	
	uint8_t dy=object[3];
	uint8_t vievpos=object[8];
	uint8_t Font=object[11];
	FontSettings_MyTypeDef fs = FontSettings(Font);
	uint8_t dx=(fs.heigth*vievpos)<<3;
	EPD_1IN54_PrintFillRectagle(0xff,x,y,dx,dy);
}

uint16_t EPD_1IN54_DrawObjectFsSelCheckMenu(const uint8_t *object,uint8_t SelCheck) {
	My_StringObg_TypeDef setting;
	setting.x=object[0];
	uint8_t yType=(object[1]&0x03);
	uint8_t xLineSpase=object[1]>>2;
	uint16_t adrpos=(object[2]<<8)|object[3];
	uint16_t adrcheck=(object[4]<<8)|object[5];
	uint8_t vievpos=object[6];
	uint8_t countpos =object[7];
	setting.color=object[8];
	setting.Font=object[9];	
	uint8_t checkArrLen = object[10];	 
	const uint8_t *checkArr=&object[11];	
	const uint8_t *strarrr=&object[11+checkArrLen];	
	FontSettings_MyTypeDef fs = FontSettings(setting.Font);
	uint8_t startpos=0;
	uint16_t n=8;
	uint16_t pos = paramsArray[adrpos];
	
//	setting.yin=5;
	
	if(pos>=countpos){
		if((pos-countpos)<10){pos=0;}
			else{pos=countpos-1;}
		paramsArray[adrpos]=pos;
	}	
	if(pos>(vievpos-1)){
		if(pos>(countpos-vievpos)){
			startpos=countpos-vievpos;
		}else{
			startpos=pos;
		}
	}
	
	setting.y=0;
	setting.xin=0;
	setting.xout=0;		
	setting.yin=6;
	setting.yout=4;
	
	uint8_t poscnt=0;
	uint8_t chcnt=0;
	uint8_t j=0;
	
	 
	
	for(uint8_t i = 0;i<countpos;i++){
		uint8_t Str_len = 0;	
		if(i>=startpos){
			if(i==pos){setting.color=0x00;}else{setting.color=0xff;}
			setting.String=&strarrr[chcnt];
					
			setting.check=0;
			if(SelCheck){
				uint16_t checkBt=paramsArray[adrcheck];
				if((checkBt>>i)&0x01){
					setting.check=2;
					Str_len+=EPD_1IN54_myDrawStringPixelLength(&checkArr[0],setting.Font);					
					setting.checkString = &checkArr[0];
				}				
			}
			
			
			Str_len+=EPD_1IN54_myDrawStringPixelLength(&setting.String[0],setting.Font);
			
			setting.dy = Str_len;
			if(yType==0){
				setting.y = 3;	
			}
			if(yType==1){
				if(Str_len<196){
					setting.y=100-(Str_len>>1);
				}else{
					setting.y=3;
					setting.dy = 200;
				}
			}
			if(yType==2){
				if(Str_len<196){
					setting.y=196-Str_len-3;
				}else{
					setting.y=3;
					setting.dy = 200;
				}
			}
			
			n+=EPD_1IN54_myDrawString(setting);
			setting.x+=(fs.heigth<<3)+(xLineSpase<<3);
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


uint16_t EPD_1IN54_DrawObjectSelCheckMenu(const uint8_t *object,uint8_t SelCheck) {
	My_StringObg_TypeDef setting;
	setting.x=object[0];
	setting.y=object[1];
	setting.xin=0;
	setting.xout=0;
	setting.yin=object[2];
	setting.dy=object[3];
	setting.yout=0;
	uint16_t adrpos=(object[4]<<8)|object[5];
	uint16_t adrcheck=(object[6]<<8)|object[7];
	uint8_t vievpos=object[8];
	uint8_t countpos =object[9];
	setting.color=object[10];
	setting.Font=object[11];	
	const uint8_t *strarrr=&object[12];		
  FontSettings_MyTypeDef fs = FontSettings(setting.Font);	
	uint8_t startpos=0;
	uint16_t n=8;
	uint16_t pos = paramsArray[adrpos];
	if(pos>=countpos){
		if((pos-countpos)<10){pos=0;}
			else{pos=countpos-1;}
		paramsArray[adrpos]=pos;
	}	
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
			setting.check=0;
			if(SelCheck){
				uint16_t checkBt=paramsArray[adrcheck];
				if((checkBt>>i)&0x01){
					setting.check=2;
					const uint8_t checkArr[]={' ','(','o','n','o','n',')','#'};
					setting.checkString = &checkArr[0];
				}				
			}
			n+=EPD_1IN54_myDrawString(setting);
			setting.x+=(fs.heigth<<3);
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




uint8_t EPD_1IN54_DrawObjectString(const uint8_t *object){
	
	My_StringObg_TypeDef setting;
	setting.x=object[0];
	setting.y=object[1];
	setting.xin=object[2];
	setting.xout=object[3];
	setting.yin=object[4];
	setting.yout=object[5];
	setting.dy=0;
	setting.color=object[6];
	setting.Font=object[7];
	setting.String=object+8;
	setting.check=0;
	
	return 8+EPD_1IN54_myDrawString(setting);
}

void EPD_1IN54_DrawObjectItemString(const uint8_t *object){
	
	My_StringObg_TypeDef setting;
	setting.x=object[0];
	setting.y=object[1];
	setting.xin=object[2];
	setting.xout=object[3];
	setting.dy=object[4];
	setting.yout=0;
	uint8_t patams=(object[5]<<8)+object[6];
	setting.color=object[7];
	setting.Font=object[8];
	setting.yin=object[9];
	const uint8_t *data = object+10;
	setting.check=0;
	
	uint8_t i = 10;
	uint8_t cnt = 0;
	while(object[i]!=0x0a){
		if(object[i]=='#'){
			if(paramsArray[patams]==cnt){
				setting.String=data;				
				EPD_1IN54_myDrawString(setting);			
				break;
			}
			data = object+2+i;
			setting.yin=object[1+i];		
			cnt++;
		}
		i++;
	}
}



void EPD_1IN54_DrawText(uint8_t *Text,uint16_t x,uint16_t y,uint16_t xLen,uint16_t yLen,uint8_t border,uint8_t xspase,uint8_t color,uint8_t Font){
	//uint8_t Strbuf[50];
	String_MyTypeDef setting;
	setting.y=y;
	setting.x=x;
	setting.xin=0;
	setting.xout=0;
	setting.yout=0;
	setting.dy=yLen;
	setting.yin=border;
	setting.String=&Text[0];
	setting.Font=Font;
	
	FontSettings_MyTypeDef fs = FontSettings(setting.Font);		
	uint8_t strwe=fs.heigth<<3;
	

	if(border>>3){
		EPD_1IN54_PrintFillRectagle(color,setting.x,setting.y,border,setting.dy);
		setting.x+=border;
	}	
	if(((border%8)>>2)&&(xspase&0x01)){
		setting.xin=5;
		setting.xout=5;
	}
uint16_t len=0;

	//uint16_t i=0;
	uint16_t yMax=yLen-(border<<1);

	

	len=EPD_1IN54_StringRamPixelLength(&Text[0],setting.Font);	
	if((xspase&0x06)==0){setting.yin=border;}
	if((xspase&0x06)==2){setting.yin=border+((yMax-len)>>1);}
	if((xspase&0x06)==4){setting.yin=border+yMax-len;}		
	EPD_1IN54_PrintRamString(setting,color);
	setting.x+=strwe;
		uint8_t dxEnd = (xLen-setting.x)>>3;	
		if(dxEnd){
			EPD_1IN54_PrintFillRectagle(color,setting.x,setting.y,dxEnd<<3,setting.dy);
	}	
}



uint16_t EPD_1IN54_DrawTextList(const uint8_t *ListText,uint16_t x,uint16_t y,uint16_t xLen,uint16_t yLen,uint8_t border,uint8_t xspase,uint8_t color,uint8_t Font){
	//setting.String=ListText+1;
	uint8_t Strbuf[50];
	String_MyTypeDef setting;
	setting.y=y;
	setting.x=x;
	setting.xin=0;
	setting.xout=0;
	setting.yout=0;
	setting.dy=yLen;
	setting.yin=border;
	setting.String=&Strbuf[0];
	setting.Font=Font;
	
	uint8_t xin = 0;
	uint8_t xout =0;
	uint8_t n = 1;		
	uint8_t spase = 1;	
	FontSettings_MyTypeDef fs = FontSettings(setting.Font);		
	uint8_t strwe=fs.heigth<<3;
	

	if(border>>3){
		EPD_1IN54_PrintFillRectagle(color,setting.x,setting.y,border,setting.dy);
		setting.x+=border;
	}	
	if(((border%8)>>2)&&(xspase&0x01)){
		spase=0;
		setting.xin=5;
		setting.xout=5;
	}
	
	uint16_t len=0;
	uint16_t AktualLen=0;
	uint16_t j=0;
	uint16_t linesLen=0;
	uint16_t linesEnd=0;
	uint16_t i=0;//setting.yin;
	uint16_t yMax=yLen-(border<<1);
	uint16_t xMax=setting.x+strwe+spase*8;
	uint16_t textFinish=0;
	
	while((xMax<=xLen)&&(!textFinish)){
		while((ListText[i]!=0x20)&&(ListText[i]!=0)&&(ListText[i]!=0x0d)&&(j<50)){
			Strbuf[j]=ListText[i];
			i++;
			j++;
		}
		Strbuf[j]=0;
		len=EPD_1IN54_StringRamPixelLength(&Strbuf[0],setting.Font);		
		if(ListText[i]==0){
				if(len<yMax){
					AktualLen=len;
					linesEnd=j;
					len=yMax;
					textFinish=1;
					linesLen=0;
				}
			}
				
				
			if((len>=yMax)||(ListText[i]==0x0d)){
			if(ListText[i]!=0x0d){
					Strbuf[linesEnd]=0;
					i=linesLen;
			}
			else{
				i++;
				AktualLen=len;
			}		

			if((xspase&0x06)==0){setting.yin=border;}
			if((xspase&0x06)==2){setting.yin=border+((yMax-AktualLen)>>1);}
			if((xspase&0x06)==4){setting.yin=border+yMax-AktualLen;}		
			
			EPD_1IN54_PrintRamString(setting,color);
			if(xspase&0x01){
				if(spase){
					setting.xin=5;
					setting.xout=5;
					setting.x+=strwe;
					xMax=setting.x+strwe+8;
					spase=0;
				}else{
					setting.xin=0;
					setting.xout=0;
					setting.x+=8+strwe;
					xMax=setting.x+strwe;
					spase=1;
				}
			}else{
				setting.x+=strwe;
				xMax=setting.x+strwe;
			}				
			j=0;	
		}else{
			AktualLen=len;
			Strbuf[j]=0x20;
			j++;
			linesEnd=j;	
			i++;
			linesLen=i;		
		}
	}
	
		uint8_t dxEnd = (xLen-setting.x)>>3;	
		if(dxEnd){
			EPD_1IN54_PrintFillRectagle(color,setting.x,setting.y,dxEnd<<3,setting.dy);
	}	
		
	return linesLen;
}


uint8_t EPD_1IN54_DrawMemo(My_StringObg_TypeDef setting,const uint8_t *memoText,uint8_t border_xin, uint8_t border_xout,uint8_t xspase){
	setting.check=0;
	setting.String=memoText+1;
	setting.yin =memoText[0]; 
	uint8_t xin = setting.xin;
	uint8_t xout =setting.xout;
	uint8_t n = 1;		
	uint8_t spase = 0;		
	FontSettings_MyTypeDef fs = FontSettings(setting.Font);		
	uint8_t strwe=fs.heigth;
	uint8_t strweZ;
	if(setting.xin+setting.xout){
			strweZ=strwe+((setting.xin+setting.xout)>>3)+1;	
	}
	strweZ=strweZ<<3;
	strwe=strwe<<3;
	
	
	if(border_xin){
		EPD_1IN54_PrintFillRectagle(setting.color,setting.x,setting.y,border_xin<<3,setting.dy);
		setting.x+=border_xin<<3;
	}
	
	uint16_t len=EPD_1IN54_myDrawStringPixelLength(setting.String,setting.Font);
	if((len+setting.yin)<200){
		setting.yout=200-setting.yin-len;
	}else{
		setting.yout=0;
			}	
	n+=EPD_1IN54_myDrawString(setting);
	if(memoText[n+1]!=0x0A){
		while(memoText[n]=='#'){
			n++;
			if(xspase){
				if(spase){
					setting.xin=xin;
					setting.xout=xout;
					setting.x+=strwe;
					spase=0;
				}else{
					setting.xin=0;
					setting.xout=0;
					setting.x+=strweZ;
					spase=1;
				}
			}else{setting.x+=strwe;}
			
			setting.yin=memoText[n];	
			n++;
			setting.String=memoText+n;
			
			len=EPD_1IN54_myDrawStringPixelLength(setting.String,setting.Font);
			if((len+setting.yin)<200){
				setting.yout=200-setting.yin-len;
			}else{
				setting.yout=0;
			}
			n+=EPD_1IN54_myDrawString(setting);	
			if(memoText[n+1]==0x0A)break;
		}
	}
	
	

		
	if(xspase){	
			if(spase){
				setting.x+=strwe;
				spase=0;
			}else{
				setting.x+=strweZ;
				spase=1;
			}			
	}else{setting.x+=strwe;}
	
	if(border_xout){ 
		EPD_1IN54_PrintFillRectagle(setting.color,setting.x,setting.y,border_xout<<3,setting.dy);
		setting.x+=(border_xout<<3);
	}
	
	return setting.x;
}

uint8_t EPD_1IN54_DrawObjectMemo(const uint8_t *object){
	
	My_StringObg_TypeDef setting;
	setting.x=object[0];
	setting.y=object[1];
	setting.xin=object[2];
	setting.xout=object[3];
	uint8_t border_xin=object[4];
	uint8_t border_xout=object[5];
	uint8_t xspase=object[6];
	setting.dy=object[7];
	setting.color=object[8];
	setting.Font=object[9]; 
	
	return EPD_1IN54_DrawMemo(setting,&object[10],border_xin,border_xout,xspase);
}

const uint8_t *GetArrayGoals(uint8_t arrayNumber){
	const uint8_t *adr;
	
	if(arrayNumber==0){adr=&MemoTextArrayMotiv1Smoc[0];}
	if(arrayNumber==1){adr=&MemoTextArrayMotiv1Alc[0];}
	if(arrayNumber==2){adr=&MemoTextArrayGoalsNoText[0];}
	if(arrayNumber==3){adr=&MemoTextArrayMotiv1Junc[0];}
	if(arrayNumber==4){adr=&MemoTextArrayMotiv1Porn[0];}
	if(arrayNumber==5){adr=&MemoTextArrayGoalsNoText[0];}
	if(arrayNumber==6){adr=&MemoTextArrayGoalsNoText[0];}
	if(arrayNumber==7){adr=&MemoTextArrayMotiv1Buing[0];}
	if(arrayNumber==8){adr=&MemoTextArrayGoalsNoText[0];}
	if(arrayNumber==9){adr=&MemoTextArrayGoalsNoText[0];}	
	
	if(arrayNumber==10){adr=&MemoTextArrayGoalsPanic[0];}
	if(arrayNumber==11){adr=&MemoTextArrayGoalsPanicEnd[0];}
	
return 	adr;	
}

void EPD_1IN54_DrawObjectText(const uint8_t *object){
	uint16_t x=object[0];
	uint16_t y=object[1];
	uint16_t xLen=object[2];
	uint16_t yLen=object[3];
	uint8_t border=object[4];
	uint8_t xspase=object[5];
	uint8_t color=object[6];	
	uint8_t fount=object[7];
  uint16_t params=(object[8]<<8)+object[9];	
	uint16_t checkN=object[10];	
	uint8_t	j=11;
	uint8_t Strbuf[50];
	uint8_t	i=0;
	
	while((object[j])&&(i<50)){
		Strbuf[i]=object[j];
		i++;
		j++;
	}
	
	Strbuf[i]=0;
	if(paramsArray[params]&(1<<checkN)){
		Strbuf[i]=' ';
		i++;		
		Strbuf[i]='(';
		i++;
		Strbuf[i]='O';
		i++;
		Strbuf[i]='N';
		i++;
		Strbuf[i]=')';
		i++;
		Strbuf[i]=0;	
	}
	
	EPD_1IN54_DrawText(&Strbuf[0],x,y,xLen,yLen,border,xspase,color,fount);	
}

uint8_t EPD_1IN54_DrawObjectTextList(const uint8_t *object){

	uint16_t x=object[0];
	uint16_t y=object[1];
	uint16_t xLen=object[2];
	uint16_t yLen=object[3];
	uint8_t border=object[4];
	uint8_t xspase=object[5];
	uint8_t color=object[6];	
	uint8_t fount=object[7];	
	EPD_1IN54_DrawTextList(&object[8],x,y,xLen,yLen,border,xspase,color,fount);	
}


uint8_t EPD_1IN54_DrawObjectTextListGroup(const uint8_t *object){

	uint16_t x=object[0];
	uint16_t y=object[1];
	uint16_t xLen=object[2];
	uint16_t yLen=object[3];
	uint8_t border=object[4];
	uint8_t xspase=object[5];
	uint8_t color=object[6];	
	uint8_t fount=object[7];	
	uint16_t paramsAdr_TextInPagePosition=(object[8]<<8)|object[9];
	uint16_t paramsAdr_Page=(object[10]<<8)|object[11];
	uint16_t paramsAdr_Content=(object[12]<<8)|object[13];	
	const uint8_t * adrarr=GetArrayGoals(paramsAdr_Content);
	if(object[14]){	
		adrarr=GetArrayGoals(paramsArray[paramsAdr_Content]); 
	}	
	uint16_t count=0;
	uint16_t n=0;
	uint16_t data = 0;
	uint16_t i = 0;
	uint16_t cnt = 0;

	//EPD_1IN54_DrawTextList(&adrarr[0],x,y,xLen,yLen,border,xspase,color,fount);	
	if(&adrarr[paramsArray[paramsAdr_Page]]!=oldPageTextList){paramsArray[paramsAdr_TextInPagePosition]=0;}
	oldPageTextList=&adrarr[paramsArray[paramsAdr_Page]];
	
	while(adrarr[i]!=0x0a){
		if(adrarr[i]==0){
			if(paramsArray[paramsAdr_Page]==cnt){
				count=EPD_1IN54_DrawTextList(&adrarr[ data+paramsArray[paramsAdr_TextInPagePosition]],x,y,xLen,yLen,border,xspase,color,fount);	
				break;
			}
			data =i+1;
			cnt++;
		}
		i++;
	}
	
	if(count>0){
		paramsArray[paramsAdr_TextInPagePosition]+=count;
	}else{
		paramsArray[paramsAdr_TextInPagePosition]=0;
		if(object[14]){	paramsArray[paramsAdr_Page]++;}
	}
	
}


uint8_t EPD_1IN54_DrawObjectList(const uint8_t *object){
	My_StringObg_TypeDef setting;
	setting.x=object[0];
	setting.y=object[1];
	setting.xin=object[2];
	setting.xout=object[3];
	uint8_t border_xin=object[4];
	uint8_t border_xout=object[5];
	uint8_t dx = object[6];
	setting.dy=object[7];
	uint8_t xspase=object[8];
	setting.color=object[9];
	setting.Font=object[10];	  
	uint16_t paramsAdr=(object[11]<<8)|object[12];
	uint16_t paramsAdr1=(object[14]<<8)|object[15];	
	const uint8_t * adrarr=GetArrayGoals(paramsAdr1);
	if(object[13]){	
		adrarr=GetArrayGoals(paramsArray[paramsAdr1]);
	}
	
	uint16_t n=0;
	uint16_t data = 0;
	uint16_t i = 0;
	uint16_t cnt = 0;

	while(adrarr[i]!=0x0b){
		if(adrarr[i]==0x0a){
			if(paramsArray[paramsAdr]==cnt){
				n=EPD_1IN54_DrawMemo(setting,&adrarr[data],border_xin,border_xout,xspase);		
				break;
			}
			data =1+i;
			cnt++;
		}
		i++;
	}
	if(n<dx){
		EPD_1IN54_PrintFillRectagle(setting.color,n,setting.y,dx-n,setting.dy);	
	}
	
	return n;
}

uint8_t EPD_1IN54_DrawObjectParamList(const uint8_t *object){
	My_StringObg_TypeDef setting;
	setting.x=object[0];
	setting.y=object[1];
	setting.xin=object[2];
	setting.xout=object[3];
	uint8_t border_xin=object[4];
	uint8_t border_xout=object[5];
	uint8_t xspase=object[6];
	setting.dy=object[7];
	setting.color=object[8];
	setting.Font=object[9];	  
	uint16_t paramsAdr=(object[10]<<8)|object[10];
	uint16_t MemoAdr = MemoTextArrayMotiv1Adres[paramsAdr];
	
	const uint8_t * MotivArray = GetArrayGoals(paramsArray[1]);
	return EPD_1IN54_DrawMemo(setting,&MotivArray[MemoAdr],border_xin,border_xout,xspase);
}

uint8_t findhash(const uint8_t *buf){
	uint32_t hash=0;
		hash=(buf[0]<<16)|(buf[1]<<8)|(buf[2]);
		if(hash==0x00002323)return 1;
	return 0;
}

void EPD_1IN54_StartObjRun(const uint8_t *object){
	My_ObjKey_TypeDef key;
	key.command=object[1];
	key.objektadres=(object[2]<<8)+object[3];
	key.paramadres=(object[4]<<8)+object[5];
	key.reserv=object[6];
	key.reservAdres1=(object[7]<<8)+object[8];
	key.reservAdres2=(object[9]<<8)+object[10];
	GoToCommand(key);
}

void EPD_1IN54_WriteObjKey(const uint8_t *object){
	if(object[0]==1){		//Key1
		ObjKey1.command=object[1];
		ObjKey1.objektadres=(object[2]<<8)+object[3];
		ObjKey1.paramadres=(object[4]<<8)+object[5];
		ObjKey1.reserv=object[6];
		ObjKey1.reservAdres1=(object[7]<<8)+object[8];
		ObjKey1.reservAdres2=(object[9]<<8)+object[10];	
	}
	if(object[0]==2){		//Key2
		ObjKey2.command=object[1];
		ObjKey2.objektadres=(object[2]<<8)+object[3];
		ObjKey2.paramadres=(object[4]<<8)+object[5];
		ObjKey2.reserv=object[6];
		ObjKey2.reservAdres1=(object[7]<<8)+object[8];
		ObjKey2.reservAdres2=(object[9]<<8)+object[10];			
	}
	if(object[0]==3){		//Key3
		ObjKey3.command=object[1];
		ObjKey3.objektadres=(object[2]<<8)+object[3];
		ObjKey3.paramadres=(object[4]<<8)+object[5];
		ObjKey3.reserv=object[6];
		ObjKey3.reservAdres1=(object[7]<<8)+object[8];
		ObjKey3.reservAdres2=(object[9]<<8)+object[10];			
	}	
	if(object[0]==4){		//Soft Key
		My_ObjKey_TypeDef key;
		key.command=object[1];
		key.objektadres=(object[2]<<8)+object[3];
		key.paramadres=(object[4]<<8)+object[5];
		key.reserv=object[6];
		key.reservAdres1=(object[7]<<8)+object[8];
		key.reservAdres2=(object[9]<<8)+object[10];	
		GoToCommand(key);
	}
}

void EPD_1IN54_ObjDataOperation(const uint8_t *object){
	if(object[0]==1){  //stack operation
		uint16_t value =(object[2]<<8)+object[3];
		if(object[1]==0){currentScrinPosition=value;}
		if(object[1]==1){currentScrinPosition+=value;}
		if(object[1]==2){currentScrinPosition-=value;}
	}
	if(object[0]==2){  //logic operation
		uint16_t paramAdr1 =(object[3]<<8)+object[4];
		uint16_t paramAdr2 =(object[5]<<8)+object[6];
		uint16_t value =(object[7]<<8)+object[8];
		uint16_t temp=0;
		if(object[2]==1){temp=value<<paramsArray[paramAdr2];}
		if(object[2]==2){temp=value>>paramsArray[paramAdr2];}
		if(object[2]==3){temp=paramsArray[paramAdr2]<<value;}
		if(object[2]==4){temp=paramsArray[paramAdr2]>>value;}
		if(object[2]==5){temp=paramsArray[paramAdr2]|value;}
		if(object[2]==6){temp=paramsArray[paramAdr2]&value;}
		if(object[2]==7){temp=paramsArray[paramAdr2]^value;}
		if(object[2]==8){temp=paramsArray[paramAdr2];}
		if(object[2]==9){temp=value;}
		
		if(object[1]==1){paramsArray[paramAdr1]>>=temp;}
		if(object[1]==2){paramsArray[paramAdr1]<<=temp;}
		if(object[1]==3){paramsArray[paramAdr1]>>=~temp;}
		if(object[1]==4){paramsArray[paramAdr1]<<=~temp;}		
		if(object[1]==5){paramsArray[paramAdr1]|=temp;}
		if(object[1]==6){paramsArray[paramAdr1]&=temp;}
		if(object[1]==7){paramsArray[paramAdr1]^=temp;}
		if(object[1]==8){paramsArray[paramAdr1]|=~temp;}
		if(object[1]==9){paramsArray[paramAdr1]&=~temp;}
		if(object[1]==10){paramsArray[paramAdr1]^=~temp;}
		if(object[1]==11){paramsArray[paramAdr1]=temp;}
	}
	if(object[0]==3){  //Arifmetic operation
		uint16_t paramAdr1 =(object[3]<<8)+object[4];
		uint16_t paramAdr2 =(object[5]<<8)+object[6];
		uint16_t value =(object[7]<<8)+object[8];
		
		if(object[1]==1){paramsArray[paramAdr1]=value;}
		if(object[1]==2){paramsArray[paramAdr1]+=value;}
		if(object[1]==3){paramsArray[paramAdr1]-=value;}
		if(object[1]==4){paramsArray[paramAdr1]*=value;}
		if(object[1]==5){paramsArray[paramAdr1]/=value;}
		if(object[1]==6){paramsArray[paramAdr1]%=value;}
		if(object[1]==7){paramsArray[paramAdr1]=paramsArray[paramAdr2]+value;}
		if(object[1]==8){paramsArray[paramAdr1]=paramsArray[paramAdr2]-value;}
		if(object[1]==9){paramsArray[paramAdr1]=paramsArray[paramAdr2]*value;}
		if(object[1]==10){paramsArray[paramAdr1]=paramsArray[paramAdr2]/value;}
		if(object[1]==11){paramsArray[paramAdr1]=paramsArray[paramAdr2]%value;}
		if(object[1]==12){paramsArray[paramAdr1]=value-paramsArray[paramAdr2];}
		if(object[1]==13){paramsArray[paramAdr1]=value/paramsArray[paramAdr2];}
		if(object[1]==14){paramsArray[paramAdr1]=value%paramsArray[paramAdr2];}
		if(object[1]==15){paramsArray[paramAdr1]+=paramsArray[paramAdr2];}
		if(object[1]==16){paramsArray[paramAdr1]-=paramsArray[paramAdr2];}
		if(object[1]==17){paramsArray[paramAdr1]*=paramsArray[paramAdr2];}
		if(object[1]==18){paramsArray[paramAdr1]/=paramsArray[paramAdr2];}
		if(object[1]==19){paramsArray[paramAdr1]%=paramsArray[paramAdr2];}
		if(object[1]==20){paramsArray[paramAdr1]=paramsArray[paramAdr2]+paramsArray[value];}
		if(object[1]==21){paramsArray[paramAdr1]=paramsArray[paramAdr2]-paramsArray[value];}
		if(object[1]==22){paramsArray[paramAdr1]=paramsArray[paramAdr2]*paramsArray[value];}
		if(object[1]==23){paramsArray[paramAdr1]=paramsArray[paramAdr2]/paramsArray[value];}
		if(object[1]==24){paramsArray[paramAdr1]=paramsArray[paramAdr2]%paramsArray[value];}
		
	}
	if(object[0]==4){  //Command operation
		uint16_t counter =(object[2]<<8)+object[3];
		uint16_t data = (object[4]<<8)+object[5];
		uint8_t data2 = object[6];
		if(object[1]==1){																								// start count
			if(CounterArray[paramsArray[counter]]==0xffffffff){
					CounterArray[paramsArray[counter]]=0xffffffff-(NRF_RTC0->COUNTER>>3);
			}
		}	
		if(object[1]==2){CounterArray[paramsArray[counter]]=0xffffffff;}// stop count
		if(object[1]==3){
			if(CounterArray[paramsArray[counter]]==0xffffffff){						// read count
					paramsArray[data]=0;
				}else{
					if(CounterArray[paramsArray[counter]]>0x0e0ff1f0){paramsArray[data]=0;
					}else{paramsArray[data]=CounterArray[paramsArray[counter]]/3600;} }
					
				
				}	
			
		if(object[1]==4){saveCounter(paramsArray[counter]);}  					// reset count	
		if(object[1]==5){
			BLE_enable = 0xff;
			//Save_params();
			//NVIC_SystemReset();
		}// ble Set
		if(object[1]==6){BLE_enable = 0;}// ble ReSet
			
		if(object[1]==7){
			if(CounterBackArray[(paramsArray[counter]*5)+data2]==0xffffffff){						// read records count
					paramsArray[data]=0;
				}else{
						if(CounterBackArray[(paramsArray[counter]*5)+data2]>0x0e0ff1f0){paramsArray[data]=0;
					}else{paramsArray[data]=CounterBackArray[(paramsArray[counter]*5)+data2]/3600;}
				}	
			}	
		if(object[1]==77){  //Reset operation
			//W25Q_Flash_erase();	
			W25Q_Flash_Sektor_erase(0);	
			NVIC_SystemReset();
	}
	}
	if(object[0]==5){  //if operation
		uint16_t data1 = (object[2]<<8)+object[3];
		uint16_t value1 = (object[4]<<8)+object[5];
		uint16_t data2 = (object[6]<<8)+object[7];
		uint16_t value2 = (object[8]<<8)+object[9];
		if(object[1] == 1){if(paramsArray[data1]==value1){paramsArray[data2]=value2;}}
		if(object[1] == 2){if(paramsArray[data1]!=value1){paramsArray[data2]=value2;}}
		if(object[1] == 3){if(paramsArray[data1]> value1){paramsArray[data2]=value2;}}
		if(object[1] == 4){if(paramsArray[data1]< value1){paramsArray[data2]=value2;}}	
	}
	if(object[0]==6){  //RandomData operation			
		uint16_t data1 = (object[2]<<8)+object[3];
		uint16_t value1 = (object[4]<<8)+object[5];
		uint16_t data2 = (object[6]<<8)+object[7];
		uint16_t value2 = (object[8]<<8)+object[9];
		if(object[1] == 1){paramsArray[data1]=RandomCounter % value1;}	
		if(object[1] == 2){paramsArray[data1]=data2+(RandomCounter % value1);}	
		if(object[1] == 3){paramsArray[data1]=data2*(RandomCounter % value1);}	
		if(object[1] == 4){paramsArray[data1]=data2+(value2*(RandomCounter % value1));}
	}
}

uint8_t EPD_1IN54_DrawObjectIntString(const uint8_t *object){
	My_StringObg_TypeDef setting;
	setting.x=object[0];
	setting.y=object[1];
	setting.xin=object[2];
	setting.xout=object[3];
	setting.yin=object[4];
	setting.yout=object[5];
	setting.dy=0;
	setting.color=object[6];
	setting.Font=object[7];
	setting.check=object[8];
	setting.intString=paramsArray[((object[9]<<8)|object[10])];
	setting.checkString=object+11;
	
	return 8+EPD_1IN54_myDrawString(setting);
}

const uint8_t *getPicture(uint8_t PictureNumber){
const uint8_t * outAdr=&RombPicture[0];
	
	if(PictureNumber==0){outAdr=&RombPicture[0];}	
	if(PictureNumber==1){outAdr=&SmokingPicture[0];}	
	if(PictureNumber==2){outAdr=&AlcogolPicture[0];}	
	if(PictureNumber==3){outAdr=&DrugsPicture[0];}	
	if(PictureNumber==4){outAdr=&JunkFoodPicture[0];}	
	if(PictureNumber==5){outAdr=&PornPicture[0];}	
	if(PictureNumber==6){outAdr=&SugarPicture[0];}	
	if(PictureNumber==7){outAdr=&SocialMediaPicture[0];}	
	if(PictureNumber==8){outAdr=&ImpulsiveBuyingPicture[0];}
	
	if(PictureNumber==9){outAdr=&RedMeatPicture[0];}
	if(PictureNumber==10){outAdr=&SingleUsePlasticPicture[0];}
	
	if(PictureNumber==0x10){outAdr=&LogoPicture[0];}
	
	if(PictureNumber==0x11){outAdr=&BlueToothMenuPicture[0];}
	if(PictureNumber==0x12){outAdr=&HardResetMenuPicture[0];}
	if(PictureNumber==0x13){outAdr=&BlueToothPicture[0];}	
	if(PictureNumber==0x14){outAdr=&HardResetPicture[0];}	
	if(PictureNumber==0x15){outAdr=&NextMotivPicture[0];}	
	

	if(PictureNumber==0x21){outAdr=&SmokingPictureMini[0];}	
	if(PictureNumber==0x22){outAdr=&AlcogolPictureMini[0];}	
	if(PictureNumber==0x23){outAdr=&DrugsPictureMini[0];}	
	if(PictureNumber==0x24){outAdr=&JunkFoodPictureMini[0];}	
	if(PictureNumber==0x25){outAdr=&PornPictureMini[0];}	
	if(PictureNumber==0x26){outAdr=&SugarPictureMini[0];}	
	if(PictureNumber==0x27){outAdr=&SocialMediaPictureMini[0];}	
	if(PictureNumber==0x28){outAdr=&ImpulsiveBuyingPictureMini[0];}
	
	if(PictureNumber==0x29){outAdr=&RedMeatPictureMini[0];}
	if(PictureNumber==0x2a){outAdr=&SingleUsePlasticPictureMini[0];}
	
	
return outAdr;
}


uint16_t EPD_1IN54_DrawObjectPicture(const uint8_t *object) {
	const uint8_t *adrPicture = getPicture(object[3]);
	return 6+EPD_1IN54_PrintPicture(object[0],object[1],adrPicture[0],adrPicture[1],object[2],&adrPicture[2]);
}


void EPD_1IN54_DrawPartObject(const uint8_t *object,uint8_t params){
	const uint8_t *part;
	uint8_t NumberPart=(object[0]<<8)+object[1];
	if(params){
		NumberPart=paramsArray[(object[0]<<8)+object[1]]+(object[2]<<8)+object[3];
	}
	
	if(NumberPart>0){
		if(NumberPart==0x01){part=&PartObjGoalsSmoking[0];}
		if(NumberPart==0x02){part=&PartObjGoalsAlcogol[0];}
		if(NumberPart==0x03){part=&PartObjGoalsDrugs[0];}
		if(NumberPart==0x04){part=&PartObjGoalsJunkFood[0];}
		if(NumberPart==0x05){part=&PartObjGoalsPorn[0];}
		if(NumberPart==0x06){part=&PartObjGoalsSugar[0];}
		if(NumberPart==0x07){part=&PartObjGoalsSocialMedia[0];}
		if(NumberPart==0x08){part=&PartObjGoalsImpulsiveBuying[0];}		
		if(NumberPart==0x09){part=&PartObjGoalsRedMeat[0];}		
		if(NumberPart==0x0a){part=&PartObjGoalsSingleUsePlastic[0];}	
		
		if(NumberPart==0x10){part=&PartMenuBackChoose[0];}
		if(NumberPart==0x11){part=&PartMenuNoYes[0];}
		if(NumberPart==0x12){part=&PartGoalsMenu[0];}

		
		
		EPD_1IN54_DrawObjScrin(part);
	}
}


void EPD_1IN54_DrawObject(const uint8_t *object){
uint8_t objtype=object[3];
			object+=6;
//uint16_t objdata = objstart+6;
			if(objtype==1){EPD_1IN54_DrawObjectString(object);}
			if(objtype==2){EPD_1IN54_DrawObjectSelCheckMenu(object,0);}
			if(objtype==3){EPD_1IN54_DrawObjectPicture(object);}
			if(objtype==4){EPD_1IN54_DrawObjectKeyRigth(object);}	
		  if(objtype==5){EPD_1IN54_DrawObjectKeyLeft(object);}	
			if(objtype==6){EPD_1IN54_DrawObjectSelCheckMenu(object,1);}
			if(objtype==7){EPD_1IN54_DrawObjectMemo(object);}
			if(objtype==8){EPD_1IN54_DrawObjectItemString(object);}
			if(objtype==9){EPD_1IN54_StartObjRun(object);}
			if(objtype==10){EPD_1IN54_WriteObjKey(object);}
			if(objtype==11){EPD_1IN54_ObjDataOperation(object);}
			if(objtype==12){EPD_1IN54_DrawObjectList(object);}
			if(objtype==13){EPD_1IN54_DrawObjectParamList(object);}
			if(objtype==14){EPD_1IN54_DrawObjectFsSelCheckMenu(object,1);}
			if(objtype==15){EPD_1IN54_DrawObjectFsSelCheckMenu(object,0);}
			if(objtype==16){EPD_1IN54_DrawObjectIntString(object);}
			if(objtype==17){EPD_1IN54_DrawObjectTextListGroup(object);}
			if(objtype==18){EPD_1IN54_DrawPartObject(object,1);}
			if(objtype==19){EPD_1IN54_DrawObjectTextList(object);}
			if(objtype==20){EPD_1IN54_DrawPartObject(object,0);}
			if(objtype==21){EPD_1IN54_DrawObjectText(object);}
}

void EPD_1IN54_DrawObjScrin(const uint8_t *object){
	uint32_t objstart;
	uint32_t find = 0;
	ObjKey1.command = 0;
	ObjKey2.command = 0;
	ObjKey3.command = 0;
		
	objstart = 0;
	while (findhash(&object[objstart])){
			uint16_t newstart =(object[objstart+4]<<8)|object[objstart+5];
			EPD_1IN54_DrawObject(&object[objstart]);			
			if(newstart>0){objstart+=6+newstart;}else break;					
	}	
	
}

void EPD_1IN54_DrawObjStartScrin(){	
	if(WaitUntilIdle()){
		MyePaperInit(1);
	}
	if(WaitUntilIdle()){
		SendCommand(DATA_ENTRY_MODE_SETTING);
		SendData(0x02);                     // X decrement; Y increment
		EPD_1IN54_PrintFillRectagle(0x00,0,0,200,200);	
		EPD_1IN54_TurnOnDisplay();
		EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);	
		EPD_1IN54_TurnOnDisplay();
	}
		Spi_While_ms(1000);
	//	currentScrinPosition=0;
	//	stackScrin[currentScrinPosition]=0;	
	if(WaitUntilIdle()){
		EPD_1IN54_PrintFillRectagle(0xff,0,0,200,200);
		EPD_1IN54_DrawObjScrin(getCurentScrin());
	}
		EPD_1IN54_TurnOnDisplay();
		Spi_While_ms(1000);
}

