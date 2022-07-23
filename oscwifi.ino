#include <WiFiNINA.h>
#include <OSCMessage.h>
#include <OSCData.h>

// WIFI
#include "arduino_secrets.h"
//please enter your sensitive data in the Secret tab/arduino_secrets.h

int status = WL_IDLE_STATUS;  // the WiFi radio's status

//Constants:
int flexValue;
float flexFloat;
const int ledPin = LED_BUILTIN;  //pin 3 has PWM funtion
const int flexPin = A0;          //pin A0 to read analog input
const int calhigh = 270;         // Low range of detection (bent)
const int callow = 600;          // high range of detection (straight)

WiFiUDP Udp;

//destination IP
IPAddress outIp(192, 168, 0, 203);

const unsigned int outPort = 9000;  // local port to listen on

int count = 0;

void setup() {
  // put your setup code here, to run once:
  Udp.begin(8080);

  // Debug LED
  pinMode(ledPin, OUTPUT);

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial)
    ;
  // wait for serial port to connect. Needed for native USB port only
  // }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");

  // print your WiFi's IP address:
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  // polling the sensor for new measure

  flexValue = analogRead(flexPin);
  flexValue = map(flexValue, callow, calhigh, 0, 255);
  flexValue = constrain(flexValue, 0, 255);
  flexFloat = flexValue / 255.0;
  // Serial.println(flexFloat); Print val for debugging
  analogWrite(ledPin, flexValue);

  //the message wants an OSC address as first argument
  OSCMessage msg("/avatar/parameters/FootLeftFloat");
  msg.add(flexFloat);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);    // send the bytes to the SLIP strea
  Udp.endPacket();  // mark the end of the OSC Packet
  msg.empty();  // free space occupied by message

/*
  OSCMessage msg2("/avatar/parameters/FootRightFloat");
  msg2.add(flexFloat);
  Udp.beginPacket(outIp, outPort);
  msg2.send(Udp);   // send the bytes to the SLIP strea
  Udp.endPacket();  // mark the end of the OSC Packet
  msg2.empty();  // free space occupied by message
*/

  delay(50);
}