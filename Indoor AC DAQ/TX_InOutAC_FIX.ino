/*-----( Import needed libraries )-----*/
#include <SoftwareSerial.h>
#include <XBee.h>
#include <DHT.h>  
#include <OneWire.h>
#include <DallasTemperature.h>
#include <avr/sleep.h>

/*-----( Declare Constants and Pin Numbers )-----*/
#define SSerialRX        10  //Serial Receive pin
#define SSerialTX        11  //Serial Transmit pin

#define SSerialTxControl 3   //RS485 Direction control

#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define DHTPIN1          7    //DHT22 input pin 12
#define DHTTYPE1         DHT22
#define DHTPIN2          8    //DHT22 input pin 13
#define DHTTYPE2         DHT22 

#define ONE_WIRE_BUS     12    //DS18B20 input pin 8

const int XBee_wake = 6; // Arduino pin used to sleep the XBee

/*-----( Declare objects )-----*/
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

XBee xbee = XBee();                   // Initialize XBee ZB client

DHT dht1(DHTPIN1, DHTTYPE1);
DHT dht2(DHTPIN2, DHTTYPE2);

OneWire oneWire(ONE_WIRE_BUS);        // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature. 

// Assign the unique addresses of your 1-Wire temp sensors.
DeviceAddress InletTemp  = { 0x28, 0xFF, 0x80, 0xF8, 0xC1, 0x17, 0x05, 0xAB };
DeviceAddress OutletTemp = { 0x28, 0xFF, 0xE4, 0xEC, 0xC1, 0x17, 0x04, 0xCD };

/*-----( Declare Variables )-----*/
const int wakeUpPin = 5;
byte byteReceived;
float inflow_temp;
float inflow_humid;
float outflow_temp;
float outflow_humid;
float ev_inlet_temp;
float ev_outlet_temp;
float airflow_speed;
uint8_t payload1[28];
uint8_t payload2[20];
bool toggle=0;

volatile int sleep_count = 0;
const int interval = 1;
const int sleep_total = 2;

// Create request for XBee ZB
Tx16Request packet1 = Tx16Request(0x3333, payload1, sizeof(payload1));
Tx16Request packet2 = Tx16Request(0x3333, payload2, sizeof(payload2));

TxStatusResponse txStatus = TxStatusResponse();
union {
    uint8_t b[4];
    float f;
    //byte b[8];
} u;

