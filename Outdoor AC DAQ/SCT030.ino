// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;                   // Create an instance

#define A_SENSOR_ANALOG_PIN 2

void setup()
{  
  Serial.begin(9600);
  emon1.current(A_SENSOR_ANALOG_PIN, 29.0);             // Current: input pin, calibration.
}

void loop()
{
  double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  //float Irms = emon1.Irms;
  //Perhitungan Irms
  Serial.print("Arus : ");
  Serial.print(Irms); // Irms
  Serial.println("  A");

  delay(2000);
}
