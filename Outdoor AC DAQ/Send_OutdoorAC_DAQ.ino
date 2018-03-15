/*-----( Import needed libraries )-----*/
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EmonLib.h>
#include <LowPower.h>

/*-----( Declare Constants and Pin Numbers )-----*/
#define SSerialRX        10  //Serial Receive pin
#define SSerialTX        11  //Serial Transmit pin

#define SSerialTxControl 3   //RS485 Direction control
#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define ONE_WIRE_BUS 9       // Data wire is plugged into pin 9 on the Arduino

/*-----( Declare objects )-----*/
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

EnergyMonitor emon1;                  // Create an instance for SCT013-030

OneWire oneWire(ONE_WIRE_BUS);        // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature.

// Assign the unique addresses of your 1-Wire temp sensors.
DeviceAddress OutdoorTemp = { 0x28, 0xFF, 0x37, 0x7C, 0x54, 0x16, 0x04, 0xE5 };
DeviceAddress InletTemp   = { 0x28, 0xAB, 0x6E, 0x04, 0x00, 0x00, 0x80, 0x8B };
DeviceAddress OutletTemp  = { 0x28, 0x8E, 0x74, 0x04, 0x00, 0x00, 0x80, 0x17 };

/*-----( Declare Variables )-----*/
const int wakeUpPin = 2;
int byteReceived;
int byteSend;
int offset = 102;     // zero pressure adjust
int fullScale = 922;  // max pressure (span) adjust
int rawValue ;        // A/D readings
float outdoor_temp=27.1;
float con_inlet_temp=75.2;
float con_outlet_temp=34.3;
float compressor_current=4.2;
float suction_pressure=150.3;
float suction_pressure_MPA;
byte payload[20];

union {
    uint8_t b[4];
    float f;
    //byte b[8];
} u;


void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(9600);
  Serial.println("=====START OUTDOOR AC DAQ=====");  // Can be ignored
  
  //pinMode(Pin13LED, OUTPUT);   
  pinMode(SSerialTxControl, OUTPUT);  
  digitalWrite(SSerialTxControl, RS485Receive);      // Init Transceiver

  
  
  // Start up the library
  sensors.begin();
  // set the resolution to 10 bit
  sensors.setResolution(OutdoorTemp, 10);
  sensors.setResolution(InletTemp, 10);
  sensors.setResolution(OutletTemp, 10);

  // Current SCT013-030 Sensor: input pin, calibration.
  emon1.current(1, 30);
  
  // Start the software serial port, to another device
  RS485Serial.begin(9600);   // set the data rate 
  pinMode(wakeUpPin,INPUT);
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  attachInterrupt(0,wakeUp,RISING);

  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  detachInterrupt(0);
      //DS18B20 get Temperature Setup
  sensors.requestTemperatures();
  /*
  outdoor_temp        = sensors.getTempC(OutdoorTemp);
  con_inlet_temp      = sensors.getTempC(InletTemp);
  con_outlet_temp     = sensors.getTempC(OutletTemp);
  compressor_current  = emon1.calcIrms(1480);
  
  // pressure conversion to PSI
  rawValue              = analogRead(2);
  suction_pressure_MPA  = (rawValue - offset) *3.1/(fullScale - offset);
  suction_pressure      = suction_pressure_MPA * 145.038;
  if (suction_pressure<0) { suction_pressure=0;} */

  //Convert all measurement to Byte with Function
  toByte();

  //Send all measurement to IndoorAC DAQ via Serial
  digitalWrite(SSerialTxControl, RS485Transmit);  // Enable RS485 Transmit   
  Serial.println(RS485Serial.write(payload,20));  // Send byte to Remote Arduino

  Serial.println("==============================");

       
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/
void toByte ()
{
  u.f = outdoor_temp;
    for (int i=0;i<4;i++){
      payload[i]=u.b[i];
    }
  Serial.println(u.f);
    
  u.f = con_inlet_temp;
    for (int i=0;i<4;i++){
      payload[i+4]=u.b[i];
    }
  Serial.println(u.f);
  
  u.f = con_outlet_temp;
    for (int i=0;i<4;i++){
      payload[i+8]=u.b[i];
    }
  Serial.println(u.f);
    
  u.f = compressor_current;
    for (int i=0;i<4;i++){
      payload[i+12]=u.b[i];
    }
  Serial.println(u.f);
    
  u.f = suction_pressure;
    for (int i=0;i<4;i++){
      payload[i+16]=u.b[i];
    }
  Serial.println(u.f);
}

void wakeUp() {
  //Serial.println("tes");
  
  }
//*********( THE END )***********
