#include <WiFiNINA.h>
#include <OSCMessage.h>
#include <OSCData.h>
#include <math.h>
#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab/arduino_secrets.h

// Constants:
const bool debug = false;           // Enable for debug serial over USB
const bool left = true;             // Left foot = true, else false = right foot
const bool Limit = true;            // Don't send repeat vals
const int flexPin = A0;             // pin A0 to read analog input
const int ledPin = LED_BUILTIN;     // PWM funtion LED
int attempt = 0;              // WiFi connection attempt tracker
int calhigh;                  // Low range of detection (bent) 330
int callow;                   // high range of detection (straight) 500
int lowcut;                   // Autocal low
int highcut;                  // Autocal high
int range;                    // Range of foot cal
int status = WL_IDLE_STATUS;  // the WiFi radio's statusip
int flexValue;
float flexFloat;
int prevValue;
WiFiUDP Udp;
IPAddress outIp(192, 168, 0, 203);   // OSC client IP
unsigned int outPort;                // local port to listen on
const unsigned int leftPort = 9013;  // local port to listen on
const unsigned int rightPort = 9011; // local port to listen on

void setup() {
  // put your setup code here, to run once:
  Udp.begin(8080);

  // Debug LED
  pinMode(ledPin, OUTPUT);

  // Set port 
  if (left){
    outPort = leftPort;
    if (debug) {
    Serial.println("Sending left foot signals");
    } 
  } else {
    outPort = rightPort;
    if (debug) {
    Serial.println("Sending right foot signals");
    } 
  }

  // Initialize serial and wait for port to open:
  if (debug) {
    Serial.begin(9600);
    while (!Serial)
      ;
    // wait for serial port to connect. Needed for native USB port only

  }

  // init calibrations
  flexValue = analogRead(flexPin);
  calhigh = highcut = flexValue + flexValue * .01;
  callow = lowcut = flexValue - flexValue * .01;
  range = highcut - lowcut;
  if (debug) {
    Serial.print("Initial calibration center: ");
    Serial.println(flexValue);    // Print val for debugging
    Serial.print("Low: ");
    Serial.print(callow);
    Serial.print("High: ");
    Serial.println(calhigh);      // Print val for debugging
    // Serial.println(flexFloat); // Print float val for debugging
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
    while (attempt < 5) {
      status = WiFi.status();
      delay(500);
      attempt++;
     }
     attempt = 0;
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

  // check connection
  status = WiFi.status();
  if (debug) {
    Serial.println(status);    // Print val for debugging
  }
  
  // reconnect if dropped
  if (status != WL_CONNECTED) {
    if (debug) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(ssid);
    }
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    while (attempt < 5) {
      status = WiFi.status();
      delay(500);
      attempt++;
     }
    attempt = 0;
  }

  flexValue = analogRead(flexPin);
  if (debug) {
    Serial.print("flexvalue: ");
    Serial.println(flexValue);    // Print val for debugging
  }
  
  // automatic calibration
  // set a high and low limit 5% off from max/min seen
  if (flexValue > highcut) {
    highcut = flexValue;
    range = highcut - lowcut;
    calhigh = highcut - range * .05;
    if (debug) {
      Serial.print("New highcal: ");
      Serial.println(calhigh);    // Print val for debugging
    }
  }  else if (flexValue < lowcut){
    lowcut = flexValue;
    range = highcut - lowcut;
    callow = lowcut + range * .05;
    if (debug) {
      Serial.print("New lowcal: ");
      Serial.println(callow);     // Print val for debugging
    }
  }
  


  // send OSC message only if value has changed more than 2%
  if (!(prevValue - range*.02 <= flexValue & flexValue <= prevValue + range*.02 & Limit)) {
      prevValue = flexValue;
      flexValue = map(flexValue, calhigh, callow, 0, 255);
      flexValue = constrain(flexValue, 0, 255);
      flexFloat = flexValue / 255.0;
      flexFloat = (asin(flexFloat*2 - 1) / PI) + .5; // Curve of float 
      analogWrite(ledPin, flexValue);  // Write to debug LED

    // the message wants an OSC address as first argument
    if (left){
      OSCMessage msg("/avatar/parameters/FootLeftFloat");
      msg.add(flexFloat);
      Udp.beginPacket(outIp, outPort);
      msg.send(Udp);    // send the bytes to the SLIP strea
      Udp.endPacket();  // mark the end of the OSC Packet
      msg.empty();      // free space occupied by message
    } else {
      OSCMessage msg("/avatar/parameters/FootRightFloat");
      msg.add(flexFloat);
      Udp.beginPacket(outIp, outPort);
      msg.send(Udp);    // send the bytes to the SLIP strea
      Udp.endPacket();  // mark the end of the OSC Packet
      msg.empty();      // free space occupied by message
    }
  } else {
      if (debug) {
        Serial.println("Value within 1%, not sending");
      }
  }

  if (debug) {
    Serial.print("callow: ");
    Serial.println(callow);    // Print val for debugging
    Serial.print("calhigh: ");
    Serial.println(calhigh);    // Print val for debugging
    Serial.print("floatvalue: ");
    Serial.println(flexFloat);    // Print val for debugging
  }

  // How long to wait before checking again
  delay(20);
}