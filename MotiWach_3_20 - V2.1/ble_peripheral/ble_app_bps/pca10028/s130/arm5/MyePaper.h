/**
 *  @filename   :   epd1in54.h
 *  @brief      :   Header file for e-paper display library epd1in54.cpp
 *  @author     :   Yehui from Waveshare
 * 
 */
//Interfase -------------
#include "MySoftSPI.h"
#define SpiSend_Interfase Soft_Spi_Transmit(data,length);
#define SpiSendSameBit_Interfase Soft_Spi_TransmitSameBit(data,length);
#define SpiSendSameByte_Interfase Soft_Spi_TransmitSameByte(data,length);

//-----------------------

#define DC_Port NRF_GPIO_PORT_SELECT_PORT0  //GPIOC
#define DC_Pin 0

#define CS_Port NRF_GPIO_PORT_SELECT_PORT0  //GPIOC
#define CS_Pin 6

#define Reset_Port NRF_GPIO_PORT_SELECT_PORT0  //GPIOC
#define Reset_Pin 1

#define Busy_Port NRF_GPIO_PORT_SELECT_PORT0  //GPIOC
#define Busy_Pin 2


// Display resolution
#define EPD_WIDTH       200
#define EPD_HEIGHT      200

// EPD1IN54 commands
#define DRIVER_OUTPUT_CONTROL                       0x01
#define BOOSTER_SOFT_START_CONTROL                  0x0C
#define GATE_SCAN_START_POSITION                    0x0F
#define DEEP_SLEEP_MODE                             0x10
#define DATA_ENTRY_MODE_SETTING                     0x11
#define SW_RESET                                    0x12
#define TEMPERATURE_SENSOR_CONTROL                  0x1A
#define MASTER_ACTIVATION                           0x20
#define DISPLAY_UPDATE_CONTROL_1                    0x21
#define DISPLAY_UPDATE_CONTROL_2                    0x22
#define WRITE_RAM                                   0x24
#define WRITE_VCOM_REGISTER                         0x2C
#define WRITE_LUT_REGISTER                          0x32
#define SET_DUMMY_LINE_PERIOD                       0x3A
#define SET_GATE_TIME                               0x3B
#define BORDER_WAVEFORM_CONTROL                     0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION        0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION        0x45
#define SET_RAM_X_ADDRESS_COUNTER                   0x4E
#define SET_RAM_Y_ADDRESS_COUNTER                   0x4F
#define TERMINATE_FRAME_READ_WRITE                  0xFF

typedef struct
{
	uint8_t x;
	uint8_t y;
	uint8_t xin;
	uint8_t xout;
	uint8_t yin;
	uint8_t yout;
	uint8_t dy;
	uint8_t color;
	uint8_t Font;
	uint8_t check;
	uint32_t intString;
	const uint8_t *String;
	const uint8_t *checkString;

} My_StringObg_TypeDef;

typedef struct
{
	uint16_t x;
	uint16_t y;
	uint16_t xin;
	uint16_t xout;
	uint16_t yin;
	uint16_t yout;
	uint16_t dy;
	uint8_t Font;
	uint8_t *String;
	
} String_MyTypeDef;

typedef struct
{
	uint8_t heigth;
	uint8_t spacing;
	uint16_t offsetDinamic;
	uint16_t offsetTable;
	
} FontSettings_MyTypeDef;


void MyePaper_GPIO_DeInit();
void MyePaperInit(uint8_t mode);
void SendCommand(unsigned char command);
void SendGraficData(uint8_t data);
void SendInfoData(unsigned char data);
uint8_t WaitUntilIdle(void);
void Reset(void);
void Sleep(void);

void EPD_1IN54_Clear(uint8_t color);					
void MyePaper_GPIO_Init(void);
void EPD_1IN54_TurnOnDisplay(void);
void EPD_1IN54_TurnOnDisplayPart(void);
void EPD_1IN54_line(uint8_t x,uint8_t y,uint8_t we,uint8_t he);	
void EPD_1IN54_PrintFrame(uint8_t* image_buffer,uint16_t x,uint16_t y,uint16_t image_width,uint16_t image_height);	
void EPD_1IN54_PrintFillRectagle(uint8_t fill,uint16_t x,uint16_t y,uint16_t image_width,uint16_t image_height);
void EPD_1IN54_DrawString(uint8_t x, uint8_t y,uint8_t *str,uint8_t color,const uint8_t *font);
void EPD_1IN54_PrintRectagle(uint8_t color,uint8_t thicknes,uint16_t x,uint16_t y,uint16_t we,uint16_t he);
void EPD_1IN54_LineY(uint8_t color,uint8_t thicknes,uint16_t x,uint16_t y,uint16_t he);
void EPD_1IN54_LineX(uint8_t color,uint8_t thicknes,uint16_t x,uint16_t y,uint16_t we );
//const uint8_t *FontCange(uint8_t fontNumber);
uint16_t EPD_1IN54_SetFrame(uint16_t x,uint16_t y,uint16_t width,uint16_t height);
uint16_t EPD_1IN54_PrintPicture(uint8_t x,uint8_t y,uint16_t dx,uint16_t dy,uint8_t color,const uint8_t *picture);
uint8_t EPD_1IN54_myDrawString(My_StringObg_TypeDef setting);
void MyePaperDualInit();
uint8_t EPD_1IN54_myDrawStringPixelLength(const uint8_t *str,const uint8_t Font);
uint8_t EPD_1IN54_PrintRamString(String_MyTypeDef setting,uint8_t color);
FontSettings_MyTypeDef FontSettings(uint8_t fontNumber);
uint16_t EPD_1IN54_StringRamPixelLength(uint8_t *str,const uint8_t Font);
const uint8_t *FontCange(uint8_t fontNumber) ;
void testrScriin();

/* END OF FILE */