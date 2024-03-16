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
	//check input string, print out console message, rotate motor, print out console message, wait 3 seconds
	if(inputs == "Open door\n"){
		sprintf(buffer, "Door opening.\n");
		UART_print(buffer);
		//open door by enabling systick, interrupts start happening, have accelerometer disable systick to stop rotating
		setDire(1);
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		sprintf(buffer, "Door opened.\n");
		UART_print(buffer);
		delay(3000);
	}else if(inputs == "Close door\n"){
		sprintf(buffer, "Door closing\n");
		UART_print(buffer);
		setDire(0);
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		sprintf(buffer, "Door closed.\n");
		UART_print(buffer);
		delay(3000);
	}else{
		sprintf(buffer, "Not valid input (Open door, Close door).\n")
		UART_print(buffer);
	}
}

int main(void) {

	double x, y, z;
	uint8_t Data_Send = 0;
	uint8_t Data_Receive;
	uint8_t SecondaryAddress = 0x48 << 1;

	// Switch System Clock = 80 MHz
	System_Clock_Init();
	Motor_Init();
	SysTick_Init();
	//UART2_Init();
	LED_Init();
	SPI1_GPIO_Init();
	SPI1_Init();
	initAcc();
	I2C_GPIO_Init();
	I2C_Initialization();
	USART_Init(USART1);
	UART1_Init();
	UART1_GPIO_Init();

	sprintf(buffer, "Program Starts.\r\n");
	UART_print(buffer);

	//start door in closed position

	while(1) {
		//read accelerometer
		readValues(&x, &y, &z);
		//read tempurature sensor
		I2C_SendData(I2C1, SecondaryAddress, &Data_Send, 1);
		I2C_ReceiveData(I2C1, SecondaryAddress, &Data_Receive, 1);

		//for console control of door
		UART_onInput(inputs, IO_SIZE);

		//Check if tempurature goes above threshold
		if(Data_Receive > 22){
			sprintf(buffer, "Tempurature too high. Door opening.\n");
			UART_print(buffer);
			//open door, how do open and read accelerometer in non blocking
			setDire(1);
			SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
			sprintf(buffer, "Door opened.\n");
			UART_print(buffer);
			//Check if tempurature goes below threshold
		}else if(Data_Receive < 18){
			sprintf(buffer, "Tempurature too low. Door closing.\n");
			UART_print(buffer);
			//close door
			setDire(0);
			SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

			sprintf(buffer, "Door closed.\n");
			UART_print(buffer);
		}
		//TODO
		LED_Toggle();
		//tempurature and acceleration measurement at .1 second interval?
		delay(1000);
	}
}
