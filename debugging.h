#pragma once

#include <avr/io.h>

enum Timer1Mode {
	COUNT,
	SQUARE
};

void sleep_delay_ms(unsigned long ms);
void set_square(uint8_t us);
void blink(uint8_t n, int delay);
void wait_click();
void debugSquareBuffer();
