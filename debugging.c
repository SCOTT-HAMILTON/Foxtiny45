#include "debugging.h"

#include <avr/interrupt.h>

#include "rxUART.h"
#include "utilsUART.h"

#ifndef NULL
#define NULL 0
#endif

static volatile enum Timer1Mode timer1Mode = COUNT;

// time counter for sleep_delay_ms function
static volatile unsigned long timer1CounterMs;
static volatile uint8_t timer1SquareLevel = 0;

static volatile unsigned long txUARTByteIndex = 0;
static volatile uint8_t txUARTByteBit = 0;
static volatile uint8_t* txUARTData = NULL;
static volatile unsigned long txUARTDataSize = 0;

void sleep_delay_ms(unsigned long ms) {
	timer1Mode = COUNT;
	timer1CounterMs = 0;
	// Timer/Counter1 CTC with prescaler 64, use OCR1C
	TCCR1 = 
		1<<CTC1 | 7<<CS10;
	// Reset prescaler
	GTCCR |= 1<<PSR1;
	// Reset counter0
	TCNT1 = 0;

	// To pulse every milliseconds
	OCR1C = 125;

	// Clear output compare interrupt flag
  TIFR |= 1 << OCF1A;
	// Enable output compare interrupt
	TIMSK |= 1<<OCIE1A;
	sei();

	for(;;) {
		if (timer1CounterMs >= ms) {
			// Disable Timer/Counter1
			TIMSK &= ~(1<<OCIE1A);
			return;
		}
	}
}

void set_square(uint8_t us) {
	timer1Mode = SQUARE;
	timer1SquareLevel = 1;
	// Timer/Counter1 CTC with prescaler 8, use OCR1C
	TCCR1 = 
		1<<CTC1 | 4<<CS10;
	// Reset prescaler
	GTCCR |= 1<<PSR1;
	// Reset counter0
	TCNT1 = 0;

	OCR1C = us;

	// Clear output compare interrupt flag
  TIFR |= 1 << OCF1A;
	// Enable output compare interrupt
	TIMSK |= 1<<OCIE1A;
	sei();
}

void blink(uint8_t n, int delay) {
	for (uint8_t i = 0; i < n; ++i) {
		PORTB |= (1<<PB4);
		sleep_delay_ms(delay);
		PORTB &= ~(1<<PB4);
		sleep_delay_ms(delay);
	}
}

void wait_click() {
	/* while (PINB & 1<<PB1); */
	/* sleep_delay_ms(1000); */
	/* while (!(PINB & 1<<PB1)); */
}

void debugSquareBuffer() {
	wait_click();
	int s = uartBufferIndex;
	if (s > 0) {
		blink(s, 300);
		for (int i = 0; i < s; ++i) {
			set_square(ReverseByte(uartBuffer[i]));
			wait_click();
		}
	}
	blink(2, 200);
}

void sendUART(uint8_t* data, unsigned int size) {
	timer1Mode = TX_UART;
	txUARTByteIndex = 0;
	txUARTByteBit = 0;
	txUARTDataSize = size;
	txUARTData = data;
	TIMSK &= ~(1<<OCIE1A);
	// Timer/Counter1 CTC with prescaler 8, use OCR1C
	TCCR1 = 
		1<<CTC1 | 4<<CS10;
	// Reset prescaler
	GTCCR |= 1<<PSR1;
	// Reset counter0
	TCNT1 = 0;

	OCR1C = 92;

	// Clear output compare interrupt flag
  TIFR |= 1 << OCF1A;

	PORTB &= ~(1<<PB1);
	// Enable output compare interrupt
	TIMSK |= 1<<OCIE1A;
	sei();
	// wait for transmission to end
	for(;;) {
		if (txUARTByteBit >= 9+TX_STOP_BITS) {
			// Disable Timer/Counter1
			TIMSK &= ~(1<<OCIE1A);
			PORTB |= 1<<PB1;
			++txUARTByteIndex;
			if (txUARTByteIndex >= txUARTDataSize) {
				return;
			} else {
				txUARTByteBit = 0;
				GTCCR |= 1<<PSR1;
				TCNT1 = 0;
				TIFR |= 1 << OCF1A;
				PORTB &= ~(1<<PB1);
				TIMSK |= 1<<OCIE1A;
				sei();
			}
		}
	}
}

ISR (TIMER1_COMPA_vect) {
	if (timer1Mode == TX_UART) {
		OCR1C = 104;
		uint8_t byte = txUARTData[txUARTByteIndex];
		if ((byte >> txUARTByteBit) & 1 || txUARTByteBit >= 8) {
			PORTB |= 1<<PB1;
		} else {
			PORTB &= ~(1<<PB1);
		}
		++txUARTByteBit;
	} else if (timer1Mode == SQUARE) {
		if (timer1SquareLevel) {
			PORTB |= 1<<PB4;
		} else {
			PORTB &= ~(1<<PB4);
		}
		timer1SquareLevel = 1-timer1SquareLevel;
	} else if (timer1Mode == COUNT) {
		timer1CounterMs++;
	}
}
