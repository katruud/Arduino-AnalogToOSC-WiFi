# Arduino-AnalogToOSC-WiFi
Send OSC values over WiFi based on an analog Arduino input

Developed for an Arduino Nano RP2040 with an adafruit [Short Flex Sensor](https://www.adafruit.com/product/1070). 

## Setup
Create a voltage divider circuit with the flex sensor. In my testing, I used a 44k Ohm resistor. Power can be provided to the circuit from the built-in 5v pin.

Set your SSID name and password in the arduino_secrets.h file.

Depending on your application, the variables calhigh and callow will need to be set to capture the complete range of motion. You may uncomment the serial print lines for debugging your specific values.

You must also set the destination IP variable to match that of your OSC destination. 

The delay function may be adjusted to modify update rate. It is currently set to 20 Hz

## Package Dependencies 
- [WiFiNINA library for Arduino](https://github.com/arduino-libraries/WiFiNINA)
- [OSC for Arduino](https://github.com/CNMAT/OSC)

## References
- [Wiring an analog flex sensor to Nano](https://www.instructables.com/Flex-Sensors-With-Arduino/)
- [Nano OSC over Wifi example](https://forum.arduino.cc/t/arduino-nano-iot-keeps-breaking-connection-with-wifi-module-failed/972559)
