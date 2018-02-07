#include <Printers.h>
#include <XBee.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

Adafruit_BMP085 bmp;
volatile int sleep_count = 0;
//const int interval = 1;
const int sleep_total = 8;

//create the XBee object
//XBee xbee = xbee();   

//to send two floats of 4 bytes each
//uint8_t payload[8] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

// Union to convery float to byte string
/*union u_tag
{
  uint8_t b[4];
  float fval
} u;

// Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x00000000);
ZBTxRequest Tx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse(); */

void goToSleep()   
{
// The ATmega328 has five different sleep states.
// See the ATmega 328 datasheet for more information.
// SLEEP_MODE_IDLE -the least power savings 
// SLEEP_MODE_ADC
// SLEEP_MODE_PWR_SAVE
// SLEEP_MODE_STANDBY
// SLEEP_MODE_PWR_DOWN -the most power savings
// I am using the deepest sleep mode from which a
// watchdog timer interrupt can wake the ATMega328

set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode.
sleep_enable(); // Enable sleep mode.
sleep_mode(); // Enter sleep mode.
// After waking from watchdog interrupt the code continues
// to execute from this point.

sleep_disable(); // Disable sleep mode after waking.
                     
}

void watchdogOn() {
  
// Clear the reset flag, the WDRF bit (bit 3) of MCUSR.
MCUSR = MCUSR & B11110111;
  
// Set the WDCE bit (bit 4) and the WDE bit (bit 3) 
// of WDTCSR. The WDCE bit must be set in order to 
// change WDE or the watchdog prescalers. Setting the 
// WDCE bit will allow updtaes to the prescalers and 
// WDE for 4 clock cycles then it will be reset by 
// hardware.
WDTCSR = WDTCSR | B00011000; 

// Set the watchdog timeout prescaler value to 1024 K 
// which will yeild a time-out interval of about 8.0 s.
WDTCSR = B00100001;

// Enable the watchdog timer interupt.
WDTCSR = WDTCSR | B01000000;
MCUSR = MCUSR & B11110111;

}

ISR(WDT_vect)
{
sleep_count ++; // keep track of how many sleep cycles
// have been completed.
}

void setup() 
{
  bmp.begin();
  //xbee.begin(9600);
  Serial.begin(9600);
   // Turn on the watchdogOn
  watchdogOn();
  
  // Disable the ADC by setting the ADEN bit (bit 7) to zero.
  ADCSRA = ADCSRA & B01111111;

  // Disable the analog comparator by setting the ACD bit
  // (bit 7) to one.
  ACSR = B10000000;

  // Disable digital input buffers on all analog input pins
  // by setting bits 0-5 to one.
  DIDR0 = DIDR0 | B00111111;
 
 /* xbee.setSerial(Serial);
 // Turn on the watchdogOn */
  
  
  /* Disable the ADC by setting the ADEN bit (bit 7) to zero. */
  ADCSRA = ADCSRA & B01111111;

  /* Disable the analog comparator by setting the ACD bit
  // (bit 7) to one. */
  ACSR = B10000000;

  /* Disable digital input buffers on all analog input pins
  // by setting bits 0-5 to one. */
  DIDR0 = DIDR0 | B00111111;
  
}


void loop() 
{
  goToSleep();
  if (sleep_count != sleep_total)
  {
  float T = bmp.readTemperature();
  float P = bmp.readPressure ();
  
  Serial.println("Hasil Pembacaan Sensor"); 
  delay(3000);
  Serial.println("Room DAQ");
  Serial.print("Temperature = ");
  Serial.print(T);
  Serial.println(" C");
  Serial.print("Pressure = ");
  Serial.print(P);
  Serial.println(" Pa");
  Serial.println(" ");
  delay(2000);

//Convert Data to string
  Serial.println("Convert Data To String.....");
  delay(3000);
  Serial.print("T");
  Serial.print(T);
  Serial.print(",");
    
  Serial.print("P");
  Serial.print(P);
  Serial.print(",");
  Serial.println(" ");
  delay(3000);
  
  Serial.println("Send to ZigBee ");
  Serial.println(" ");
  /*
  // Convert Temperature into a byte array and copy it into the payload array
  u.fval = T ;
  for (int i=0; i<4; i++)
  {
    payload[i]=u.b[i];
  }

  // Convert Pressure into a byte array and copy it into the payload array
  u.fval = P ;
  for (int i=0; i<4; i++)
  {
    payload[i]=u.b[i];
  }

  xbee.send(Tx);
  */
  }
  else
  {

  }
  delay(5000);
}



