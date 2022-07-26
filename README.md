# Arduino-AnalogToOSC-WiFi
Send OSC values over WiFi based on an analog Arduino input

Developed for an [Arduino Nano RP2040](https://store-usa.arduino.cc/products/arduino-nano-rp2040-connect-with-headers) with an adafruit [Short Flex Sensor](https://www.adafruit.com/product/1070). 

## Hardware Setup
Create a voltage divider circuit with the flex sensor. In my testing, I used a 44k Ohm resistor. Power can be provided to the circuit from the built-in 5v pin. [This guide](https://www.instructables.com/Flex-Sensors-With-Arduino/) has a handy wiring diagram, though note that A0 is used as the input in the program.

<img src="https://raw.githubusercontent.com/katruud/Arduino-AnalogToOSC-WiFi/main/images/wiring.jpg" width="640" height="480">

Set your SSID name and password in the arduino_secrets.h file.

Depending on your application, the variables calhigh and callow will need to be set to capture the complete range of motion. You may uncomment the serial print lines for debugging your specific values.

You must also set the destination IP variable to match that of your OSC destination. 

The delay function may be adjusted to modify update rate. It is currently set to 20 Hz

For testing, the serial print function may be enabled. This will display the sensor values calculated by the program before they are sent over WiFi.

For testing OSC quickly, [Protokol](https://hexler.net/protokol) is useful. 

## VRChat Setup

The program is built to send an OSC message to VRChat. A float is sent to the FootLeftFloat parameter. This can be used to drive an animation on your avatar, for example to move toe position realistically.

![Shoe Tracking](images/vrchat-example.gif)

## To do

- Troubleshoot performance issues related to WiFi, test bluetooth?
- Develop portable power and mounting solution
- Robustly mount flex sensor to shoe
- Test other/cheaper SOC such as the [Nano 33 IoT](https://store-usa.arduino.cc/products/arduino-nano-33-iot-with-headers)

## Package Dependencies 
- [WiFiNINA library for Arduino](https://github.com/arduino-libraries/WiFiNINA)
- [OSC for Arduino](https://github.com/CNMAT/OSC)

## References
- [Wiring an analog flex sensor to Nano](https://www.instructables.com/Flex-Sensors-With-Arduino/)
- [Nano OSC over Wifi example](https://forum.arduino.cc/t/arduino-nano-iot-keeps-breaking-connection-with-wifi-module-failed/972559)