void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port
  Serial.begin(9600);
  Serial.println("=====START INDOOR AC DAQ=====");
  xbee.setSerial(Serial);

  pinMode(SSerialTxControl, OUTPUT);    
  
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver   

  // Start up the library
  sensors.begin();
  // set the resolution to 10 bit
  sensors.setResolution(InletTemp, 10);
  sensors.setResolution(OutletTemp, 10);
  // Start the software serial port, to another device
  RS485Serial.begin(9600);   // set the data rate 

  watchdogOn(); // Turn on the watch dog timer.

  ADCSRA = ADCSRA & B01111111;
  ACSR = B10000000;
  DIDR0 = DIDR0 | B00111111; 
  
  pinMode(wakeUpPin,OUTPUT);
  digitalWrite(wakeUpPin,LOW);
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  //delay(8000);
  goToSleep(); // ATmega328 goes to sleep for about 8 seconds

  if (sleep_count==sleep_total){
      //delay(100);
      //DS18B20 get Temperature Setup
      
      //delay(50);

      if (toggle==0){
      sensors.requestTemperatures();

      inflow_temp     = dht1.readTemperature();
      inflow_humid    = dht1.readHumidity ();
      outflow_temp    = dht2.readTemperature();
      outflow_humid   = dht2.readHumidity ();
      ev_inlet_temp   = sensors.getTempC(InletTemp);
      ev_outlet_temp  = sensors.getTempC(OutletTemp);
      /*float tempC = ((((float)tempRawAD * 5.0) / 1024.0) - 0.400) / 0.0195; 
      float Volts = (float)analogRead(A0)*5.0/1023.0;
      airflow_speed =  pow((((Volts-1.3692)/(3.038517*(pow(tempC,0.115157))))/0.087288),3.009364); 
      */

      airflow_speed=510.2;
      
      toPayload ();

      toFloat1();

      xbee.send(packet1);

      if (xbee.readPacket(2000)){
        Serial.println("Success");
      }
      else{
        Serial.println("Error");
      }

      

      delay(100);
      }
      //toFloat1();

      else{

      //delay(5000);
      //toFloat1();
      
      
      digitalWrite(wakeUpPin,HIGH);
      digitalWrite(SSerialTxControl, RS485Receive);  // Disable RS485 Transmit

      while (RS485Serial.available()!=20){}
      Serial.println(RS485Serial.available());
      int i=0;
      while (RS485Serial.available()>0)       //Look for data from other Arduino
       {
        byteReceived = RS485Serial.read();    // Read received byte
        payload2[i]   = byteReceived;
        i++;
        //delay(395);   
       }
       
       
       toFloat2();
       

       Serial.println("");
       Serial.println("=============================");
       //pinMode(XBee_wake, OUTPUT);
       //digitalWrite(XBee_wake, LOW);
       xbee.send(packet2);

       
      if (xbee.readPacket(2000)){
        Serial.println("Success");
      }
      else{
        Serial.println("Error");
      }
      delay(100);
       
/////////////////////////////////////////////
      /*while (!(xbee.readPacket(2000))){
        xbee.send(packet);
      }

      // should be a znet tx status              
      if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
         xbee.getResponse().getTxStatusResponse(txStatus);
        
         // get the delivery status, the fifth byte
           if (txStatus.getStatus() == SUCCESS) {
              // success.  time to celebrate
              Serial.println("Success");
              //flashLed(statusLed, 5, 50);
           } else {
              // the remote XBee did not receive our packet. is it powered on?
              //flashLed(errorLed, 3, 500);
              Serial.println("Error1");
           }
              
      } else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
      // or flash error led
      Serial.println("Error2");
      }*/
       
//////////////////////////////////////////////
    /*if (xbee.readPacket(5000)) {
        // got a response!

        // should be a znet tx status              
      if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
         xbee.getResponse().getTxStatusResponse(txStatus);
        
         // get the delivery status, the fifth byte
           if (txStatus.getStatus() == SUCCESS) {
              // success.  time to celebrate
              Serial.println("Success");
              //flashLed(statusLed, 5, 50);
           } else {
              // the remote XBee did not receive our packet. is it powered on?
              //flashLed(errorLed, 3, 500);
              Serial.println("Error1");
           }
        }      
    } else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
      // or flash error led
      Serial.println("Error2");
    } else {
      // local XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
      //flashLed(errorLed, 2, 50);
      Serial.println("Error3");
    }*/
//////////////////////////////////////////////////
       //delay(100);
       //pinMode(XBee_wake, INPUT); // Put pin in a high impedence state
       //digitalWrite(XBee_wake, HIGH);
       digitalWrite(wakeUpPin,LOW);
      }
       toggle = !toggle;
       sleep_count=0;  
  }
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/
void toFloat1 ()
{
 for (int j=0;j<7;j++){
    for (int i=0;i<4;i++){
      u.b[i]=payload1[i+j*4];
    }
    Serial.println(u.f);
 }
}

void toFloat2 ()
{
 for (int j=0;j<5;j++){
    for (int i=0;i<4;i++){
      u.b[i]=payload2[i+j*4];
    }
    Serial.println(u.f);
 }
}

void toPayload ()
{
  u.f = inflow_temp;
    for (int i=0;i<4;i++){
      payload1[i]=u.b[i];
    }
    
  u.f = inflow_humid;
    for (int i=0;i<4;i++){
      payload1[i+4]=u.b[i];
    }
  
  u.f = outflow_temp;
    for (int i=0;i<4;i++){
      payload1[i+8]=u.b[i];
    }
    
  u.f = outflow_humid;
    for (int i=0;i<4;i++){
      payload1[i+12]=u.b[i];
    }
    
  u.f = ev_inlet_temp;
    for (int i=0;i<4;i++){
      payload1[i+16]=u.b[i];
    }

  u.f = ev_outlet_temp;
    for (int i=0;i<4;i++){
      payload1[i+20]=u.b[i];
    }

  
  u.f = airflow_speed;
    for (int i=0;i<4;i++){
      payload1[i+24]=u.b[i];
    }
}

void watchdogOn() {
MCUSR = MCUSR & B11110111;  
WDTCSR = WDTCSR | B00011000; 
WDTCSR = B00100001;
WDTCSR = WDTCSR | B01000000;
MCUSR = MCUSR & B11110111;
}

void goToSleep()   
{
set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode.
sleep_enable(); // Enable sleep mode.
sleep_mode(); // Enter sleep mode.
sleep_disable(); // Disable sleep mode after waking.                   
}

ISR(WDT_vect)
{
sleep_count ++;
}

//*********( THE END )***********

