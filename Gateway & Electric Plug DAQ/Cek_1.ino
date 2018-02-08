const int analogIn = A0;              // inisialisasi pin A0 sebagai pembacaan sensor
float sensor_Value = 0;               // setting nilai default pembacaan sensor 0
float v_Out;                          // nilai tegangan output
        
void setup() {
  Serial.begin (9600);                // inisialisasi komunikasi serial pada 960bps
}

void loop() {
  sensor_Value = analogRead (A0);     // read the analog in value:
  v_Out = 250 * (sensor_Value / 1024);// rumus perhitungan tegangan output
  // // Mencetak hasil ke monitor serial
  Serial.print ("vOut = ");
  Serial.print(v_Out);
  Serial.println("V");
  // diberikan nilai tunda 1 s untuk pembacaan berikutnya
  // Menampilkan pembacaan analog-to-digital converter setelah pembacaan terakhir
  delay (1000);
}

