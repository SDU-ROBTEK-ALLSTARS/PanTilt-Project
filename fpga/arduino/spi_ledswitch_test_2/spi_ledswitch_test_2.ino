/*
  Digital Pot Control
  
  This example controls an Analog Devices AD5206 digital potentiometer.
  The AD5206 has 6 potentiometer channels. Each channel's pins are labeled
  A - connect this to voltage
  W - this is the pot's wiper, which changes when you set it
  B - connect this to ground.
 
 The AD5206 is SPI-compatible,and to command it, you send two bytes, 
 one with the channel number (0 - 5) and one with the resistance value for the
 channel (0 - 255).  
 
 The circuit:
  * All A pins  of AD5206 connected to +5V
  * All B pins of AD5206 connected to ground
  * An LED and a 220-ohm resisor in series connected from each W pin to ground
  * CS - to digital pin 10  (SS pin)
  * SDI - to digital pin 11 (MOSI pin)
  * CLK - to digital pin 13 (SCK pin)
 
 created 10 Aug 2010 
 by Tom Igoe
 
 Thanks to Heather Dewey-Hagborg for the original tutorial, 2005
 
*/


// inslude the SPI library:
#include <SPI.h>


// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;

void setup() {
  // set the slaveSelectPin as an output:
  pinMode (slaveSelectPin, OUTPUT);
  pinMode (2, OUTPUT);
  pinMode (3, OUTPUT);
  pinMode (4, OUTPUT);
  pinMode (5, OUTPUT);
  // initialize SPI:
  Serial.begin(9600);
  Serial.println("ping");  
  SPI.setDataMode(SPI_MODE0);
  SPI.begin(); 
}

void loop() 
{
  byte cmd, result;
  if (Serial.available()) 
  {
    cmd = Serial.read();
    digitalWrite(slaveSelectPin,LOW);
    result = SPI.transfer(cmd);
    digitalWrite(slaveSelectPin,HIGH);
    Serial.write(result); 
    
    if (result == 0x00)
      digitalWrite(2,HIGH);
    else
      digitalWrite(2,LOW);
    
    if (result == 0x55)
      digitalWrite(3,HIGH);
    else
      digitalWrite(3,LOW);
      
    if (result == 0x30)
      digitalWrite(4,HIGH);
    else
      digitalWrite(4,LOW);
      
     if (result == 0xFF)
      digitalWrite(5,HIGH);
    else
      digitalWrite(5,LOW); 
  }
}