#include <Printers.h>
#include <XBee.h>
#include <DHT.h>  
#include <Adafruit_BMP085.h>

// Initialize DHT sensor module
Adafruit_BMP085 bmp;

// Initialize XBee ZB client
XBee xbee = XBee();
// Payload byte array
uint8_t payload[19]={};
// Send to coordinator
//XBeeAddress64 addr64 = XBeeAddress64(0, 0);
// Create request for XBee ZB
Tx16Request packet = Tx16Request(0x2222, payload, sizeof(payload));
 

union {
    //uint8_t b[4];
    float f;
    byte b[8];
} u;

union {
    //uint8_t b[4];
    float f;
    byte b[8];
} a;

void setup() {
  Serial.begin(9600); 
  xbee.setSerial(Serial);
  bmp.begin();
}

void loop() {

  xbee.send(packet);
  
  float t = bmp.readTemperature();
  float p = bmp.readPressure ();
 
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  //if (!isnan(t) && !isnan(h)) {
     
    // convert humidity into a byte array and copy it into the payload array

    
    payload[0]='T';
    u.f = t;
    for (int i=1;i<9;i++){
           payload[i]=u.b[i];
    }
    payload[9]=',';
    payload[10]='P';
     a.f = p;
    for (int i=11;i<19;i++){
           payload[i]=a.b[i];
    }
    Serial.println(t);
    Serial.println(p);
    xbee.send(packet);
  
    
  delay(20000);
}
