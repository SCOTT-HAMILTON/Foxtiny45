#pragma once

#include <avr/io.h>

uint16_t get_external_temp();
void init_temp_adc();

uint16_t get_batt_voltage();
void init_batt_volt_adc();
