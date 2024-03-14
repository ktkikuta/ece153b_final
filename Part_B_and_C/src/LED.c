/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Project
 */

#include "LED.h"

void LED_Init(void){
	//Enable GPIO Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	// Initialize Green LED
	GPIOA->MODER &= ~GPIO_MODER_MODE5_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5;
}

void LED_On(void){
	GPIOA->ODR |= GPIO_ODR_OD5;
}

void LED_Off(void){
	GPIOA->ODR &= ~GPIO_ODR_OD5;
}

void LED_Toggle(void){
	GPIOA->ODR ^= GPIO_ODR_OD5;
}
