/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * Description of project
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "ts_calibration.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static	int timer1=0;
static int timer2=0;
static int state=1;
static int color=LCD_COLOR_BLUE;


/* Private function prototypes -----------------------------------------------*/
static int GetUserButtonPressed(void);
static int GetTouchState (int *xCoord, int *yCoord);

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void){
	HAL_IncTick();

	if(state==1){
		timer1++;
	}else if(state==2){
		timer2++;
	}


}

/**
 * @brief  The application entry point.
 * @retval int
 */


void EXTI0_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);

	if(state==1){
		state = 2;
	}else if (state==2){
		state = 1;
	}



}

void EXTI3_IRQHandler(void){
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);

	//Abfrage
	if(color==LCD_COLOR_BLUE){
		color=LCD_COLOR_GREEN;
	}else if(color==LCD_COLOR_GREEN){
		color=LCD_COLOR_BLUE;
	}

}


int main(void){

	/* MCU Configuration--------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize LCD and touch screen */
	LCD_Init();
	TS_Init(LCD_GetXSize(), LCD_GetYSize());
	/* touch screen calibration */
	//	TS_Calibration();



	/* Clear the LCD and display basic starter text */
	LCD_Clear(LCD_COLOR_BLACK);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font20);
	// There are 2 ways to print text to screen: using printf or LCD_* functions
	LCD_DisplayStringAtLine(0, "    ");
	// printf Alternative
	LCD_SetPrintPosition(1, 0);
	printf(" EXTI Interrupte 30");
	LCD_SetPrintPosition(2, 0);
	printf("  ");

	LCD_SetFont(&Font8);
	LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLACK); // TextColor, BackColor
	LCD_DisplayStringAtLineMode(39, "copyright Kimeswenger", CENTER_MODE);


	// Konfiguration GPIO(timer und led)

	GPIO_InitTypeDef timer;
	timer.Mode = GPIO_MODE_IT_RISING;
	timer.Alternate = 0;
	timer.Speed = GPIO_SPEED_FAST;
	timer.Pin = GPIO_PIN_0;
	timer.Pull = GPIO_PULLDOWN;

	HAL_GPIO_Init(GPIOA, &timer);

	GPIO_InitTypeDef color_switch;
	color_switch.Mode = GPIO_MODE_OUTPUT_PP;
	color_switch.Alternate = 0;
	color_switch.Speed = GPIO_SPEED_MEDIUM;
	color_switch.Pin = GPIO_PIN_3;
	color_switch.Pull = GPIO_PULLUP;

	HAL_GPIO_Init(GPIOG, &color_switch);


	//NVIC konfiguration
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);


	int cnt = 0;
	/* Infinite loop */
	while (1){

		// display timer
		LCD_SetFont(&Font20);
		LCD_SetTextColor(color);
		LCD_SetPrintPosition(5, 0);
		printf("   Timer_1: %.1f", timer1/1000.0);
		LCD_SetPrintPosition(6, 0);
		printf("   Timer_2: %.1f", timer2/1000.0);


		// test touch interface
		int x, y;
		if (GetTouchState(&x, &y)) {
			LCD_FillCircle(x, y, 5);
		}

	}
}



/**
 * Check if User Button has been pressed
 * @param none
 * @return 1 if user button input (PA0) is high
 */
static int GetUserButtonPressed(void) {
	return (GPIOA->IDR & 0x0001);
}

/**
 * Check if touch interface has been used
 * @param xCoord x coordinate of touch event in pixels
 * @param yCoord y coordinate of touch event in pixels
 * @return 1 if touch event has been detected
 */
static int GetTouchState (int* xCoord, int* yCoord) {
	void    BSP_TS_GetState(TS_StateTypeDef *TsState);
	TS_StateTypeDef TsState;
	int touchclick = 0;

	TS_GetState(&TsState);
	if (TsState.TouchDetected) {
		*xCoord = TsState.X;
		*yCoord = TsState.Y;
		touchclick = 1;
		if (TS_IsCalibrationDone()) {
			*xCoord = TS_Calibration_GetX(*xCoord);
			*yCoord = TS_Calibration_GetY(*yCoord);
		}
	}

	return touchclick;
}


















