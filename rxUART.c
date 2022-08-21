#include "rxUART.h"

#include <avr/interrupt.h>

volatile uint8_t usiReceived = 0;
volatile uint8_t usiByteReceived = 0;
volatile uint8_t uartBuffer[UART_BUFFER_SIZE] = {'\0'};
volatile uint8_t uartBufferIndex = 0;

// Old timer values
static volatile uint8_t oldTCCR0A;
static volatile uint8_t oldTCCR0B;
static volatile uint8_t oldTCNT0;

void setupUART() {
	// Save old timer values
	oldTCCR0B = TCCR0B;
	oldTCCR0A = TCCR0A;
	// Disable USI
	USICR = 0;

	// PCIE not INT0 because we want to get interrupt
	// even when sleeping
	// Enable interrupt on PB1 change
	PCMSK |= 1<<PCINT0;
	GIMSK |= 1<<PCIE;
	
	// INPUT_PULLUP on DB0
	DDRB &= ~(1<<DDB0);
	PORTB |= 1 << PB0;
	sei();
}

void clearUARTBuffer() {
	uartBufferIndex = 0;
	uartBuffer[0] = '\0';
}

void startUARTReceive() {
	// Save old timer counter
	oldTCNT0 = TCNT0;
	// Didn't receive anything yet
	usiReceived = 0;

	// Disable pin change interrupts
	GIMSK &= ~(1<<PCIE);
	
	// Timer/Counter0 CTC 8x prescaler
	TCCR0A = 2<<WGM00;
	TCCR0B = 1<<CS01;
	// Reset prescaler
	GTCCR |= 1<<PSR0;
	// Reset counter0
	TCNT0 = 0;

	// Number of bits to count
	OCR0A = 48;

	// Clear output compare interrupt flag
  TIFR |= 1 << OCF0A;
	// Enable output compare interrupt
	TIMSK |= 1<<OCIE0A;
	sei();
}

ISR (PCINT0_vect) {
  if (!(PINB & 1<<PINB0)) {
		startUARTReceive();
  }
}

ISR (TIMER0_COMPA_vect) {
	// Disable COMPA interrupt
	TIMSK &= ~(1<<OCIE0A);
	// Reset counter0
	TCNT0 = 0;
	// Shift every bit width this time
  OCR0A = 104;

	// Configure USI to use Timer/Counter0's CTC as clock source
	// , disable any wiremode and enable overflow interrupt
	USICR |=
		1<<USICS0 |
		0<<USIWM1 | 0<<USIWM0|
		1<<USIOIE;
	// Clear the overflow flag and start counting at 8,
	// so only 8 bit are being read instead of 16
	USISR = 1<<USIOIF | 8;
}

ISR (USI_OVF_vect) {
  usiByteReceived = USIBR;
	if (uartBufferIndex >= UART_BUFFER_SIZE-1) {
		uartBuffer[0] = USIBR;
		uartBuffer[1] = '\0';
		uartBufferIndex = 1;
	} else {
		uartBuffer[uartBufferIndex] = USIBR;
		uartBuffer[uartBufferIndex+1] = '\0';
		uartBufferIndex++;
	}
  USICR = 0;
	usiReceived = 1;

	// Restore old timer values
	TCCR0A = oldTCCR0A;
	TCCR0B = oldTCCR0B;
	// Note Arduino millis() and micros() will loose the time it took us to receive a byte
	// Approximately 1ms at 9600 baud
	TCNT0 = oldTCNT0;

	// Clear pin change interrupt flag
	GIFR = 1<<PCIF;
	// Enable pin change interrupts again
  GIMSK |= 1<<PCIE;
	sei();
}
