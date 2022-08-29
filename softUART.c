#include <avr/io.h>
#define F_CPU    8000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdlib.h>

#include "rxUART.h"
#include "utilsUART.h"
#include "debugging.h"
#include "wdtSleep.h"

#define DONTDEBUG 1

void wakeup_uart() {
	uint8_t buf[] = {13, 13, 13};
	sendUART(buf, 3);
	sleep_delay_ms(2000);
	clearUARTBuffer();
}

int sleep_uart() {
	uint8_t buf[] = {65, 84, 36, 80, 61, 49, 13};
	sendUART(buf, 7);
	char recv_buff[5];
	int bytes = wait_get_line((uint8_t*)recv_buff, 5);
	if (bytes == -1) {
#ifndef DONTDEBUG
		blink(6, 300);
#endif
		clearUARTBuffer();
		return -1;
	} else if (bytes == -2) {
#ifndef DONTDEBUG
		blink(5, 300);
		debugSquareBuffer();
#endif
		clearUARTBuffer();
		return -1;
	}
	if (bytes != 2) {
		if (uartBufferIndex == 20 &&
				ReverseByte(uartBuffer[0]) == 69 &&
				ReverseByte(uartBuffer[1]) == 82) {
			// received ERROR: parse error
			// you can try to restart
			clearUARTBuffer();
			return -2;
		} else {
#ifndef DONTDEBUG
			blink(4, 300);
			sleep_delay_ms(2000);
			blink(bytes, 300);
			sleep_delay_ms(2000);
			debugSquareBuffer();
#endif
			clearUARTBuffer();
			return -1;
		}
	}
	if (recv_buff[0] != 79) {
#ifndef DONTDEBUG
		blink(3, 300);
		debugSquareBuffer();
#endif
		clearUARTBuffer();
		return -1;
	}
	if (recv_buff[1] != 75) {
#ifndef DONTDEBUG
		blink(2, 300);
		debugSquareBuffer();
#endif
		clearUARTBuffer();
		return -1;
	}
#ifndef DONTDEBUG
	blink(7, 300);
#endif
	sleep_delay_ms(2000);
	clearUARTBuffer();
	return 0;
}

void retry_sleep_uart() {
	for (int i = 0; i < 3; ++i) {
		if (sleep_uart() != -2) {
			break;
		}
		blink(3, 100);
	}
}

int main(void) {
	setupUART();
	setup_watchdog(8);
	// OUTPUT PB4 Low
	DDRB |= (1<<PB4);
	PORTB &= ~(1<<PB4);

	// OUTPUT PB1 HIGH
	DDRB |= 1<<PB1;
	PORTB |= 1<<PB1;
	/* uint8_t buff[UART_BUFFER_SIZE]; */
	int last_send_counter_s = 0;
	wakeup_uart();
	retry_sleep_uart();
	for (;;) {
		wdt_sleep();
		uint8_t buff[32];
		int s = uartBufferIndex;
		for (int i = 0; i < s; ++i) {
			buff[i] = ReverseByte(uartBuffer[i]);
		}
		buff[s] = '\r';
		sendUART(buff, s+1);
		clearUARTBuffer();
		sleep_delay_ms(1000);
		/* last_send_counter_s += 4; */
		/* if (last_send_counter_s >= 1096) { */
		/* 	last_send_counter_s = 0; */
		/* 	wakeup_uart(); */
		/* 	send_internal_temp(); */
		/* 	retry_sleep_uart(); */
		/* } */
	}

	return 0;
}
