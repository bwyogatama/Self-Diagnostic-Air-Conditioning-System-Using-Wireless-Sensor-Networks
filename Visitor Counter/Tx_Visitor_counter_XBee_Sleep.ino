#include <Printers.h>
#include <XBee.h>

// Sensor ultrasonik 1
#define trigPin1 6
#define echoPin1 7
// Sensor ultrasonik 2
#define trigPin2 8
#define echoPin2 10

const int XBee_wake = 9; // Arduino pin used to sleep the XBee

int i = 0;
int i2 = 0;
int currentState = 0;
int previousState = 0;
int currentState2 = 0;
int previousState2 = 0;

// This variable is made volatile because it is changed inside an interrupt function
volatile int sleep_count = 0;   // Keep track of how many sleep cycles have been completed.
//const int interval = 1;   // Interval in minutes between waking and doing tasks.
//const int sleep_total = (interval*60)/8; // Approximate number 
// of sleep cycles needed before the interval defined above 
// elapses. Not that this does integer math.
const int sleep_total = 8;

// Initialize XBee ZB client
XBee xbee = XBee();
// Payload byte array
uint8_t payload[1]={};
// Send to coordinator
// Create request for XBee ZB
Tx16Request packet = Tx16Request(0x2222, payload, sizeof(payload));



void setup() {
 // initialize I/O pins
 Serial.begin(9600); 
 xbee.setSerial(Serial);
  pinMode(trigPin1, OUTPUT);
 pinMode(echoPin1, INPUT);
 pinMode(trigPin2, OUTPUT);
 pinMode(echoPin2, INPUT);
}

void loop() {
     
 long duration1, distance1;
 digitalWrite(trigPin1, LOW); 
 delayMicroseconds(2); 
 digitalWrite(trigPin1, HIGH);
 delayMicroseconds(10); 
 digitalWrite(trigPin1, LOW);
 duration1 = pulseIn(echoPin1, HIGH);
 distance1 = (duration1/2) / 29.1;
 
 long duration2, distance2;
 digitalWrite(trigPin2, LOW); 
 delayMicroseconds(2); 
 digitalWrite(trigPin2, HIGH);
 delayMicroseconds(10); 
 digitalWrite(trigPin2, LOW);
 duration2 = pulseIn(echoPin2, HIGH);
 distance2 = (duration2/2) / 29.1;
 
 // object entering in the system
 if (distance1 <= 30){
 currentState = 1;
 }
  else {
      currentState = 0;
 }
 
 
 if(currentState != previousState){
  while(currentState == 1) {
    long duration2, distance2;
    digitalWrite(trigPin2, LOW); 
    delayMicroseconds(2); 
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10); 
    digitalWrite(trigPin2, LOW);
    duration2 = pulseIn(echoPin2, HIGH);
    distance2 = (duration2/2) / 29.1;
    if (distance2 <= 10){
        currentState2 = 1;
      }
        else {
              currentState2 = 0;
        }
    
   
    if(currentState2 != previousState2){
        if(currentState2 == 1) {
            i = i+1;
            payload[0]=i;  
            Serial.println(" M A S U K");
            Serial.print("Peopele = ");
            Serial.println(i);
            delay(1000); // pause for 1/2 second
        }
          else {
            payload[0]=i;  
            Serial.println(" M A S U K");
            Serial.print("Peopele = ");
            Serial.println(i);
          }
          return;
        }
     }
          
 }

 
 // object exit from the system 
if (distance2 <= 30){
 currentState2 = 1;
  }
    else {
          currentState2 = 0;
  }
 delay(100);
 
 if(currentState2 != previousState2){
    while (currentState2 == 1) {
      long duration1 , distance1;
       digitalWrite(trigPin1, LOW); 
       delayMicroseconds(2); 
       digitalWrite(trigPin1, HIGH);
       delayMicroseconds(10); 
       digitalWrite(trigPin1, LOW);
        
       duration1 = pulseIn(echoPin1, HIGH);
       distance1 = (duration1/2) / 29.1;
       
 if (distance1 <= 30){
    currentState = 1;
    }
      else {
          currentState = 0;
    }
    delay(100);


 if(currentState != previousState){
    if(currentState == 1) {
      i = i-1; 
      payload[0]= i;  
      Serial.println(" K E L U A R ");
      Serial.print(" Peopele = ");
      Serial.println(i);
      delay(1000); // pause for 1/2 second
    }
      else {
      payload[0]= i;  
      Serial.println(" K E L U A R ");
      Serial.print(" Peopele = ");
      Serial.println(i);
      }
      return;
     }
    }
  }


    if ( i != i2){
      pinMode(XBee_wake, OUTPUT);
      digitalWrite(XBee_wake, LOW);

      xbee.send(packet);
    
      //Sleep the XBee
      pinMode(XBee_wake, INPUT); // Put pin in a high impedence state
      digitalWrite(XBee_wake, HIGH);
      
      i2 = i;
    }
 }
