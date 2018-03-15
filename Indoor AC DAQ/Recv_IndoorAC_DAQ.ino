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

#define DHTPIN1          8    //DHT22 input pin 8
#define DHTTYPE1         DHT22
#define DHTPIN2          9    //DHT22 input pin 9
#define DHTTYPE2         DHT22 

#define ONE_WIRE_BUS     7    //DS18B20 input pin 9

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
byte byteReceived;
float inflow_temp=34.66;
float inflow_humid=78.55;
float outflow_temp=27.57;
float outflow_humid=69.49;
float ev_inlet_temp=28.59;
float ev_outlet_temp=31.48;
uint8_t payload[44];

volatile int sleep_count = 0;
const int interval = 5;
const int sleep_total = 1;

// Create request for XBee ZB
Tx16Request packet = Tx16Request(0x2222, payload, sizeof(payload));

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
//
  pinMode(6,OUTPUT);
  digitalWrite(6,LOW);
  
  // Start the software serial port, to another device
  RS485Serial.begin(9600);   // set the data rate 

  watchdogOn(); // Turn on the watch dog timer.

  ADCSRA = ADCSRA & B01111111;
  ACSR = B10000000;
  DIDR0 = DIDR0 | B00111111;
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{

  goToSleep(); // ATmega328 goes to sleep for about 8 seconds

  if (sleep_count==sleep_total){
      //DS18B20 get Temperature Setup
      digitalWrite(6,HIGH);
      sensors.requestTemperatures();
    /*
      inflow_temp     = dht1.readTemperature();
      inflow_humid    = dht1.readHumidity ();
      outflow_temp    = dht2.readTemperature();
      outflow_humid   = dht2.readHumidity ();
      ev_inlet_temp   = sensors.getTempC(InletTemp);
      ev_outlet_temp  = sensors.getTempC(OutletTemp); */
      
      toPayload ();
      
      digitalWrite(SSerialTxControl, RS485Receive);  // Disable RS485 Transmit

     while (RS485Serial.available()==0){}
      int i=24;
      while (RS485Serial.available()>0)       //Look for data from other Arduino
       {
        byteReceived = RS485Serial.read();    // Read received byte
        payload[i]   = byteReceived;
        i++;
        delay(10);   
       }
       
       toFloat();
    
       xbee.send(packet);
       Serial.println("");
       Serial.println("=============================");

       digitalWrite(6,LOW);
       sleep_count=0;
  }
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/
void toFloat ()
{
 for (int j=0;j<11;j++){
    for (int i=0;i<4;i++){
      u.b[i]=payload[i+j*4];
    }
    Serial.println(u.f);
 }
}

void toPayload ()
{
  u.f = inflow_temp;
    for (int i=0;i<4;i++){
      payload[i]=u.b[i];
    }
    
  u.f = inflow_humid;
    for (int i=0;i<4;i++){
      payload[i+4]=u.b[i];
    }
  
  u.f = outflow_temp;
    for (int i=0;i<4;i++){
      payload[i+8]=u.b[i];
    }
    
  u.f = outflow_humid;
    for (int i=0;i<4;i++){
      payload[i+12]=u.b[i];
    }
    
  u.f = ev_inlet_temp;
    for (int i=0;i<4;i++){
      payload[i+16]=u.b[i];
    }

  u.f = ev_outlet_temp;
    for (int i=0;i<4;i++){
      payload[i+20]=u.b[i];
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
