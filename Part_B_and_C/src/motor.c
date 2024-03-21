/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Project
 */

#include "stm32l476xx.h"
#include "motor.h"
#include "SysTimer.h"

static const uint32_t MASK = GPIO_ODR_OD5 | GPIO_ODR_OD6 | GPIO_ODR_OD8 | GPIO_ODR_OD9;
static const uint32_t HalfStep[8] = { (~(GPIO_ODR_OD5 | GPIO_ODR_OD6 | GPIO_ODR_OD9) | GPIO_ODR_OD8),
										(~(GPIO_ODR_OD6 | GPIO_ODR_OD9) | GPIO_ODR_OD5 | GPIO_ODR_OD8),
										(~(GPIO_ODR_OD6 | GPIO_ODR_OD8 | GPIO_ODR_OD9) | GPIO_ODR_OD5),
										(~(GPIO_ODR_OD6 | GPIO_ODR_OD8) | GPIO_ODR_OD5 | GPIO_ODR_OD9),
										(~(GPIO_ODR_OD5 | GPIO_ODR_OD6 | GPIO_ODR_OD8) | GPIO_ODR_OD9),
										(~(GPIO_ODR_OD5 | GPIO_ODR_OD8) | GPIO_ODR_OD6 | GPIO_ODR_OD9),
										(~(GPIO_ODR_OD5 | GPIO_ODR_OD8 | GPIO_ODR_OD9) | GPIO_ODR_OD6),
										(~(GPIO_ODR_OD5 | GPIO_ODR_OD9) | GPIO_ODR_OD6 | GPIO_ODR_OD8)};


static volatile uint8_t step = 0;

void Motor_Init(void) {

	//Enable GPIO Clock
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN);

	//Configure pins 5, 6, 8, and 9 as output
	GPIOC->MODER &= ~GPIO_MODER_MODE5;
	GPIOC->MODER |= GPIO_MODER_MODE5_0;
	GPIOC->MODER &= ~GPIO_MODER_MODE6;
	GPIOC->MODER |= GPIO_MODER_MODE6_0;
	GPIOC->MODER &= ~GPIO_MODER_MODE8;
	GPIOC->MODER |= GPIO_MODER_MODE8_0;
	GPIOC->MODER &= ~GPIO_MODER_MODE9;
	GPIOC->MODER |= GPIO_MODER_MODE9_0;

	//Set output speeds to fast
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5;
	GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED5_1;
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED6;
	GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED6_1;
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED8;
	GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_1;
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED9;
	GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED9_1;

	//Set output type to push-pull
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT5;
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT6;
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT8;
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT9;

	//Set to no pull up, no pull down
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD5;
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD6;
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD8;
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD9;

	GPIOC->ODR &= ~MASK;
}

void rotate(void) {
	GPIOC->ODR &= ~MASK;
	GPIOC->ODR |= HalfStep[step];
	if(dire == 0){		//clockwise, step++
		if(step == 7){
			step = 0;
		}else{
			step++;
		}
	}else if (dire == 1){				//counterclockwise, step--
		if(step == 0){
			step = 7;
		}else{
			step--;
		}
	}else {
		// do nothing, stop door.
	}
}

void setDire(int8_t direction) {
	dire = direction;
}
