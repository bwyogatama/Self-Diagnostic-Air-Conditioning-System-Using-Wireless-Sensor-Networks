#include <SoftwareSerial.h>
#include <Printers.h>
#include <XBee.h>

// Initialize XBee ZB client
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx16 = Rx16Response();
Rx64Response rx64 = Rx64Response();

uint8_t payload[19]={};
int statusLed = 13;

uint8_t option = 0;
uint8_t data = 0;

union {
      float f;
      byte b[8];
   } u;

   union {
      float f;
      byte b[8];
   } a;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  xbee.setSerial(Serial);
  pinMode(statusLed, OUTPUT);
//  xbee.begin(9600);  // start serial
  flashLed(statusLed, 30, 50); 
}

void loop() {
    
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
        // got a rx packet
        //flashLed(statusLed, 10, 5000);
        //Serial.println("got a rx packet");
        
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
                xbee.getResponse().getRx16Response(rx16);
          option = rx16.getOption();
          data = rx16.getData(0);
        } else {
                xbee.getResponse().getRx64Response(rx64);
          option = rx64.getOption();
          data = rx64.getData(0);
        }
        
        //flashLed(statusLed, 10, 50);

         
         // Receiving and printing a float
    payload[0]=char(rx16.getData(0));
    for (int i=1;i<9;i++){
      u.b[i]=rx16.getData(i);
    }
    payload[9]=char(rx16.getData(9));
    payload[10]=char(rx16.getData(10));
    for (int i=11;i<19;i++){
      a.b[i]=rx16.getData(i);
    }
         /*
         u.b[1] = rx16.getData(1);
         u.b[2] = rx16.getData(2);
         u.b[3] = rx16.getData(3);
         u.b[4] = rx16.getData(4);
         u.b[5] = rx16.getData(5);
         u.b[6] = rx16.getData(6);
         u.b[7] = rx16.getData(7);
         u.b[8] = rx16.getData(8);
         /*u.b[8] = rx16.getData(8);
         u.b[9] = rx16.getData(9);
         u.b[10] = rx16.getData(10);
         u.b[11] = rx16.getData(11);
         u.b[12] = rx16.getData(12);
         u.b[13] = rx16.getData(13);
         u.b[14] = rx16.getData(14);
         u.b[15] = rx16.getData(15);
         */
         Serial.print(char(rx16.getData(0)));
         Serial.print(u.f);
         Serial.print(char(rx16.getData(9)));
         Serial.print(char(rx16.getData(10)));
         Serial.println(a.f);
         

         //flashLed(statusLed, 10, 50);
        //myLCD.write(0xFE); myLCD.write(0x01);  // Clear Screen
        
      } else {
        // not something we were expecting
        flashLed(statusLed, 3, 1000);    
      }
    } else if (xbee.getResponse().isError()) {
      flashLed(statusLed, 3, 1000);    
      Serial.println("ERROR BUNG");
    } 
}

void flashLed(int pin, int times, int wait) {
    
    for (int i = 0; i < times; i++) {
      digitalWrite(pin, HIGH);
      delay(wait);
      digitalWrite(pin, LOW);
      
      if (i + 1 < times) {
        delay(wait);
      }
    }
}



