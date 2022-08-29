#pragma once

#include <avr/io.h>

uint8_t ReverseByte (uint8_t x);
int findEOL(volatile uint8_t* data, int size);
int get_uart_line(uint8_t* data, int size);
int wait_get_line(uint8_t* buffer, int size);
int get_internal_temp();
void send_temps();
