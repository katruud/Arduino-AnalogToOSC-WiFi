#include <WiFiNINA.h>
#include <OSCMessage.h>
#include <OSCData.h>
#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab/arduino_secrets.h

// Constants:
const bool debug = false;           // Enable for debug serial over USB
const int calhigh = 330;            // Low range of detection (bent)
const int callow = 500;             // high range of detection (straight)
const int flexPin = A0;             // pin A0 to read analog input
const int ledPin = LED_BUILTIN;     // PWM funtion LED
int status = WL_IDLE_STATUS;        // the WiFi radio's status
IPAddress outIp(192, 168, 0, 203);  // OSC client IP
const unsigned int outPort = 9000;  // local port to listen on
int flexValue;
float flexFloat;
WiFiUDP Udp;

void setup() {
  // put your setup code here, to run once:
  Udp.begin(8080);

  // Debug LED
  pinMode(ledPin, OUTPUT);

  // Initialize serial and wait for port to open:
  if (debug) {
    Serial.begin(9600);
    while (!Serial)
      ;
    // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    if (debug) {
      Serial.println("Communication with WiFi module failed!");
    }
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION && debug) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    if (debug) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(ssid);
    }
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  if (debug) {
    Serial.println("You're connected to the network");

    // print your WiFi's IP address:
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  // polling the sensor for new measure

  flexValue = analogRead(flexPin);
  flexValue = map(flexValue, callow, calhigh, 0, 255);
  flexValue = constrain(flexValue, 0, 255);
  flexFloat = flexValue / 255.0;
  analogWrite(ledPin, flexValue);  // Write to debug LED

  if (debug) {
    Serial.println(flexValue);    // Print val for debugging
    // Serial.println(flexFloat); // Print float val for debugging
  }

  // the message wants an OSC address as first argument
  OSCMessage msg("/avatar/parameters/FootLeftFloat");
  msg.add(flexFloat);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);    // send the bytes to the SLIP strea
  Udp.endPacket();  // mark the end of the OSC Packet
  msg.empty();      // free space occupied by message

  /*
  OSCMessage msg2("/avatar/parameters/FootRightFloat");
  msg2.add(flexFloat);
  Udp.beginPacket(outIp, outPort);
  msg2.send(Udp);   // send the bytes to the SLIP strea
  Udp.endPacket();  // mark the end of the OSC Packet
  msg2.empty();  // free space occupied by message
*/

  delay(200);
}