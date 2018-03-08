#include <Printers.h>
#include <XBee.h>
#include <DHT.h>  
#include <OneWire.h>
#include <DallasTemperature.h>
#define DHTPIN1 8
#define DHTTYPE1 DHT22
#define DHTPIN2 9
#define DHTTYPE2 DHT22 

// Initialize DHT sensor module
DHT dht1(DHTPIN1, DHTTYPE1);
DHT dht2(DHTPIN2, DHTTYPE2);

// Data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 10

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Assign the unique addresses of your 1-Wire temp sensors.
DeviceAddress InletTemp  = { 0x28, 0xFF, 0x80, 0xF8, 0xC1, 0x17, 0x05, 0xAB };
DeviceAddress OutletTemp = { 0x28, 0xFF, 0xE4, 0xEC, 0xC1, 0x17, 0x04, 0xCD };

// Initialize XBee ZB client
XBee xbee = XBee();
// Payload byte array
uint8_t payload[24]={};

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
  sensors.setResolution(InletTemp, 10);
  sensors.setResolution(OutletTemp, 10);
  }

void loop() {
  
   sensors.requestTemperatures();
   float t1 = dht1.readTemperature();
   float h1 = dht1.readHumidity ();
   float t2 = dht2.readTemperature();
   float h2 = dht2.readHumidity ();
   float t3 = sensors.getTempC(InletTemp);
   float t4 = sensors.getTempC(OutletTemp);
   
    // Sent Data of Indoor AC DAQ 
    // Sent Temperature Inflow
    u.f = t1;
    for (int i=0;i<4;i++){
      payload[i]=u.b[i];
    }
    Serial.println(u.f);

    // Sent Humidity Inflow
    u.f = h1;
    for (int i=0;i<4;i++){
      payload[i+4]=u.b[i];
    }
    Serial.println(u.f);

    // Sent Temperature Outflow
    u.f = t2;
    for (int i=0;i<4;i++){
      payload[i+8]=u.b[i];
    }
    Serial.println(u.f);

    // Sent Humidity Outflow
    u.f = h2;
    for (int i=0;i<4;i++){
      payload[i+12]=u.b[i];
    }
    Serial.println(u.f);

    // Sent Temperature Inlet
    u.f = t3;
    for (int i=0;i<4;i++){
      payload[i+16]=u.b[i];
    }
    Serial.println(u.f);

    // Sent Temperature Outlet
    u.f = t4;
    for (int i=0;i<4;i++){
      payload[i+20]=u.b[i];
    }
    Serial.println(u.f);
    
    // Send Packet to Receiver XBee
    xbee.send(packet);
    Serial.println("");
    Serial.println("=====================");
    
  delay(8000);
}
