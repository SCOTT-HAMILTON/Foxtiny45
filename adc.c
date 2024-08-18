#include "adc.h"
#include "sleep.h"

#include <avr/interrupt.h>

static volatile uint8_t adc_val_converted = 0;
static volatile uint16_t adc_val = 0;

ISR(ADC_vect) {
  uint8_t adc_lobyte;
  uint16_t raw_adc;
	adc_lobyte = ADCL; // get the sample value from ADCL
	raw_adc = ADCH<<8 | adc_lobyte;   // add lobyte and hibyte
	adc_val = raw_adc;	
	adc_val_converted = 1;
}

void init_temp_adc() {
	ADMUX = 1<<REFS1 | 3; // for 1.1v internal Vref and ADC3 (PB3)
	ADCSRA = 1<<ADEN |
		1<<ADIE |
		1<<ADPS2 | 1<<ADPS1;
		/* 1<<ADIF | */
		/* 1<<ADATE | */
	ADCSRB = 0;
}

void init_batt_volt_adc() {
	ADMUX = 1; // for Vcc as Vref and ADC1 (PB2)
	ADCSRA = 1<<ADEN |
		1<<ADIE |
		1<<ADPS2 | 1<<ADPS1; // for ADC interrupt and x64 clock prescaler
	ADCSRB = 0;
}

uint16_t get_external_temp() {
	unsigned long tot = 0;
	init_temp_adc();
	for (int i = 0; i < 10000; ++i) {
		adc_val_converted = 0;
		adc_val = 0;
		/* ADCSRB = 0; */
		ADCSRA |=	1<<ADSC;
		/* while (ADCSRA & (1 << ADSC) ); */
		adc_noise_reduc_sleep();
		while (!adc_val_converted);
		tot += adc_val;
	}
	ADCSRA &= ~(1<<ADEN);
	return (uint16_t)(tot/9309);
}

uint16_t get_batt_voltage() {
	unsigned long tot = 0;
	init_batt_volt_adc();
	for (int i = 0; i < 10000; ++i) {
		adc_val_converted = 0;
		adc_val = 0;
		/* ADCSRB = 0; */
		ADCSRA |=	1<<ADSC;
		/* while (ADCSRA & (1 << ADSC) ); */
		adc_noise_reduc_sleep();
		while (!adc_val_converted);
		tot += adc_val;
	}
	ADCSRA &= ~(1<<ADEN);
	return (uint16_t)(tot/40000);
}
