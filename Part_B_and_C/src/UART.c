/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Project
 */


#include "UART.h"
#include "DMA.h"

static volatile DMA_Channel_TypeDef * tx;

static volatile uint8_t data_t_0[IO_SIZE];
static volatile uint8_t data_t_1[IO_SIZE];
static volatile uint8_t input_size = 0;
static volatile uint8_t pending_size = 0;
static volatile uint8_t * active = data_t_0;
static volatile uint8_t * pending = data_t_1;
static volatile uint8_t index = 0;

#define SEL_0 1
#define BUF_0_EMPTY 2
#define BUF_1_EMPTY 4
#define BUF_0_PENDING 8
#define BUF_1_PENDING 16

void transfer_data(char ch);
void on_complete_transfer(void);

//need to set up correct DMA channel to use USARTx_TX
void UART1_Init(void) {
	UART1_GPIO_Init();
	//enable usart1 clock
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	//select system clock as clock source
	RCC->CCIPR &= ~RCC_CCIPR_USART1SEL;
	RCC->CCIPR |= RCC_CCIPR_USART1SEL_0;

	tx = DMA1_Channel4;

	DMA_Init_UARTx(tx, USART1);
	tx->CMAR = (uint32_t)active;
	DMA1_CSELR->CSELR &= ~DMA_CSELR_C4S;
	DMA1_CSELR->CSELR |= (1<<13); //(0010: Channel 7 mapped on USART2_TX)

	USART_Init(USART1);
	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn, 1);  // Set USART2 interrupt priority to 1
}

//need to set up correct DMA channel to use USARTx_TX
void UART2_Init(void) {
	UART2_GPIO_Init();
	//enable usart clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	//select system clock as clock source
	RCC->CCIPR &= ~RCC_CCIPR_USART2SEL;
	RCC->CCIPR |= RCC_CCIPR_USART2SEL_0;

	tx = DMA1_Channel7;

	DMA_Init_UARTx(tx, USART2);
	tx->CMAR = (uint32_t)active;
	DMA1_CSELR->CSELR &= ~DMA_CSELR_C7S;
	DMA1_CSELR->CSELR |= (1<<25);

	USART_Init(USART2);
	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_SetPriority(USART2_IRQn, 1);
}

void UART1_GPIO_Init(void) {
	//enable port b clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	//set output mode to alternate function
	GPIOB->MODER &= ~GPIO_MODER_MODE6;
	GPIOB->MODER |= GPIO_MODER_MODE6_1;
	GPIOB->MODER &= ~GPIO_MODER_MODE7;
	GPIOB->MODER |= GPIO_MODER_MODE7_1;
	//Set AFR
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL6;
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL7;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_0;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_1;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_2;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7_0;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7_1;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7_2;
	//set pins to very high speed
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7;
	//set push pull output type
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT6;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT7;
	//pull up resistors
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD6;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD7;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_0;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD7_0;
}

void UART2_GPIO_Init(void) {
	//clock enable
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	//output mode to alternate function
	GPIOA->MODER &= ~GPIO_MODER_MODE2;
	GPIOA->MODER |= GPIO_MODER_MODE2_1;
	GPIOA->MODER &= ~GPIO_MODER_MODE3;
	GPIOA->MODER |= GPIO_MODER_MODE3_1;
	//set afr
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2;
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL3;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL3_0;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL3_1;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL3_2;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_0;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_1;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_2;
	//Set pins to very high speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3;
	//set push pull output type
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT2;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT3;
	//pull up resistors
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD2;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD3;
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD2_0;
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD3_0;
}

