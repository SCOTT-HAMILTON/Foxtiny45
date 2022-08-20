#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>

void setup_watchdog(int ii);
void wdt_sleep();
