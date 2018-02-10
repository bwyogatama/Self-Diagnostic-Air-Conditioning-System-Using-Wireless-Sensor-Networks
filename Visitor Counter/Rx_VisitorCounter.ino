#include <SoftwareSerial.h>
#include <Printers.h>


// Initialize XBee ZB client
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx16 = Rx16Response();
Rx64Response rx64 = Rx64Response();

uint8_t payload[2]={};
int statusLed = 13;

uint8_t option = 0;
uint8_t data = 0;

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
    payload[1]=int(rx16.getData(1));

    Serial.print(char(rx16.getData(0)));
    Serial.println(int(rx16.getData(1)));
        
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



