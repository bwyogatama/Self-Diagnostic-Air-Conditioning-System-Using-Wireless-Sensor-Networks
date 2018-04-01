#include <Printers.h>
#include <XBee.h>
//#include <Adafruit_Sensor.h>
//#include <Adafruit_BMP085.h>
#include <SoftwareSerial.h>
SoftwareSerial ArduinoSerial(6, 7); // Pin Serial2 pada ECU utama

// Initialize BMP sensor module
// Adafruit_BMP085 bmp;
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

float room_temp = 0;
float room_press = 0;
int visitors = 0;
float inflow_temp = 0;
float inflow_humid = 0;
float outflow_temp = 0;
float outflow_humid = 0;
float compressor_current = 0;
float suction_pressure = 0;
float outdoor_temp = 0;
float compressor_temp = 70;
float ev_inlet_temp = 0;
float ev_outlet_temp = 0;
float con_inlet_temp = 0;
float con_outlet_temp = 0;
float airflow_speed = 0;

// union data to float
union {
      float f;
      byte b[4];
   } u;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  xbee.setSerial(Serial);
  //bmp.begin();
 //Serial.begin(115200);
  ArduinoSerial.begin(115200);
}

void loop() {

    /*
   // Read data temperature and pressure from BMP180 sensor
   room_temp = bmp.readTemperature();
   room_press = bmp.readPressure();

    // Send data to Node MCU
    kirim_NodeMCU();
   
   Serial.println("DATA ROOM DAQ");
   Serial.print("Room Temperature = ");
   Serial.println(room_temp);
   Serial.print("Room Press   = ");
   Serial.println(room_press);
   Serial.println("========================================================");
   //delay(10000);
   */
   
      

   // Xbee read packet from another tx Xbee
   xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      Serial.println("available");
      
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
        // got a rx packet
        Serial.println("gotpacket");

        if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
          xbee.getResponse().getRx16Response(rx16);
          option = rx16.getOption();
          data = rx16.getData(0);
          Serial.println("Series1receive");
          
        } else {
                xbee.getResponse().getRx64Response(rx64);
          option = rx64.getOption();
          data = rx64.getData(0);
          Serial.println("Series2receive");
        }
    
    address = rx16.getRemoteAddress16(); // Get Adress of XBee Transmitter
    Serial.println(address);
    
    // Data ROOM DAQ
    if (address == 0x4444) { // Get adress of Room DAQ XBee Tx
    // Receiving and printing a float Temperature
    Serial.println("DATA ROOM DAQ");
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i);
    }
    room_temp = u.f;
    Serial.print("room_temp = ");
    Serial.println(room_temp);
       
    // Receiving and printing a float Pressure
    for (int i=0;i<4;i++){
      u.b[i]=rx16.getData(i+4);
    }
    room_press = u.f;

    //Send data to node MCU
    
    //delay(100);
        
    Serial.print("room_press   = ");
    Serial.println(room_press);
    Serial.print("Address   = 0x");
    Serial.println(address, HEX);
    Serial.println("========================================================");
    kirim_NodeMCU();
    
    } 
    /*    
    // Get Data of Visitor Counter
     if (address == 0x4444){  // Adress of Visitor Counter Xbee Tx
    // Receiving and printing a counter of people
    Serial.println("DATA VISITOR COUNTER");
    payload[0]=rx16.getData(0);
    visitors = payload[0];

    // Send data to Node MCU
    kirim_NodeMCU();
    //delay(100);

    
    Serial.print("JmlOrang  = ");
    Serial.println(visitors);
    Serial.print("Address   = 0x ");
    Serial.println(address, HEX);
    Serial.println("========================================================");
    }*/
       
    // Get Data of Outdoor AC and Indoor ACDAQ 
     else if (address == 0x5555){  // Adress of Outdoor AC DAQ Xbee Tx
    // Receiving and printing a Tempperature Ambient

    if (rx16.getDataLength()>20){
        Serial.println("DATA INDOOR AC DAQ");
        for (int i=0;i<4;i++){
          u.b[i]=rx16.getData(i);
        }
        inflow_temp = u.f;
        Serial.print("inflow_temp  = ");
        Serial.println(inflow_temp);
        
        // Receiving and printing a Current of Compressor
        for (int i=0;i<4;i++){
          u.b[i]=rx16.getData(i+4);
        }
        inflow_humid = u.f;
        Serial.print("inflow_humid   = ");
        Serial.println(inflow_humid);
        
        // Receiving and printing a Temperature of SUction Line
        for (int i=0;i<4;i++){
          u.b[i]=rx16.getData(i+8);
        }
        outflow_temp = u.f;
        Serial.print("outflow_temp   = ");
        Serial.println(outflow_temp);
        
        // Receiving and printing a Pressure of Suction Line
        for (int i=0;i<4;i++){
          u.b[i]=rx16.getData(i+12);
        }
        outflow_humid = u.f;
        Serial.print("outflow_humid   = ");
        Serial.println(outflow_humid);
    
        // Receiving and printing a Pressure of Liquid Line
        for (int i=0;i<4;i++){
          u.b[i]=rx16.getData(i+16);
        }
        ev_inlet_temp = u.f;
        Serial.print("ev_inlet_temp  = ");
        Serial.println(ev_inlet_temp);
    
        
        // Receiving and printing a Pressure of Liquid Line
        for (int i=0;i<4;i++){
          u.b[i]=rx16.getData(i+20);
        }
        ev_outlet_temp = u.f;
        Serial.print("ev_outlet_temp  = ");
        Serial.println(ev_outlet_temp);
    
        for (int i=0;i<4;i++){
          u.b[i]=rx16.getData(i+24);
        }
        airflow_speed = u.f;
        Serial.print("airflow_speed  = ");
        Serial.println(airflow_speed);
        Serial.print("Address   = 0x ");
        Serial.println(address, HEX);
        Serial.println("========================================================");
        kirim_NodeMCU();

    }
    else{
        Serial.println("DATA OUTDOOR AC DAQ");
        // Receiving and printing a Pressure of Liquid Line
        for (int i=0;i<4;i++){
          u.b[i]=rx16.getData(i);
        }
        outdoor_temp = u.f;
        Serial.print("outdoor_temp  = ");
        Serial.println(outdoor_temp);
    
        // Receiving and printing a Pressure of Liquid Line
        for (int i=0;i<4;i++){
          u.b[i]=rx16.getData(i+4);
        }
        con_inlet_temp = u.f;
        Serial.print("con_inlet_temp  = ");
        Serial.println(con_inlet_temp);
    
        // Receiving and printing a Pressure of Liquid Line
        for (int i=0;i<4;i++){
          u.b[i]=rx16.getData(i+8);
        }
        con_outlet_temp = u.f;
        Serial.print("con_outlet_temp  = ");
        Serial.println(con_outlet_temp);
    
        // Receiving and printing a Pressure of Liquid Line
        for (int i=0;i<4;i++){
          u.b[i]=rx16.getData(i+12);
        }
        compressor_current = u.f;
        Serial.print("compressor_current  = ");
        Serial.println(compressor_current);
    
        // Receiving and printing a Pressure of Liquid Line
        for (int i=0;i<4;i++){
          u.b[i]=rx16.getData(i+16);
        }
        suction_pressure = u.f;
        Serial.print("suction_pressure = ");
        Serial.println(suction_pressure);      
        Serial.print("Address   = 0x ");
        Serial.println(address, HEX);
        Serial.println("========================================================");
        kirim_NodeMCU();
    
    }


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

void kirim_NodeMCU(){
        
        // Pengiriman data waktu dari GPS berupa jam, menit, detik, tanggal, bulan, taun
        ArduinoSerial.print(room_temp); ArduinoSerial.print('\n');
        ArduinoSerial.print(room_press); ArduinoSerial.print('\n'); 
        ArduinoSerial.print(visitors); ArduinoSerial.print('\n'); 
        ArduinoSerial.print(inflow_temp); ArduinoSerial.print('\n'); 
        ArduinoSerial.print(inflow_humid); ArduinoSerial.print('\n'); 
        ArduinoSerial.print(outflow_temp); ArduinoSerial.print('\n'); 
        ArduinoSerial.print(outflow_humid); ArduinoSerial.print('\n'); 
        ArduinoSerial.print(compressor_current); ArduinoSerial.print('\n'); 
        ArduinoSerial.print(suction_pressure); ArduinoSerial.print('\n'); 
        ArduinoSerial.print(outdoor_temp); ArduinoSerial.print('\n'); 
        ArduinoSerial.print(compressor_temp); ArduinoSerial.print('\n'); 
        ArduinoSerial.print(ev_inlet_temp); ArduinoSerial.print('\n'); 
        ArduinoSerial.print(ev_outlet_temp); ArduinoSerial.print('\n'); 
        ArduinoSerial.print(con_inlet_temp); ArduinoSerial.print('\n'); 
        ArduinoSerial.print(con_outlet_temp); ArduinoSerial.print('\n'); 
        ArduinoSerial.print(airflow_speed); ArduinoSerial.print('\n');   
 }

