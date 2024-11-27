#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#define SEALEVELPRESSURE_HPA (1013.25)
const int chipSelect = 10;

float packet[6];
float cur;
float prev;
float altZ;
float sum;
float apogee = 0;
float speed;
float tim;
float dis;
int i;
int a;
int samp = 10;
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
  bmp.setOutputDataRate(BMP3_ODR_100_HZ);
  Serial.print("Sampling...");
  if(!bmp.performReading()){
    Serial.println("Sample Failed! Check wiring and reset chip");
    return;
  }
  Serial.print("initializing SD...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed! Check if inserted");
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

String arrayToString(float *array, int size) {
  String result = "";

  for (int i = 0; i < size; i++) {
    result += String(array[i]);
    if (i < size - 1) {
      result += ","; // Add comma separator except for the last element
    }
  }

  return result;
}

void loop() {
  for(int a = 1; a < samp; a++){
    sum = sum + bmp.readAltitude(SEALEVELPRESSURE_HPA)*3.28 -altZ;
  }
  sum = sum / samp;
  prev = cur;
  cur = sum;

  tim = tim + .1;
  dis = cur - prev;
  speed = dis / tim;


  if(cur > apogee){
  apogee = cur;  
 }
 File dataFile = SD.open("datalog.txt", FILE_WRITE);

  packet[0]=tim;
  packet[1]=bmp.temperature;
  packet[2]=bmp.pressure / 100.0;
  packet[3]=sum;
  packet[4]=apogee;
  packet[5]=speed;
  int arraySize = sizeof(packet) / sizeof(packet[0]);

  String result = arrayToString(packet, arraySize);

  if(dataFile){
    dataFile.println(result);
    Serial.println(result);
    dataFile.close();
  }

  delay(10);
  sum = 0;
}
