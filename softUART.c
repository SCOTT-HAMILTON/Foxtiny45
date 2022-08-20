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

int main(void) {
	setupUART();
	setup_watchdog(8);
	// INPUT_PULLUP on DB1
	DDRB &= ~(1<<DDB1);
	PORTB |= 1 << PB1;
	// OUTPUT PB4 Low
	DDRB |= (1<<PB4);
	PORTB &= ~(1<<PB4);
	for (;;) {
		wdt_sleep();
		set_square(255);
		_delay_ms(500);
		TIMSK &= ~(1<<OCIE1A);
		PORTB &= ~(1<<PB4);
		_delay_ms(500);
		uint8_t expected_msg[] = {
			83, 65, 76, 85, 84, 69, 32, 78, 69, 65, 76
		};
		int expected_msg_size = sizeof(expected_msg);
		int bi = uartBufferIndex;
		if (bi >= expected_msg_size) {
			/* wait_click(); */
			int ok = 1;
			for (int i = 0; i < expected_msg_size; ++i) {
				if (ReverseByte(uartBuffer[i]) != expected_msg[i]) {
					ok = 0;
					break;
				}
			}
			if (ok) {
				blink(4, 200);
			} else {
				blink(2, 200);
			}
			clearUARTBuffer();
		} else {
			blink(bi, 200);
			clearUARTBuffer();
		}
	}

	return 0;
}
