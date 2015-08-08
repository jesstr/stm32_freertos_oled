#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "jlx12864oled.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "font8x6.h"


/* Input data queue to LCD gatekeper */
QueueHandle_t LCDDataQueue;
/* Input command queue to LCD gatekeper */
QueueHandle_t LCDCmdQueue;

void LCD_SPISend(uint16_t data);
void LCD_WriteCmd(unsigned char cmd);
void LCD_WriteData(unsigned char cmd);
void LCD_SetAddr(unsigned char page, unsigned char column);
void LCD_ClearScreen(void);
void LCD_FillScreen(unsigned char data1, unsigned char data2);
void LCD_WriteStr(char *data);


/* Send byte to LCD */
void LCD_SPISend(uint16_t data) {
	CS_OFF;
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    CS_ON;
}


/* Write commant to LCD */
void LCD_WriteCmd(unsigned char cmd) {
	RS_OFF;
	LCD_SPISend(cmd);
	RS_ON;
}


/* Write data to LCD */
void LCD_WriteData(unsigned char data) {
	RS_ON;
	LCD_SPISend(data);
	RS_OFF;
}


/* Set addres (cursor) to LCD to print data from */
void LCD_SetAddr(unsigned char page, unsigned char column) {
	LCD_WriteCmd(0xB0 + page);
	LCD_WriteCmd((( column >> 4 )& 0x0F ) + 0x10);
	LCD_WriteCmd( column & 0x0F );
}


void LCD_ClearScreen(void) {
	unsigned int i, j;

	for (i = 0; i < 8; i++) {
		LCD_SetAddr(i, 2);
		for (j = 0; j < 128; j++) {
			LCD_WriteData(0x00);
		}
	}
}


/* Fill screen with bytes data1 and data2 */
void LCD_FillScreen(unsigned char data1, unsigned char data2) {
	unsigned int i, j;

	for (i = 0; i < 8; i++) {
		LCD_SetAddr(i, 2);
		for (j = 0; j < 64; j++) {
			LCD_WriteData(data1);
			LCD_WriteData(data2);
		}
	}
}


/* Write string to LCD */
void LCD_WriteStr(char *str) {
	unsigned int j, font_offset;

	for (j = 0; j < 128; j++) {

		if (*(str + j / 6) == '\0') {
						return;
					}

		 if(*(str + j / 6) < 0x90)
			 {
			 /* Set offset to english chars */
			 font_offset=0x20;
			 }
		 else
			 {
			 /* Set offset to russian chars */
			 font_offset=0x60;
			 }
		LCD_WriteData(sym[*(str + j / 6) - font_offset][j % 6]);
	}
}


void LCD_PrintStr(unsigned char row, unsigned char col, char *str) {
	LCDMessage message;

	message.column = col;
	message.row = row;
	message.text = str;

	xQueueSend( LCDDataQueue, &message, portMAX_DELAY );
}


/* LCD initialization */
void LCD_Init() {
	RES_OFF;
	vTaskDelay(10 / portTICK_RATE_MS);
	RES_ON;
	vTaskDelay(10 / portTICK_RATE_MS);

	LCD_WriteCmd(0xAE);

	LCD_WriteCmd(0x02);
	LCD_WriteCmd(0x10);

	LCD_WriteCmd(0x40);

	LCD_WriteCmd(0xB0);

	LCD_WriteCmd(0x81);
	LCD_WriteCmd(0xFF);

	LCD_WriteCmd(0xA1);

	LCD_WriteCmd(0xA6);

	LCD_WriteCmd(0xA8);
	LCD_WriteCmd(0x3F);

	LCD_WriteCmd(0xAD);
	LCD_WriteCmd(0x8B);

	LCD_WriteCmd(0x33);

	LCD_WriteCmd(0xC8);

	LCD_WriteCmd(0xD3);
	LCD_WriteCmd(0x00);

	LCD_WriteCmd(0xD5);
	LCD_WriteCmd(0x80);

	LCD_WriteCmd(0xD9);
	LCD_WriteCmd(0x1F);

	LCD_WriteCmd(0xDA);
	LCD_WriteCmd(0x12);

	LCD_WriteCmd(0xDB);
	LCD_WriteCmd(0x40);

	LCD_WriteCmd(0xAF);

}


/*LCD gatekeeper task */
void LCD_Task(void *pvParameters) {
	LCDMessage message;

	LCD_Init();
	vTaskDelay(100 / portTICK_RATE_MS);
	LCD_ClearScreen();


//	vTaskDelay(2000 / portTICK_RATE_MS);
//	LCD_FillScreen(0xFF,0xFF);
//	vTaskDelay(2000 / portTICK_RATE_MS);
//	LCD_ClearScreen();
//
//	LCD_SetAddr(0, 130);
//	LCD_WriteStr("MMM");
//	vTaskDelay(500 / portTICK_RATE_MS);
//	LCD_SetAddr(1, 2);
//	LCD_WriteStr("MMM");

	for (;;) {
		/* Wait for a message to display */
		xQueueReceive( LCDDataQueue, &message, portMAX_DELAY );

		LCD_SetAddr(message.row, message.column);
		LCD_WriteStr(message.text);
	}

}

