#include "ble.h"

typedef struct
{
	uint8_t command;
	uint16_t objektadres;
	uint16_t paramadres;
	uint8_t reserv;
	uint16_t reservAdres1;
	uint16_t reservAdres2;
} My_ObjKey_TypeDef;

extern My_ObjKey_TypeDef ObjKey1;
extern My_ObjKey_TypeDef ObjKey2;
extern My_ObjKey_TypeDef ObjKey3;
extern uint8_t currentScrinPosition;
extern uint16_t stackScrin[128];
extern uint16_t paramsArray[128];
extern uint32_t CounterArray[16];
extern uint32_t CounterBackArray[80];
extern uint8_t BLE_enable;

extern uint8_t currentScrinPositionUnChenged;
extern uint16_t ScrinUnChenged;
extern uint16_t paramsArrayUnChenged[128];
extern uint32_t CounterArrayUnChenged[16];
extern uint16_t RandomCounter;

//----------------

void KeyReaction(uint8_t key,uint8_t newKey);
uint8_t EPD_1IN54_DrawObjectString(const uint8_t *object);
uint16_t EPD_1IN54_DrawObjectPicture(const uint8_t *object);
void EPD_1IN54_DrawObjectRect(const uint8_t *object);
uint8_t EPD_1IN54_DrawObjectKeyRigth(const uint8_t *object);
uint8_t EPD_1IN54_DrawObjectKeyLeft(const uint8_t *object);
uint16_t EPD_1IN54_DrawObjectMenu(const uint8_t *object); 
void EPD_1IN54_DrawObjScrin(const uint8_t *object);
void EPD_1IN54_ClearObjectMenu(const uint8_t *object);
void LoadRamParams(void);
void Save_params();
void EPD_1IN54_DrawObject(const uint8_t *object);
void EPD_1IN54_DrawObjScrin(const uint8_t *object);
void EPD_1IN54_DrawObjStartScrin();


