#include <SoftwareSerial.h>
#include <Printers.h>
#include <XBee.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>

// Initialize BMP sensor module
Adafruit_BMP085 bmp;
// Pin SDA ke A4
// Pin SCL ke A5

// Initialize XBee ZB client
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx16 = Rx16Response();
Rx64Response rx64 = Rx64Response();

uint8_t payload[100]={};
uint8_t option = 0;
uint8_t data = 0;
uint16_t address = 0;

// 
union {
      float f;
      byte b[4];
   } u;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  xbee.setSerial(Serial);
  bmp.begin();
}

void loop() {
    /*
   // Read data temperature and pressure from BMP180 sensor
   float t = bmp.readTemperature();
   float p = bmp.readPressure();
   Serial.println("DATA ROOM DAQ");
   Serial.print("Room Temperature = ");
   Serial.println(t);
   Serial.print("Room Press   = ");
   Serial.println(p);
   Serial.println("========================================================");
   //delay(10);
   */

   // Xbee read packet from another tx Xbee
   xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
        // got a rx packet

        if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
          xbee.getResponse().getRx16Response(rx16);
          option = rx16.getOption();
          data = rx16.getData(0);
          
        } else {
                xbee.getResponse().getRx64Response(rx64);
          option = rx64.getOption();
          data = rx64.getData(0);
        }
    
    address = rx16.getRemoteAddress16(); // Get Adress of XBee Transmitter

    /*
    // Data ROOM DAQ
    if (address == 0x3333) { // Get adress of Room DAQ XBee Tx
    // Receiving and printing a float Temperature
    Serial.println("DATA ROOM DAQ");
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i);
    }
    float A = u.f;
    Serial.print("Room Temperature = ");
    Serial.println(A);
       
    // Receiving and printing a float Pressure
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i+4);
    }
    float B = u.f;
    Serial.print("Room Press   = ");
    Serial.println(B);
    Serial.print("Address   = 0x");
    Serial.println(address, HEX);
    Serial.println("========================================================");
    } 
    */
    
    // Get Data of Visitor Counter
     if (address == 0x3333){  // Adress of Visitor Counter Xbee Tx
    // Receiving and printing a counter of people
    Serial.println("VISITOR COUNTER");
    payload[0]=rx16.getData(0);
    int A = payload[0];
    Serial.print("JmlOrang  = ");
    Serial.println(A);
    Serial.print("Address   = 0x ");
    Serial.println(address, HEX);
    Serial.println("========================================================");
    }
       
    // Get Data of Outdoor AC and Indoor ACDAQ 
     else if (address == 0x5555){  // Adress of Outdoor AC DAQ Xbee Tx
    // Receiving and printing a Tempperature Ambient
    Serial.println("OUTDOOR AC DAQ");
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i);
    }
    float B = u.f;
    Serial.print("inflow_temp  = ");
    Serial.println(B);
    
    // Receiving and printing a Current of Compressor
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i+4);
    }
    float C = u.f;
    Serial.print("inflow_humid   = ");
    Serial.println(C);
    
    // Receiving and printing a Temperature of SUction Line
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i+8);
    }
    float D = u.f;
    Serial.print("outflow_temp   = ");
    Serial.println(D);
    
    // Receiving and printing a Pressure of Suction Line
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i+12);
    }
    float E = u.f;
    Serial.print("outflow_humid   = ");
    Serial.println(E);

    // Receiving and printing a Pressure of Liquid Line
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i+16);
    }
    float F = u.f;
    Serial.print("ev_inlet_temp  = ");
    Serial.println(F);

    
    // Receiving and printing a Pressure of Liquid Line
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i+20);
    }
    float G = u.f;
    Serial.print("ev_outlet_temp  = ");
    Serial.println(G);

    // Receiving and printing a Pressure of Liquid Line
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i+24);
    }
    float H = u.f;
    Serial.print("outdoor_temp  = ");
    Serial.println(H);

    // Receiving and printing a Pressure of Liquid Line
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i+28);
    }
    float I = u.f;
    Serial.print("con_inlet_temp  = ");
    Serial.println(I);

    // Receiving and printing a Pressure of Liquid Line
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i+32);
    }
    float J = u.f;
    Serial.print("con_outlet_temp  = ");
    Serial.println(J);

    // Receiving and printing a Pressure of Liquid Line
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i+36);
    }
    float K = u.f;
    Serial.print("compressor_current  = ");
    Serial.println(K);

    // Receiving and printing a Pressure of Liquid Line
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i+40);
    }
    float L = u.f;
    Serial.print("suction_pressure = ");
    Serial.println(L);      
    Serial.print("Address   = 0x ");
    Serial.println(address, HEX);
    Serial.println("========================================================");
    }  
    
         
      } else {
        // not something we were expecting
        //flashLed(statusLed, 3, 1000);    
        //Serial.println("NOT EXPECTING");
      }
    } else if (xbee.getResponse().isError()) {
      //flashLed(statusLed, 3, 1000);    
      //Serial.println("ERROR");
    }
} 

