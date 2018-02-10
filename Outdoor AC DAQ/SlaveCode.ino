// Nano_1_I2C_comm_slave_DS18B20
// this Nano is the i2c slave
// slave operates a DS18B20 temperature sensor
//
// output of DS18B20 are floats
// master requests temperatures
// slave sends temp float with two decimals coded as chars
// master decodes chars to float
// and displays with one decimal on 128×32 OLED
// 16 July 2017
// Floris Wouterlood
// public domain

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 8

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress Probe_01 = { 0x28, 0xFF, 0xD7, 0x91, 0x92, 0x16, 0x04, 0x69};
float DS18B20_float = 0;
char t[10]; //empty array where to put the numbers going to the master
volatile int Val; // variable used by the master to sent data to the slave


void setup() 
{
  Serial.begin(9600); // start serial monitor
  Wire.begin(8); // between brackets slave id: #8
  Wire.onRequest(requestEvent); // master will requests data
  Wire.onReceive(receiveEvent); // what to do when receiving data
  
  Serial.print ("Dallas Control Library Version ");
  Serial.println (DALLASTEMPLIBVERSION);
  
  sensors.begin(); // initialize temperature probe
  sensors.setResolution(Probe_01, 10); // resolution 10 is sufficient
  Serial.println();
  Serial.print("Devices on one wire bus = ");
  Serial.println (sensors.getDeviceCount());
  Serial.println ("Getting temperatures… ");
  Serial.println ("================================ ");
}


void loop() {
// read DS18B20 probe a

sensors.requestTemperatures (); // read temperature on one wire bus device
printTemperature (Probe_01);
Serial.print ("temp = ");
Serial.print (DS18B20_float,2);
Serial.println (" *C");

// convert float to string with one decimal (floatVar, minStringWidthIncDecimalPoint,
// numVarsAfterDecimal, empty array)

dtostrf(DS18B20_float, 3, 2, t);
delay(500);
}


// subroutines ============================================

// subroutine: what to do when asked for data =============
void requestEvent() {
Wire.write(t);
}

// what to do when receiving data from master
void receiveEvent(int howMany)
{Val = Wire.read();}

// subroutine printTemperature ===========================

void printTemperature (DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempCByIndex(deviceAddress);;
  
  if (tempC == -127.00)
  {
    Serial.print ("Error getting temperature ");
  }
  else
  {
    DS18B20_float=(tempC); // float variable
  }
}
