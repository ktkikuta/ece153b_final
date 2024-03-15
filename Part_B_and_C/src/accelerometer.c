#include "SPI.h"
#include "SysTimer.h"
#include "accelerometer.h"

//copied from lab 4c, not sure if correct
void accWrite(uint8_t addr, uint8_t val){
	uint16_t command = ((0 << 15) | (0 << 14) | ((addr & 0x3F) << 8) | val);
	SPI_Transfer_Data(command);
}

uint8_t accRead(uint8_t addr){
	// access SPI_Transfer_Data
	uint16_t command = ((1 << 15) | (0 << 14) | ((addr & 0x3F) << 8) & 0xFF00);
	uint16_t readData = SPI_Transfer_Data(command);
	return (readData & 0xFF);
}

void initAcc(void){
	//set to low power mode
	accWrite(0x2C, 0x0A);
	// set full range mode
	accWrite(0x31, 0x09);
	// enable measurement
	accWrite(0x2D, 0x08);
}

void readValues(double* x, double* y, double* z){
	// find scaler from data sheet
	double scaler = 0.0039;
	// read values into x,y,z using accRead
	uint8_t x_read1 = accRead(0x32);
	uint8_t x_read2 = accRead(0x33);
	uint8_t y_read1 = accRead(0x34);
	uint8_t y_read2 = accRead(0x35);
	uint8_t z_read1 = accRead(0x36);
	uint8_t z_read2 = accRead(0x37);

	*x = (int16_t)((x_read2 << 8) | x_read1) * scaler;
	*y = (int16_t)((y_read2 << 8) | y_read1) * scaler;
	*z = (int16_t)((z_read2 << 8) | z_read1) * scaler;
}
