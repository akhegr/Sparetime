// This Arduino sketch reads DS18B20 "1-Wire" digital
// temperature sensors.
// Tutorial:
// http://www.hacktronics.com/Tutorials/arduino-1-wire-tutorial.html

#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 3

    String writeParam = "0";
    String tempValue = "0";
    char buffer[7];
    
/* 
Forbind + og GND portene med modstand på 4,7 kOhm
GND (-)	= GND
OUT	= 3 (digital)
5V  (+)	= 5V
*/

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Assign the addresses of your 1-Wire temp sensors.
// See the tutorial on how to obtain these addresses:
// http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html

DeviceAddress insideThermometer = { 0x28, 0xFF, 0x3D, 0x9E, 0x11, 0x14, 0x00, 0xE2 };


// This demo does web requests to a fixed IP address, using a fixed gateway.
// 2010-11-27 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <EtherCard.h>

#define REQUEST_RATE 5000 // milliseconds

// ethernet interface mac address
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
// ethernet interface ip address
static byte myip[] = { 192,168,1,190 };
// gateway ip address
static byte gwip[] = { 192,168,1,1 };
// remote website ip address and port
static byte hisip[] = { 94,231,107,241 };
// remote website name
char website[] PROGMEM = "mydomain.com";

byte Ethernet::buffer[300];   // a very small tcp/ip buffer is enough here
static long timer=30000;

// called when the client request is complete
static void my_result_cb (byte status, word off, word len) {
  Serial.print("<<< reply ");
  Serial.print(millis() - timer);
  Serial.println(" ms");
  Serial.println((const char*) Ethernet::buffer + off);
}

void setup () {
  Serial.begin(57600);
  
  // Start up the library
  sensors.begin();
  // set the resolution to 10 bit (good enough?)
  sensors.setResolution(insideThermometer, 10);
  
  Serial.println("\n[getStaticIP]");
  
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");

  ether.staticSetup(myip, gwip);

  ether.copyIp(ether.hisip, hisip);
  ether.printIp("Server: ", ether.hisip);

  while (ether.clientWaitingGw())
    ether.packetLoop(ether.packetReceive());
  Serial.println("Gateway found");
  
  timer = - REQUEST_RATE; // start timing out right away
}

void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  
  
  if (tempC == -127.00) {
    //Serial.print("Error getting temperature");
    Serial.print("Error getting temperature");
  } else {
    Serial.print("Temperaturen er: ");
    Serial.print(tempC);
    Serial.println(" C");
    
    writeParam = "temp=";
    tempValue = dtostrf(tempC, 1, 2, buffer);
    writeParam += tempValue;

    ether.browseUrl(PSTR("/add.php?"), writeParam, website, my_result_cb);

    delay(1000);
  }
}

void loop () {
  
  ether.packetLoop(ether.packetReceive());
  
  if (millis() > timer + REQUEST_RATE) {
    timer = millis();
    Serial.println("\n>>> REQ");
    sensors.requestTemperatures();
    printTemperature(insideThermometer);
    
  }
}
