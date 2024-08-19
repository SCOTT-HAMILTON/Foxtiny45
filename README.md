<h1 align="center">Foxtiny45</h1>

<p align="center">
      <a href="https://scott-hamilton.mit-license.org/"><img alt="MIT License" src="https://img.shields.io/badge/License-MIT-525252.svg?labelColor=292929&logo=creative%20commons&style=for-the-badge" /></a>
</p>

Attiny45-powered temperature sensor communicating with the Sigfox network. It is using the BRKWS01 board from the [SNOC](https://snoc.fr/) to transmit temperature data to the network which allows it to be installed anywhere as long as it stays in Sigfox's [coverage map](https://www.sigfox.com/coverage/).

## Requirements
 - RaspberryPi/Arduino/Attiny Programmer
 - A Sigfox account
 - gcc-avr
 - avr-libc
 - avrdude
 - make
 - wiringPi (for gpio program)

## Pinout of the attiny45
 - PB4 for the status led
 - PB1 for UART TX (to BRKWS01 RX)
 - PB0 for UART RX (to BRKWS01 TX)
 - PB2 for battery voltage (reduced to 3.3v max with high impedance voltage dividor)

## Help
This is just a little project, but feel free to fork, change, extend or correct the code.

## License
Foxtiny45 is delivered as it is under the well known MIT License
