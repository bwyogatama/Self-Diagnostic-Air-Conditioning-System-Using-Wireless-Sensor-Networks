#include <DHT.h>
#include <XBee.h>
#include <Printers.h>
#include <SoftwareSerial.h>

#define DHT1PIN 8            // Connect DHT ke pin 8 Arduino
//#define DHT2PIN 9            // Connect DHT ke pin 9 Arduino
#define DHT1TYPE DHT22       // Mendefinisikan DHT22 sebagai tipe sensor
//#define DHT2TYPE DHT22
DHT dht1(DHT1PIN, DHT1TYPE);
//DHT dht2(DHT2PIN, DHT2TYPE);

// Initialize XBee ZB client
XBee xbee = XBee();
// Payload byte array
uint8_t payload[19]={};
// Send to coordinator
//XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x41686ED6);
// Create request for XBee ZB
Tx16Request tx = Tx16Request(0x2222, payload, sizeof(payload));


/*void set_float_to_payload(float value, int index) {
  uint8_t *value_array;
  value_array = reinterpret_cast<uint8_t*>(&value);
  for(int i=0; i<sizeof(value); i++){
    payload[i+index] = value_array[i];
  }
}*/

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

TxStatusResponse txStatus = TxStatusResponse();

void setup() {
  Serial.begin(9600); 
  xbee.setSerial(Serial);
  dht1.begin();
  //dht2.begin();
}

void loop() {
  
  float t1 = dht1.readTemperature();
  float h1 = dht1.readHumidity();
  //float t2 = dht2.readTemperature();
  //float h2 = dht2.readHumidity();

  //set_float_to_payload(t1, 0);
  //set_float_to_payload(h1, 4);
  //set_float_to_payload(t2, 8);
  //set_float_to_payload(h2, 12);

   payload[0]='T';
    u.f = t1;
    for (int i=1;i<9;i++){
           payload[i]=u.b[i];
    }
    payload[9]=',';
    payload[10]='H';
     a.f = h1;
    for (int i=11;i<19;i++){
           payload[i]=a.b[i];
    }

  Serial.print("Temperature = ");
  Serial.print(t1);
  Serial.println(" C");
  Serial.print("Kelembaban = ");
  Serial.print(h1);
  Serial.println(" %");
  
  xbee.send(tx);
   
  delay(7000);
}
