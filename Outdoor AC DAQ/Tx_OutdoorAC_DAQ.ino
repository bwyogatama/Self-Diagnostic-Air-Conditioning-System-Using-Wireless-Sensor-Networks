#include <Printers.h>
#include <XBee.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EmonLib.h>                   // Include Emon Library


// Initialize sensor module
// Create an instance for SCT013-030
EnergyMonitor emon1;

// Data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 11

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Assign the unique addresses of your 1-Wire temp sensors.
DeviceAddress OutdoorTemp = { 0x28, 0xFF, 0x37, 0x7C, 0x54, 0x16, 0x04, 0xE5 };
DeviceAddress InletTemp   = { 0x28, 0xAB, 0x6E, 0x04, 0x00, 0x00, 0x80, 0x8B };
DeviceAddress OutletTemp  = { 0x28, 0x8E, 0x74, 0x04, 0x00, 0x00, 0x80, 0x17 };

// Initialize XBee ZB client
XBee xbee = XBee();
// Payload byte array
uint8_t payload[20]={};

// Create request for XBee ZB
Tx16Request packet = Tx16Request(0x2222, payload, sizeof(payload));

union {
    uint8_t b[4];
    float f;
    //byte b[8];
} u;

void setup() {
  Serial.begin(9600); 
  xbee.setSerial(Serial);
  // Start up the library
  sensors.begin();
  // set the resolution to 10 bit
  sensors.setResolution(OutdoorTemp, 10);
  sensors.setResolution(InletTemp, 10);
  sensors.setResolution(OutletTemp, 10);
  // Current: input pin, calibration.
  emon1.current(1, 30);
  }

void loop() {

   sensors.requestTemperatures();
   float t1 = sensors.getTempC(OutdoorTemp);
   float t2 = sensors.getTempC(InletTemp);
   float t3 = sensors.getTempC(OutletTemp);
   float i1 = emon1.calcIrms(1480);

   int offset = 102; // zero pressure adjust
   int fullScale = 922; // max pressure (span) adjust
   int rawValue = analogRead(A2); // A/D readings in Analog 2
   float p0 = (rawValue - offset) *3.1/(fullScale - offset); // pressure conversion
   float p1 = p0 * 145.038; // real pressure conversion in PSI
   //float p1 = 500.00;
   
    // Sent Data of Outdoor AC DAQ 
    // Sent Temperature Outdoor
    u.f = t1;
    for (int i=0;i<4;i++){
      payload[i]=u.b[i];
    }
    Serial.println(u.f);

    // Sent Temperature Inlet
    u.f = t2;
    for (int i=0;i<4;i++){
      payload[i+4]=u.b[i];
    }
    Serial.println(u.f);

    // Sent Temperature Outlet
    u.f = t3;
    for (int i=0;i<4;i++){
      payload[i+8]=u.b[i];
    }
    Serial.println(u.f);

    // Sent Currentt
    u.f = i1;
    for (int i=0;i<4;i++){
      payload[i+12]=u.b[i];
    }
    Serial.println(u.f);

    // Sent Suction Line Pressure
    u.f = p1;
    for (int i=0;i<4;i++){
      payload[i+16]=u.b[i];
    }
    Serial.println(u.f);
    
    // Send Packet to Receiver XBee
    xbee.send(packet);
    Serial.println("");
    Serial.println("=====================");
    
  delay(8000);
}
