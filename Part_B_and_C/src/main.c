/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Project
 */

#include "stm32l476xx.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "LED.h"
#include "DMA.h"
#include "UART.h"
#include "motor.h"
#include "SPI.h"
#include "I2C.h"
#include "accelerometer.h"
#include <stdio.h>

static char buffer[IO_SIZE];

void UART_onInput(char* inputs, uint32_t size) {
	//TODO
}

int main(void) {
	// Switch System Clock = 80 MHz
	System_Clock_Init();
	Motor_Init();
	SysTick_Init();
	UART2_Init();
	LED_Init();
	SPI1_GPIO_Init();
	SPI1_Init();
	initAcc();
	I2C_GPIO_Init();
	I2C_Initialization();

	sprintf(buffer, "Program Starts.\r\n");
	UART_print(buffer);

	//start door in closed position

	while(1) {

		//Check if tempurature goes above threshold

		sprintf(buffer, "Tempurature too high. Door opening.\n");
		UART_print(buffer);
		//open door
		sprintf(buffer, "Door opened.\n");
		UART_print(buffer);


		//Check if tempurature goes below threshold

		sprintf(buffer, "Tempurature too low. Door closing.\n");
		UART_print(buffer);
		//close door
		sprintf(buffer, "Door closed.\n");
		UART_print(buffer);

		//Console control of door
		//read console input
		//3 second delay where temp and acc dont control door
		delay(3000);

		//TODO
		LED_Toggle();
		//tempurature and acceleration measurement at .1 second interval
		delay(100);
	}
}
