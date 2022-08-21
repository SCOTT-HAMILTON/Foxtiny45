#include <avr/io.h>
#define F_CPU    8000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "rxUART.h"
#include "utilsUART.h"
#include "debugging.h"
#include "wdtSleep.h"

int findEOL(uint8_t* data, int size) {
	for (int i = 0; i < size-1; ++i) {
		if (ReverseByte(data[i]) == 13 && ReverseByte(data[i+1]) == 10) {
			return i;
		}
	}
	return -1;
}

int main(void) {
	setupUART();
	setup_watchdog(8);
	// INPUT_PULLUP on DB1
	DDRB &= ~(1<<DDB1);
	PORTB |= 1 << PB1;
	// OUTPUT PB4 Low
	DDRB |= (1<<PB4);
	PORTB &= ~(1<<PB4);

	// OUTPUT PB3 HIGH
	DDRB |= 1<<PB3;
	PORTB |= 1<<PB3;
	set_square(125);
	for (;;) {
		wdt_sleep();
		blink(3, 100);
		if (uartBufferIndex > 0) {
			int line_size = findEOL(uartBuffer, uartBufferIndex);
			uint8_t buff[UART_BUFFER_SIZE];
			for (int i = 0; i < line_size+2; ++i) {
				buff[i] = ReverseByte(uartBuffer[i]);
			}
			buff[line_size+2] = '\0';
			if (line_size != -1) {
				sendUART(buff, line_size+2);
				clearUARTBuffer();
				blink(2, 200);
			} else {
				set_square(uartBufferIndex*20);
			}
		}
	}

	return 0;
}
