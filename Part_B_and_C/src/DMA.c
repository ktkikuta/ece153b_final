/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Project
 */

#include "DMA.h"
#include "SysTimer.h"

//copied from lab 6, need to figure out correct channel,
void DMA_Init_UARTx(DMA_Channel_TypeDef * tx, USART_TypeDef * uart) {
	// enable clock for DMA
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	delay(2); // wait for DMA to finish setting up

	// disable channel
	tx->CCR &= ~DMA_CCR_EN;

	// disable memory-to-memory mode
	tx->CCR &= ~DMA_CCR_MEM2MEM;

	// set high channel priority
	tx->CCR &= ~DMA_CCR_PL;
	tx->CCR |= DMA_CCR_PL_1;

	// set peripheral size to 8-bit
	tx->CCR &= ~DMA_CCR_PSIZE;

	// set memory size to 8-bit
	tx->CCR &= ~DMA_CCR_MSIZE;

	// disable peripheral increment mode
	tx->CCR &= ~DMA_CCR_PINC;

	// enable memory increment mode
	tx->CCR |= DMA_CCR_MINC;

	// disable circular mode
	tx->CCR &= ~DMA_CCR_CIRC;

	// set data transfer direction to peripheral-to-memory
	tx->CCR &= ~DMA_CCR_DIR;

	// set data source and destination
	tx->CPAR = (uint32_t) &uart->TDR;

	// disable half transfer interrupt
	tx->CCR &= ~DMA_CCR_HTIE;

	// disable transfer error interrupt
	tx->CCR &= ~DMA_CCR_TEIE;

	// enable transfer complete interrupt
	tx->CCR |= DMA_CCR_TCIE;

	// set interrupt priority to 0
    NVIC_SetPriority(DMA1_Channel6_IRQn, 0);

    // enable interrupt
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}


void DMA1_Channel6_IRQHandler(void){
	NVIC_ClearPendingIRQ(DMA1_Channel6_IRQn);

	// check transfer complete flag
	if (DMA1->ISR & DMA_ISR_TCIF6) {
		// clear transfer complete flag
		DMA1->IFCR = DMA_IFCR_CTCIF6;

		// disable DMA
		DMA1_Channel6->CCR &= ~DMA_CCR_EN;
	}
	// clear global interrupt flag
	DMA1->IFCR = DMA_IFCR_CGIF6;
}
void DMA1_Channel5_IRQHandler(void){
	NVIC_ClearPendingIRQ(DMA1_Channel5_IRQn);

	// check transfer complete flag
	if (DMA1->ISR & DMA_ISR_TCIF5) {
		// clear transfer complete flag
		DMA1->IFCR = DMA_IFCR_CTCIF5;

		// disable DMA
		DMA1_Channel5->CCR &= ~DMA_CCR_EN;
	}
	// clear global interrupt flag
	DMA1->IFCR = DMA_IFCR_CGIF5;
}