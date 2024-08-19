<h1 align="center">Foxtiny45</h1>

<p align="center">
      <a href="https://scott-hamilton.mit-license.org/"><img alt="MIT License" src="https://img.shields.io/badge/License-MIT-525252.svg?labelColor=292929&logo=creative%20commons&style=for-the-badge" /></a>
</p>

Attiny45-powered temperature sensor communicating with the Sigfox network. It is using the BRKWS01 board from the [SNOC](https://snoc.fr/) to transmit temperature data to the network which allows it to be installed anywhere as long as it stays in Sigfox's [coverage map](https://www.sigfox.com/coverage/). The leverage of sleep capabilities of both the BRKWS01 and the Attiny45 allows for a reduced power consumption.

## Requirements
 - raspberry pi
 - Attiny45 or 85 (the code is more than 3k so a 25 won't cut it)
 - Sigfox account
 - gcc-avr (on the Pi)
 - avr-libc (on the Pi)
 - avrdude [from kcuzner](https://github.com/kcuzner/avrdude) (on the Pi)
 - make (on the Pi)
 - wiringPi (for gpio program) (on the Pi)
 - [BRKWS01 by the SNOC](https://partners.sigfox.com/products/sigfox-breakout-board-brkws01)
 - 3.3V linear temperature sensor (you may have to adapt the final gain in `get_external_temp` function from `adc.c`)
 - high impedance potentiometer (100kΩ or more to reduce battery consumption).

For avrdude and wiringPi, you can follow instructions [here](https://www.instructables.com/Programming-the-ATtiny85-from-Raspberry-Pi/) by prb3333 to install them on the Pi.

## Pinout of the attiny45
 - PB4 for the status led
 - PB1 for UART TX (to BRKWS01 RX)
 - PB0 for UART RX (to BRKWS01 TX)
 - PB2 for battery voltage (reduced to 3.3v max with high impedance potentiometer as voltage divider)

## Preview
![Sigfox messages](https://github.com/user-attachments/assets/ffaff356-9263-4c5c-9da6-ca9d5f861cb5)

## Payload
The payload sent to sigfox is XXYYYZZZ in HEX with,
 - XX: the byte in HEX for the battery voltage (00->0V at PB2, FF->3.3V at PB2), knowing the position of your potentiometer (use a multimeter and apply 15V to the potentiometer), you can then calculate the corresponding true voltage for the battery (simple cross-multiplication)
 - YYY: the 12 bits (only 10 are used because the Attiny45 has a 10 bit ADC) for the external temperature in celsius times 10. For e.g. 11F -> 287 -> 28.7°C
 - ZZZ: same but for the internal temperature

## Bonus
 - [Sigfox-S3-Data-Backup](https://github.com/SCOTT-HAMILTON/Sigfox-S3-Data-Backup) to automatically backup your sent payloads to an S3 cloud (filebase provides up to 5G of free S3 buckets which is about a lifetime of payload storage at 20 messages/day every single day of the year)
 - [balena-fox-backup](https://github.com/SCOTT-HAMILTON/balena-fox-backup) to run automatically the `Sigfox-S3-Data-Backup` on your balenacloud device (for e.g. a raspberry pi)
 - [fox-show-temps](https://github.com/SCOTT-HAMILTON/fox-show-temps) to plot all of your backed up data (internal and external temperature + battery voltage) on a single page

## Help
This is just a little project, but feel free to fork, change, extend or correct the code.

## License
Foxtiny45 is delivered as it is under the well known MIT License

**References that helped**
 - [Attiny45 datasheet] : <https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf>
 - [Rpi GPIO pinout] : <https://pinout.xyz/>
 - [UART protocol] : <https://docs.arduino.cc/learn/communication/uart/>
 - [BRKWS01 specsheet] : <https://storage.googleapis.com/public-assets-xd-sigfox-production-338901379285/ixzyw4f0_brkws01-specsheet.pdf>
 - [BRKWS01 full AT commands doc] : <https://yadom.fr/media/product-attachment//home/magento///File-1572880878.pdf>

[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. There is no need to format nicely because it shouldn't be seen. Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)

   [android documentation]: <https://developer.android.com/>
   [Attiny45 datasheet]: <https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf>
   [Rpi GPIO pinout]: <https://pinout.xyz/>
   [UART protocol]: <https://docs.arduino.cc/learn/communication/uart/>
   [BRKWS01 specsheet]: <https://storage.googleapis.com/public-assets-xd-sigfox-production-338901379285/ixzyw4f0_brkws01-specsheet.pdf>
   [BRKWS01 full AT commands doc]: <https://yadom.fr/media/product-attachment//home/magento///File-1572880878.pdf>
