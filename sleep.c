#include "sleep.h"

#include <avr/sleep.h>

void setup_watchdog(int ii) {
  uint8_t bb;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= 1<<WDIE;
}

void wdt_sleep() {
	uint8_t oldMCUCR = MCUCR;
	uint8_t oldDDRB = DDRB;
	/* DDRB = 0; */
	ADCSRA &= ~(1<<ADEN);
	/* PRR |= 15; */
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	MCUCR |=
		1<<SM1 |
		1<<BODS | 1<<BODSE;
	MCUCR &=
		~(1<<BODSE) &
		~(1<<SM0);
	
	sleep_mode();

	ADCSRA |= 1<<ADEN;
	/* PRR &= ~(15); */
	MCUCR = oldMCUCR;
	DDRB = oldDDRB;
}

void adc_noise_reduc_sleep() {
	uint8_t oldMCUCR = MCUCR;
	set_sleep_mode(SLEEP_MODE_ADC);

	MCUCR |=
		1<<SM0 |
		1<<BODS | 1<<BODSE;
	MCUCR &=
		~(1<<BODSE) &
		~(1<<SM1);
	
	sleep_mode();

	MCUCR = oldMCUCR;
}

// Mandatory
ISR(WDT_vect) {}
