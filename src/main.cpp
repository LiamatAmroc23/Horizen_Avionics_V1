#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Wire.h>

#define SEALEVELPRESSURE_HPA (1013.25)

float cur;
float prev;
float altZ;
float sum;
float apogee = 0;
int i;
int a;
int samp = 100;
int average();

Adafruit_BMP3XX bmp;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Horizen OS V0.1");
  delay(500);
  if(!bmp.begin_I2C()) {
    Serial.println("BMP390 Not found!");
    while(true);
  }
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
  Serial.print("Sampling...");
  if(!bmp.performReading()){
    Serial.println("Sample Failed! Check wiring and reset chip");
    return;
  }
  delay(1000);
  Serial.println("Zeroing Altitude...");
  for (int i = 1; i<samp; i++){
     altZ = bmp.readAltitude(SEALEVELPRESSURE_HPA)*3.28;
     prev = 0;
  }
  Serial.println("Temp(C)  Pressure(HPA)  Altitude (M)");
}


void loop() {
  for(int a = 1; a < samp; a++){
    sum = sum + bmp.readAltitude(SEALEVELPRESSURE_HPA)*3.28 -altZ;
  }
  sum = sum / samp;
  prev = cur;
  cur = sum;
  if(cur > apogee){
  apogee = cur;  
 }
  Serial.print(bmp.temperature);
  Serial.print(",");
  Serial.print(bmp.pressure / 100.0);
  Serial.print(",");\
  Serial.print(sum);
  Serial.print(",");
  Serial.println(apogee);
  delay(1);
  sum = 0;
}
