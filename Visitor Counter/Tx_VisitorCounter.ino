#include <Printers.h>
#include <XBee.h>

// Sensor ultrasonik 1
#define trigPin1 6
#define echoPin1 7
// Sensor ultrasonik 2
#define trigPin2 8
#define echoPin2 9

int hit1 = 0;
int hit2 = 0;
int count = 0;
int maxDist = 15;

// Initialize XBee ZB client
XBee xbee = XBee();
// Payload byte array
uint8_t payload[2]={};
// Send to coordinator
// Create request for XBee ZB
Tx16Request packet = Tx16Request(0x2222, payload, sizeof(payload));


void setup() {
  Serial.begin(9600); 
  xbee.setSerial(Serial);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
}

void loop() {

  xbee.send(packet);
  
  int duration1;
  int distance1;
  digitalWrite(trigPin1, LOW);
  delayMicroseconds (2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds (5);
  digitalWrite(trigPin1, LOW);

  duration1 = pulseIn (echoPin1, HIGH);
  distance1 = (duration1/2)/29.1;

  if (distance1 < maxDist){
    //Serial.println ("Entering.......");
    //Serial.println(distance1);
    hit1 = 1;
  }

  // Check Sensor Ultrasonik 2
  int duration2;
  int distance2;
  digitalWrite(trigPin2, LOW);
  delayMicroseconds (2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds (5);
  digitalWrite(trigPin2, LOW);

  duration2 = pulseIn (echoPin2, HIGH);
  distance2 = (duration2/2)/29.1;

  if (distance2 < maxDist){
    //Serial.println ("Exitting.......");
    hit2 = 1;
  } 

  //entry was triggered, scan exit until exit is hit
  while (hit1 == 1 && hit2 == 0){
    // check sensor 2
    int dur;
    int dist;
    digitalWrite(trigPin2, LOW);
    delayMicroseconds (2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds (5);
    digitalWrite(trigPin2, LOW);

    dur = pulseIn (echoPin2, HIGH);
    dist = (dur/2)/29.1;

    if (dist < maxDist ){
      // reset the two values
      hit1 = 0;
      hit2 = 0;
      count++;
      payload[1]=count;  
      Serial.println(" M A S U K");
      Serial.print("Peopele = ");
      Serial.println(count);
      delay(200);
      break;
      
    }  
  }
  //exit was triggered, scan entry until entry is hit
  while (hit1 == 0 && hit2 == 1){
    // check sensor 1
    int dur;
    int dist;
    digitalWrite(trigPin1, LOW);
    delayMicroseconds (2);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds (5);
    digitalWrite(trigPin1, LOW);

    dur = pulseIn (echoPin1, HIGH);
    dist = (dur/2)/29.1;

    if (dist < maxDist ){
      // reset the two values
      hit1 = 0;
      hit2 = 0;
      count--;
      payload[1]=count;  
      Serial.println(" K E L U A R ");
      Serial.print(" Peopele = ");
      Serial.println(count);
      delay(200);
            break;
      
    }
    payload[0]='N'; 
       
      
  }
 
  xbee.send(packet);
     
  delay(1000);
}
