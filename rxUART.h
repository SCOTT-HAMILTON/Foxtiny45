#pragma once

#include <avr/io.h>
#define UART_BUFFER_SIZE 25

volatile uint8_t usiReceived;
volatile uint8_t usiByteReceived;
volatile uint8_t uartBuffer[UART_BUFFER_SIZE];
volatile uint8_t uartBufferIndex;

void setupUART();
void clearUARTBuffer();
