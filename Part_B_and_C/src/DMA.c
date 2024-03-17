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
	//TODO
	// Initialize DMA
	// 1. Enable the clock for DMA
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	// 2. Wait 20us for DMA to finish setting up
	delay(1);

	// 3. Disable the channel
	DMA1_Channel6->CCR &= ~DMA_CCR_EN;

	// 4. Disable Memory-to-memory mode (edited)
	DMA1_Channel6->CCR &= ~DMA_CCR_MEM2MEM;

	// 5. Set channel priority to high
	DMA1_Channel6->CCR &= ~DMA_CCR_PL;
	DMA1_Channel6->CCR |= DMA_CCR_PL_1;

	// 6. Set peripheral size to 8-bit (edited)
	DMA1_Channel6->CCR &= ~DMA_CCR_PSIZE;

	// 7. Set memory size to 8-bit (edited)
	DMA1_Channel6->CCR &= ~DMA_CCR_MSIZE;

	// 8. Disable peripheral increment mode
	DMA1_Channel6->CCR &= ~DMA_CCR_PINC;

	// 9. Enable memory increment mode
	DMA1_Channel6->CCR |= DMA_CCR_MINC;

	// 10. Disable circular mode
	DMA1_Channel6->CCR &= ~DMA_CCR_CIRC;

	// 11. Set data transfer direction to Memory-to-Peripheral
	DMA1_Channel6->CCR |= DMA_CCR_DIR;
	// DMA1_Channel6->CNDTR = BUFFER_SIZE; // done in main.c?

	// 12. Set the data source to data buffer provided in CRC.h
	DMA1_Channel6->CMAR = (uint32_t) (&DataBuffer);

	// 13. Set the data destination to the data register of the CRC block
	DMA1_Channel6->CPAR = (uint32_t) (&(CRC->DR));

	// 14. Disable half transfer interrupt
	DMA1_Channel6->CCR &= ~DMA_CCR_HTIE;

	// 15. Disable transfer error interrupt
	DMA1_Channel6->CCR &= ~DMA_CCR_TEIE;

	// 16. Enable transfer complete interrupt
	DMA1_Channel6->CCR |= DMA_CCR_TCIE;

	// 17. Set interrupt priority to 0 in NVIC
	NVIC_SetPriority(DMA1_Channel6_IRQn, 0); // highest priority

	// 18. Enable interrupt in NVIC
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}


void DMA1_Channel6_IRQHandler(void){

}
void DMA1_Channel5_IRQHandler(void){

}