#ifndef __JLX12864OLED_H
#define __JLX12864OLED_H

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "queue.h"


/* LCD IO pins */
#define LCD_RS_PORT		GPIOC
#define LCD_RS_PIN		GPIO_Pin_0

#define LCD_RES_PORT	GPIOC
#define LCD_RES_PIN		GPIO_Pin_1

#define LCD_CS_PORT		GPIOC
#define LCD_CS_PIN		GPIO_Pin_2

/* IO pins macros */
#define RS_ON		LCD_RS_PORT->BSRR = LCD_RS_PIN;
#define RS_OFF		LCD_RS_PORT->BRR = LCD_RS_PIN;

#define RES_ON		LCD_RES_PORT->BSRR = LCD_RES_PIN;
#define RES_OFF		LCD_RES_PORT->BRR = LCD_RES_PIN;

#define CS_ON		LCD_CS_PORT->BSRR = LCD_CS_PIN;
#define CS_OFF		LCD_CS_PORT->BRR = LCD_CS_PIN;

/* SPI used with LCD */
#define OLED_SPI	SPI1

/* Commands of LCD gatekeeper */
#define LCD_CMD_CLRSCR	0x00


/* Input data queue to LCD gatekeper */
extern QueueHandle_t LCDDataQueue;
/* Input command queue to LCD gatekeper */
extern QueueHandle_t LCDCmdQueue;

/* The structure passed to the LCD when there is text to display. */
typedef struct
{
	unsigned char column;
	unsigned char row;
	char *text;
} LCDMessage;


/*LCD gatekeeper task */
void LCD_Task(void *pvParameters);
/* Print string on LCD at [row, col] position */
void LCD_PrintStr(unsigned char row, unsigned char col, char *str);
/* Clear LCD */
void LCD_ClearScreen(void);
/* LCD initialization */
void LCD_Init();


#endif /* __JLX12864OLED_H */
