//Horizen Avionics V0.9
//Uses a micro SD Card adapter and a BMP390
//Created by Liam Gessman
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
//set to your local pressure (zeros out altitude so doesn't really matter)
#define SEALEVELPRESSURE_HPA (1013.25)
//chip select for your SD card reader
const int chipSelect = 10;

const int mainChute = 6;
const int drougeChute = 7;
const int 

const int mainDeploy = 300;

float packet[6];
//data packet array
float cur;
float prev;
//altitude variables for distance and apogee
float altZ;
//altitude zero
float sum;
//averaged altitude
float apogee = 0;

float speed;
float tim;
float dis;
//for speed
int r;
int i;
int a;

int dig[4];

//for for loops
int samp = 10;
//change this for number of samples for altitude average
int average();
//averaging function


Adafruit_BMP3XX bmp;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Horizen OS V0.9");
  delay(500);
  if(!bmp.begin_I2C()) {
    Serial.println("BMP390 Not found!");
    while(true);
  }
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_100_HZ);

  //tests sd card and BMP390
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
  //zeros altitude measure ment
  Serial.println("Zeroing Altitude...");
  for (int i = 1; i<samp; i++){
     altZ = bmp.readAltitude(SEALEVELPRESSURE_HPA)*3.28;
     prev = 0;
  }

  Serial.println("Time(sec)  Temp(C)  Pressure(HPA)  Altitude (F) ");
  pinMode(drougeChute, OUTPUT);
  pinMode(mainChute, OUTPUT);
  pinMode(BUILTIN_LED)
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
int Pulse(int limit){


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
  if(cur < apogee && cur > 1000)
{
  //digitalWrite(drougeChute, HIGH);
  //delay(2000);
  //digitalWrite(drougeChute, LOW);
}
  if(cur < apogee && cur < mainDeploy)
  {
  //digitalWrite(mainChute, HIGH);
  //delay(2000);
  //digitalWrite(mainChute, LOW);
  }
 File dataFile = SD.open("datalog.txt", FILE_WRITE);

  packet[0]=tim;
  packet[1]=(bmp.temperature * 9/5) + 32;
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
