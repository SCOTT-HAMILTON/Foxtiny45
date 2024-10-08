#include "utilsUART.h"
#include "rxUART.h"
#include "debugging.h"
#include "stringUtils.h"
#include "adc.h"

#include <stdlib.h>
#include <limits.h>

uint8_t ReverseByte (uint8_t x) {
    x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
    x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
    x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
    return x;
}

int findEOL(volatile uint8_t* data, int size) {
	for (int i = 0; i < size; ++i) {
		uint8_t b = ReverseByte(data[i]);
		if (b == 13 || b == 10) {
			return i;
		}
	}
	return -1;
}

int get_uart_line(uint8_t* data, int size) {
	int line_size = findEOL(uartBuffer, uartBufferIndex);
	if (line_size == -1) {
		return -1;
	}
	int min_size = (size<line_size)?size:line_size;
	for (int i = 0; i < min_size; ++i) {
		data[i] = ReverseByte(uartBuffer[i]);
	}
	data[min_size] = '\0';
	return min_size;
}

int wait_get_line(uint8_t* buffer, int size) {
	clearUARTBuffer();
	for (int i = 0; i < 2000; ++i) {
		if (uartBufferIndex > 0)break;
    sleep_delay_ms(1);
	}
	if (uartBufferIndex <= 0)return -1;

	int bytes = 0;
	for (int i = 0; i < 2000; ++i) {
		bytes = get_uart_line(buffer, size);
		if (bytes > 0) {
			return bytes;
		}
    sleep_delay_ms(1);
	}
  return -2;
}

int get_internal_temp() {
	uint8_t at_get_temp[] = {0x41, 0x54, 0x24, 0x54, 0x3f, 0xd, 10};
	uint8_t buff[5];
	sendUART(at_get_temp, 7);
	wait_get_line(buff, 5);
	clearUARTBuffer();
	long t = strtol((const char*)buff, NULL, 10);
	if (t == LONG_MIN && t == LONG_MAX) {
		return -1;
	}
	return (int)t;
}

void send_temps() {
	blink(3, 200);
	int tint = get_internal_temp();
	int text = get_external_temp();
	uint16_t volt = get_batt_voltage();

	if (tint < 200 || tint > 400) {
		blink(5, 200);
	} else if (tint == -1) {
		tint = 0;
		blink(4, 200);
	}
	char at_send[17] = "AT$SF=";
	dec2hex(tint, at_send + 6, 8);
	char b = at_send[11];
	dec2hex(text, at_send + 6, 5);
	at_send[11] = b;
	b = at_send[8];
	dec2hex(volt, at_send + 6, 2);
	at_send[8] = b;
	at_send[14] = '\r';
	at_send[15] = '\n';
	at_send[16] = '\0';
	sendUART((uint8_t*)at_send, 16);
	char buff[5];
	int bytes = wait_get_line((uint8_t*)buff, 5);
	if (bytes == 2 && buff[0] == 0x4f && buff[1] == 0x4b) {
		blink(3, 200);
	} else {
		blink(2, 200);
	}
	clearUARTBuffer();
}
