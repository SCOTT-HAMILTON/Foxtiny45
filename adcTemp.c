#include "adcTemp.h"
#include "sleep.h"

#include <avr/interrupt.h>

static volatile uint8_t adc_temp_converted = 0;
static volatile uint16_t adc_temp = 0;

ISR(ADC_vect) {
  uint8_t adc_lobyte;
  uint16_t raw_adc;
	adc_lobyte = ADCL; // get the sample value from ADCL
	raw_adc = ADCH<<8 | adc_lobyte;   // add lobyte and hibyte
	adc_temp = raw_adc;	
	adc_temp_converted = 1;
}

void init_adc() {
	ADMUX = 1<<REFS1 | 3; // for 1.1v internal Vref and ADC3
	ADCSRA = 1<<ADEN |
		1<<ADIE |
		1<<ADPS2 | 1<<ADPS1;
		/* 1<<ADIF | */
		/* 1<<ADATE | */
	ADCSRB = 0;
}

uint16_t get_external_temp() {
	unsigned long tot = 0;
	for (int i = 0; i < 10000; ++i) {
		adc_temp_converted = 0;
		adc_temp = 0;
		/* ADCSRB = 0; */
		ADCSRA |=	1<<ADSC;
		/* while (ADCSRA & (1 << ADSC) ); */
		adc_noise_reduc_sleep();
		while (!adc_temp_converted);
		tot += adc_temp;
	}
	return (uint16_t)(tot/9309);
}
