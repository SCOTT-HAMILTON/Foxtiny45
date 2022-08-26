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

void wakeup_uart() {
	uint8_t buf[] = {65, 84, 13, 10};
	sendUART(buf, 4);
	sleep_delay_ms(2000);
	clearUARTBuffer();
}

int sleep_uart() {
	uint8_t buf[] = {65, 84, 36, 80, 61, 49, 13};
	sendUART(buf, 7);
	char recv_buff[5];
	int bytes = wait_get_line((uint8_t*)recv_buff, 5);
	if (bytes == -1) {
		blink(6, 300);
		return -1;
	} else if (bytes == -2) {
		blink(5, 300);
		debugSquareBuffer();
		return -1;
	}
	if (bytes != 2) {
		blink(4, 300);
		sleep_delay_ms(2000);
		blink(bytes, 300);
		sleep_delay_ms(2000);
		debugSquareBuffer();
		return -1;
	}
	if (recv_buff[0] != 79) {
		blink(3, 300);
		debugSquareBuffer();
		return -1;
	}
	if (recv_buff[1] != 75) {
		blink(2, 300);
		debugSquareBuffer();
		return -1;
	}
	blink(7, 300);
	sleep_delay_ms(2000);
	clearUARTBuffer();
	return 0;
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
	int last_send_counter_s = 0;
	/* wakeup_uart(); */
	/* sleep_delay_ms(2000); */
	/* clearUARTBuffer(); */
	/* sleep_uart(); */
	/* sleep_delay_ms(2000); */
	/* clearUARTBuffer(); */
	for (;;) {
		wdt_sleep();
		last_send_counter_s += 4;
		if (last_send_counter_s >= 1096) {
			last_send_counter_s = 0;
			wakeup_uart();
			send_internal_temp();
			sleep_uart();
		}
		if (!(PINB & 1<<PB1)) {
			blink(2, 100);
			wakeup_uart();
			sleep_uart();
		}
	}

	return 0;
}
