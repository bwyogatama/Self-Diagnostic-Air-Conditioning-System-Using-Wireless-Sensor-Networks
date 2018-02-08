#include <XBee.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

const int analogIn = A0;              // inisialisasi pin A0 sebagai pembacaan sensor
float sensor_Value = 0;               // setting nilai default pembacaan sensor 0
float v_Out;                          // nilai tegangan output
float voltage;
// This variable is made volatile because it is changed inside
// an interrupt function
volatile int sleep_count = 0;         // Keep track of how many sleep cycles have been completed.
const int interval = 1;               // Interval in minutes between waking and doing tasks.
const int sleep_total = (interval*60)/8;            //Approximate number of sleep cycles 
                                      //needed before the interval defined above elapses. Not that this does integer math.


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
       
void setup() {
  Serial.begin (9600);                // inisialisasi komunikasi serial pada 960bps
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
  
}


void loop() {
  
  goToSleep();
  if (sleep_count == sleep_total)
  {
    sleep_count = 0;
    sensor_Value = analogRead (A0);     // read the analog in value:
    v_Out = (sensor_Value / 1023) *5;// rumus perhitungan tegangan output
    // // Mencetak hasil ke monitor serial
    voltage = (v_Out+1.78)/0.001;
    Serial.print ("vOut = ");
    Serial.print(v_Out);
    Serial.println(" V");
    Serial.print ("voltage = ");
    Serial.print(voltage);
    Serial.println(" V");
  }
    else
    {
      
    }
    delay (1000);
}


