#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "jlx12864oled.h"


/* GPIO_Configuration */
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure SPI CLK (PA.05), SPI MOSI (PA.07)
     * as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure LEDs (PC.8, PC.9), RS (PC.0), RES (PC.1), CS (PC.2) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_0 |
    		GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}


/* RCC_Configuration */
void RCC_Configuration(void)
{
	/* Enable USART2 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Enable GPIOA, GPIOC, SPI clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA |
			RCC_APB2Periph_SPI1, ENABLE);
}


/* SPI configuration */
void SPI_Configuration(void)
{
	SPI_InitTypeDef SPI_InitStructure;

	/* OLED_SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;

	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;

	SPI_Init(OLED_SPI, &SPI_InitStructure);

	SPI_Cmd(OLED_SPI, ENABLE);
}


/* Demo LED1 flashing task */
void task1(void *pvParameters) {

        for (;;) {
                GPIO_SetBits(GPIOC, GPIO_Pin_9);
                LCD_PrintStr(1, 2, "Task1: Led 1 - on ");
                vTaskDelay(500);
                GPIO_ResetBits(GPIOC, GPIO_Pin_9);
                LCD_PrintStr(1, 2, "Task1: Led 1 - off");
                vTaskDelay(500);
        }
}


/* Demo LED2 flashing task */
void task2(void *pvParameters) {

	for (;;) {
			GPIO_SetBits(GPIOC, GPIO_Pin_8);
			LCD_PrintStr(2, 2, "Task2: Led 2 - on ");
			vTaskDelay(342);
			GPIO_ResetBits(GPIOC, GPIO_Pin_8);
			LCD_PrintStr(2, 2, "Task2: Led 2 - off");
			vTaskDelay(342);
	}
}


/* Demo row3 banner task */
void task3(void *pvParameters) {
	unsigned char column = 0;

	for (;;) {
		if (column > 130) {
			column = 0;
		}
		else {
			column++;
		}
		LCD_PrintStr(3, column, " Task 3");
		vTaskDelay(25);
	}
}


/* Demo row4 banner task */
void task4(void *pvParameters) {
	unsigned char column = 0;

	for (;;) {
		if (column > 130) {
			column = 0;
		}
		else {
			column++;
		}
		LCD_PrintStr(4, column, " Task 4");
		vTaskDelay(20);
	}
}


/* Demo row5 banner task */
void task5(void *pvParameters) {
	unsigned char column = 0;

	for (;;) {
		if (column > 130) {
			column = 0;
		}
		else {
			column++;
		}
		LCD_PrintStr(5, column, " Task 5");
		vTaskDelay(15);
	}
}


/* Demo row6 banner task */
void task6(void *pvParameters) {
	unsigned char column = 0;

	for (;;) {
			if (column > 130 ) {
				column = 0;
			}
			else {
				column++;
			}
			LCD_PrintStr(6, column, " Task 6");
			vTaskDelay(10);
	}
}


/* Demo harmful task, tries to clear LCD periodically */
void task7(void *pvParameters) {

	for (;;) {
		vTaskDelay(400);
		LCD_ClearScreen();
	}
}


/* Main routine */
int main(void)
{
	/* Hardware initialization */
	RCC_Configuration();
	GPIO_Configuration();
	SPI_Configuration();

	/* IO pins initial state */
	CS_ON;
	RS_OFF;
	RES_OFF;

	/* Queue of data to LCD */
	LCDDataQueue = xQueueCreate( 5, sizeof( LCDMessage ) );
	/* Queue of commands to LCD */
	LCDCmdQueue = xQueueCreate( 5, sizeof( char ) );


	/* Demo test tasks that asynchronously use LCD */

	/* 2 tasks which do LEDs flashing and write LEDs states on LCD */
	xTaskCreate( task1, ( char * ) "Led1", configMINIMAL_STACK_SIZE, NULL, 1,
							( xTaskHandle * ) NULL);
	xTaskCreate( task2, ( char * ) "Led2", configMINIMAL_STACK_SIZE, NULL, 1,
							( xTaskHandle * ) NULL);

	/* 4 tasks which print moving text banners on LCD */
	xTaskCreate( task3, ( char * ) "Row3", configMINIMAL_STACK_SIZE, NULL, 1,
								( xTaskHandle * ) NULL);
	xTaskCreate( task4, ( char * ) "Row4", configMINIMAL_STACK_SIZE, NULL, 1,
									( xTaskHandle * ) NULL);
	xTaskCreate( task5, ( char * ) "Row5", configMINIMAL_STACK_SIZE, NULL, 1,
											( xTaskHandle * ) NULL);
	xTaskCreate( task6, ( char * ) "Row6", configMINIMAL_STACK_SIZE, NULL, 1,
											( xTaskHandle * ) NULL);

	/* Harmful task, tries to clear screen periodically */
//	xTaskCreate( task7, ( char * ) "clearscr", configMINIMAL_STACK_SIZE, NULL, 1,
//											( xTaskHandle * ) NULL);

	/* Start LCD gatekeeper */
	xTaskCreate( LCD_Task, ( char * ) "LCD", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, ( xTaskHandle * ) NULL );

	vTaskStartScheduler();

    while(1)
    {
    }
}