//copied from final part a, not sure if its right for this
void USART_Init(USART_TypeDef * USARTx) {
	// Disable USART before configuring settings
	USARTx->CR1 &= ~USART_CR1_UE;

	// Set Communication Parameters
	USARTx->CR1 &= ~(USART_CR1_M);     // 00 -> 8 Data Bits
	USARTx->CR1 &= ~(USART_CR1_OVER8); // 0 -> Oversampling by 16
	USARTx->CR2 &= ~(USART_CR2_STOP);  // 00 -> 1 Stop Bit

	// Set Baud Rate
	// f_CLK = 80 MHz, Baud Rate = 9600 = 80 MHz / DIV -> DIV = 8333 = 0x208D
	USARTx->BRR = 0x208D;

	USARTx->CR3 |= USART_CR3_DMAT;

	// Enable Transmitter/Receiver
	USARTx->CR1 |= USART_CR1_TE | USART_CR1_RE;

	// Enable USART
	USARTx->CR1 |= USART_CR1_UE;

	//Configure NVIC
	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn , 1);

	//idk if we need this or not
	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_SetPriority(USART2_IRQn , 1);

}

/**
 * This function accepts a string that should be sent through UART
*/
void UART_print(char* data) {
	uint8_t data_length = 0;
	
	while(data[data_length] != '\0'){
		data_length++;
	}

	//Transfer char array to buffer
	//Check DMA status. If DMA is ready, send data
	//If DMA is not ready, put the data aside
	
	if ((tx->CCR & DMA_CCR_EN) == 0 ) {
		for(int i = 0; i < IO_SIZE; i++){
			active[i]= '\0';
		}

		for(uint8_t i = 0; i<data_length; i++){
			active[i] = (uint8_t)data[i];
		}
		tx->CMAR = (uint32_t)active;
		// set DMA memory address and size
		tx->CNDTR = data_length;

		// enable DMA channel to start transmission
		tx->CCR |= DMA_CCR_EN;

	} else {
		for(int i = 0; i < IO_SIZE; i++){
			pending[i]= '\0';
		}
		
		for(uint8_t i = 0; i<data_length; i++){
			pending[i] = (uint8_t)data[i];
		}
		pending_size = data_length;
	}
}

/**
 * This function should be invoked when a character is accepted through UART
*/
void transfer_data(char ch) {
	// Append character to input buffer.
	inputs[input_size++] = ch;
	// If the character is end-of-line, invoke UART_onInput
	if(ch == '\n'){
		UART_onInput(inputs, IO_SIZE);
		input_size = 0;
	}
}

/**
 * This function should be invoked when DMA transaction is completed
*/
void on_complete_transfer(void) {
	// If there are pending data to send, switch active and pending buffer, and send data
	if(pending_size > 0){
		tx->CMAR = (uint32_t)pending;
		// set DMA memory address and size
		tx->CNDTR = pending_size;
		// enable DMA channel to start transmission
		tx->CCR |= DMA_CCR_EN;
		pending_size = 0;
	}
}

void USART1_IRQHandler(void){
	// When receive a character, invoke transfer_data
	//not sure what character to use as parameter
	//need to set condition for when transmission is complete?? Part B instructions
	// check if RX is not empty
	NVIC_ClearPendingIRQ(USART1_IRQn);

	if (USART1->ISR & USART_ISR_RXNE) {
		USART1->RQR |= USART_RQR_RXFRQ;
		char ch = USART1->RDR;
		transfer_data(ch);
	}

	// check if transmission is done
	if (USART1->ISR & USART_ISR_TC) {
		// clear transfer complete flag
		USART1->ICR |= USART_ICR_TCCF;
		on_complete_transfer();
	}
}

void USART2_IRQHandler(void){

	NVIC_ClearPendingIRQ(USART2_IRQn);

	if (USART2->ISR & USART_ISR_RXNE) {
		USART2->RQR |= USART_RQR_RXFRQ;
		char ch = USART2->RDR;
		transfer_data(ch);
	}

	// check if transmission is done
	if (USART2->ISR & USART_ISR_TC) {
		// clear transfer complete flag
		USART2->ICR |= USART_ICR_TCCF;
		tx->CCR &= ~DMA_CCR_EN;
		on_complete_transfer();
	}
}
