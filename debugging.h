#pragma once

#include <avr/io.h>

#define TX_STOP_BITS 3

enum Timer1Mode {
	COUNT,
	SQUARE,
	TX_UART
};

void sleep_delay_ms(unsigned long ms);
void set_square(uint8_t us);
void blink(uint8_t n, int delay);
void wait_click();
void debugSquareBuffer();
void sendUART(uint8_t* data, unsigned int size);
