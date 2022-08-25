#include <avr/io.h>
#define F_CPU    8000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <limits.h>

#include "rxUART.h"
#include "utilsUART.h"
#include "debugging.h"
#include "wdtSleep.h"

static char mdechex[17] = "0123456789ABCDEF";

int dechex (unsigned int dec, char *hex, int len, int fill) {
	if (hex == NULL) {
    return -1;
	}
  int size = 0;
  int bakdec = dec;
	for (;;) {
    size++;
		dec >>= 4;
		if (dec == 0) {
			break;
		}
	}
  dec = bakdec;
  int i = 0;
  int s = fill-size;
	for (;;) {
    hex[s+size-i-1] = mdechex[dec & 0xF];
    i++;
		dec >>= 4;
		if (dec == 0) {
			break;
		}
	}
  for (i = 0; i < fill-size; ++i) {
    hex[i] = '0';
  }
  hex[fill] = '\0';
  return size;
}

int findEOL(uint8_t* data, int size) {
	for (int i = 0; i < size-1; ++i) {
		if (ReverseByte(data[i]) == 13 && ReverseByte(data[i+1]) == 10) {
			return i;
		}
	}
	return -1;
}

int get_uart_line(uint8_t* data, int size) {
	int line_size = findEOL(uartBuffer, uartBufferIndex);
	if (line_size == -1) {
		return -1;
	}
	int min_size = (size<line_size)?size:line_size;
	for (int i = 0; i < min_size; ++i) {
		data[i] = ReverseByte(uartBuffer[i]);
	}
	data[min_size] = '\0';
	return min_size;
}

int wait_get_line(uint8_t* buffer, int size) {
	while (uartBufferIndex <= 0);
	int bytes = 0;
	while (1) {
		bytes = get_uart_line(buffer, size);
		if (bytes > 0) {
			return bytes;
		}
	}
}
int get_internal_temp() {
	uint8_t at_get_temp[] = {0x41, 0x54, 0x24, 0x54, 0x3f, 0xd};
	uint8_t buff[5];
	sendUART(at_get_temp, 6);
	wait_get_line(buff, 5);
	clearUARTBuffer();
	long t = strtol((const char*)buff, NULL, 10);
	if (t == LONG_MIN && t == LONG_MAX) {
		return -1;
	}
	return (int)t;
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
	/* uint8_t buff[UART_BUFFER_SIZE]; */
	for (;;) {
		wdt_sleep();
		if (!(PINB & 1<<PB1)) {
			blink(3, 100);
			int t = get_internal_temp();
			sleep_delay_ms(1000);
			if (t >= 200 && t <= 400) {
				char at_send[16] = "AT$SF=";
				int size = dechex(t, at_send+6, 10, 8);
				blink(2, 100);
				sendUART(at_send, 15);
				char buff[5];
				int bytes = wait_get_line(buff, 5);
				if (bytes == 2 && buff[0] == 0x4f && buff[1] == 0x4b) {
					blink(4, 200);
				} else {
					blink(3, 200);
				}
			} else if (t == -1) {
				blink(2, 200);
			}
		}
	}

	return 0;
}
