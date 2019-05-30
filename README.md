[![license-badge][]][license] ![version] [![stars][]][stargazers] [![hit-count][]][count] [![github-issues][]][issues]

# MAX6675
This is an Arduino library for 12-bit MAX6675 K-thermocouple to digital converter with cold junction compensation & maximum sampling rate about 4Hz. Supports all sensor features. Can work with hardware or software emulated 3-wire SPI.

- Maximum power supply voltage is 5.5v
- K-type thermocouples have an absolute accuracy of around ±2°C
- Measurement tempereture range 0°C...+1024°C* with 0.25°C resolution
- Cold junction compensation range -20°C...+85°C
- Keep K-type thermocouple cold junction & MAX6675 at the same temperature
- Avoid placing heat-generating devices or components near the converter
  because this may produce errors
- It is *strongly* recommended to add a 10nF/0.01mF ceramic surface-mount capacitor, placed across
  the T+ and T- pins, to filter noise on the thermocouple lines.
  
*Library returns 2000, if thermocouple shorted or a communication error has occurred

Supports:

- Arduino AVR
- Arduino ESP8266
- Arduino ESP32
- Arduino STM32

[license-badge]: https://img.shields.io/badge/License-GPLv3-blue.svg
[license]:       https://choosealicense.com/licenses/gpl-3.0/
[version]:       https://img.shields.io/badge/Version-1.2.0-green.svg
[stars]:         https://img.shields.io/github/stars/enjoyneering/MAX6675.svg
[stargazers]:    https://github.com/enjoyneering/MAX6675/stargazers
[hit-count]:     http://hits.dwyl.io/enjoyneering/MAX6675.svg
[count]:         http://hits.dwyl.io/enjoyneering/MAX6675/badges
[github-issues]: https://img.shields.io/github/issues/enjoyneering/MAX6675.svg
[issues]:        https://github.com/enjoyneering/MAX6675/issues/
