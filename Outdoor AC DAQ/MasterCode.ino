// Nano_2_I2C_comm_master_OLED
// this Nano is the i2c master
// it governs the i2c OLED 128×32 display
// requests the slave temperatures
// slave sends temp floatwith two decimals coded as chars
// master decodes chars to float with atof function
// and displays with one decimal on 128×32 OLED
// 16 July 2017
// Floris Wouterlood
// public domain

#include <Wire.h>
#include <SPI.h>


float DS18B20_float = 0.0;
float DS18B20_01 = 0;
char t[10] = {}; // empty array for parking bytes received from slave
volatile int Val; // variable used by master to send data to slave

void setup() {

Wire.begin (); // join i2c bus
Serial.begin (9600); // start serial monitor
}

void loop() {

// get data from slave ==================================
Wire.requestFrom(8, 5); // request 5 bytes from slave device #8
int i = 0; // counter for each byte as it arrives

while (Wire.available()) {
t[i] = Wire.read(); // arriving characters parked in array t
i = i + 1;

}

// char array to float section ==========================
DS18B20_float = atof (t);
Serial.print ("temp = ");
Serial.print (DS18B20_float,2); // serial monityor print with 2 decimals
Serial.println (" *C");
delay(500); // time to relax

// send data to slave. here I am just sending ‘2’ ======
Val = 2;
Wire.beginTransmission (8);
Wire.write (Val);
Wire.endTransmission ();
}
