/*
 * ECE 153B
 *
 * Name(s): Kole Kikuta, Alex Xu
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
#define OPENDOOR 0
#define CLOSEDOOR 1

int temp_disabled = 0;
int temp_disabled_counter = 3000;

// cannot use var name "inputs" here, we have a global var named that
void UART_onInput(char* inputs, uint32_t size) {
	//check input string, print out console message, rotate motor, print out console message, wait 3 seconds
	if(inputs == "open\n"){
		sprintf(buffer, "Door opening.\n");
		UART_print(buffer);
		//open door by enabling systick, interrupts start happening, have accelerometer disable systick to stop rotating
		setDire(1);
		door_spinning = 1;
		door_spinning = 0;
		sprintf(buffer, "Door opened.\n"); 
		UART_print(buffer);
		// delay(3000);
		temp_disabled = 1;
	}else if(inputs == "close\n"){
		sprintf(buffer, "Door closing\n");
		UART_print(buffer);
		setDire(0);
		door_spinning = 1;
		door_spinning = 0;
		sprintf(buffer, "Door closed.\n"); 
		UART_print(buffer);
		// delay(3000);
		temp_disabled = 1;
	}else{
		setDire(2);
		sprintf(buffer, "Not valid input (open, close). Stopping door.\n");
		UART_print(buffer);
	}
}

int main(void) {

	double x, y, z;
	uint8_t Data_Send = 0;
	uint8_t Data_Receive;
	uint8_t SecondaryAddress = 0x48 << 1;
	uint8_t tempState = 0;
	uint8_t prevTemp = 0;

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
	USART_Init(USART2);
	UART2_Init();
	DMA_Init_UARTx(DMA1_Channel7 , USART2);

	sprintf(buffer, "Program Starts.\r\n");
	UART_print(buffer);

	// 1. The default position for the door should be closed (down facing). You should use accelerometer
	//		 to ensure that it's always facing down regardless of initial motor position.
	// Y = -1 closed
	// Z = 1 open
	
	readValues(&x, &y, &z);
	if(y > -1) {
		setDire(CLOSEDOOR);
		door_spinning = 1;
		while(y > -1){
			readValues(&x, &y, &z);
		}
	}
	
	
	
	while(1) {
		//read accelerometer
		readValues(&x, &y, &z);
		//read tempurature sensor
		Data_Send = 0;
		I2C_SendData(I2C1, SecondaryAddress, &Data_Send, 1);
		I2C_ReceiveData(I2C1, SecondaryAddress, &Data_Receive, 1);

		// sprintf(buffer, "Tempurature: %d\n", Data_Receive);
		// UART_print(buffer);

		int temp = (Data_Receive & 0x7F) - (((Data_Receive & 0x80) != 0) ? 128 : 0);

		if(temp != prevTemp){
			sprintf(buffer, "Tempurature: %d\n", Data_Receive);
			UART_print(buffer);
			prevTemp = Data_Receive;
		}
		/* * * * * * * * * * * * * * * * * * * * * * * * * */
		// logic for stopping door when door is finished closing or opening
		
		if (z >= 1 || y <= -1) {
			setDire(2); // set dire to stop, check rotate() in motor.c
			door_spinning = 0;
		}
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * */
		
		
		if (temp_disabled == 0) {
			/* * * * * * * * * * * * * * * * * * * * * * * * * */
			// schmitt upper
			if (temp > 30 && z < 1) {
				sprintf(buffer, "Temperature too high. Door opening.\r\n");
				UART_print(buffer);
				
				setDire(OPENDOOR);
				door_spinning = 1;
				// while (z < 1) readValues(&x, &y, &z);
				// door_spinning = 0;
				
				sprintf(buffer, "Door opened.\r\n");
				UART_print(buffer);
			}
			
			/* * * * * * * * * * * * * * * * * * * * * * * * * */
			// schmitt lower
			if (temp < 20 && y > -1) {
				sprintf(buffer, "Temperature too low. Door closing.\r\n");
				UART_print(buffer);
				
				setDire(CLOSEDOOR);
				door_spinning = 1;
				// while (y > -1) readValues(&x, &y, &z);
				// door_spinning = 0;
				
				sprintf(buffer, "Door closed.\r\n");
				UART_print(buffer);
			}
		} else if (temp_disabled == 1) {
			// delay(3000);
			// not using delay() because we need the while loop checking acc even when temp is disabled
			temp_disabled_counter -= 10; // 0.1 seconds per loop
			if (temp_disabled_counter <= 0) {
				temp_disabled = 0;
				temp_disabled_counter = 3000;
			}
		}
		/* * * * * * * * * * * * * * * * * * * * * * * * * */
		
		
		
		
		// sprintf(buffer, "X: %.2f, Y: %.2f, Z: %.2f\n", x, y, z);
		// UART_print(buffer);



		/*
		//for console control of door
		//UART_onInput(inputs, IO_SIZE);

		//check accelerometer readings to end opening/closing door
		//need to find acceleration parameters that make this work
		if(((dire == 1) && (z == 1)) || ((dire == 0) && (y == 1))){
			SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
		}

		//Check if tempurature goes above threshold
		if((Data_Receive > 22) && (tempState == 0)){
			sprintf(buffer, "Tempurature too high. Door opening.\n");
			UART_print(buffer);
			//open door, how do open and read accelerometer in non blocking
			setDire(1);
			SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
			sprintf(buffer, "Door opened.\n");
			UART_print(buffer);
			//Check if tempurature goes below threshold
		}else if(Data_Receive < 18 && (tempState == 1)){
			sprintf(buffer, "Tempurature too low. Door closing.\n");
			UART_print(buffer);
			//close door
			setDire(0);
			SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
			sprintf(buffer, "Door closed.\n");
			UART_print(buffer);
		}
		*/
		//TODO
		LED_Toggle();
		//tempurature and acceleration measurement at .1 second interval?
		delay(100);
	}
}
