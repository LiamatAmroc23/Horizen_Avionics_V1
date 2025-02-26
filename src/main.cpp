//Horizen Avionics V0.9
//Uses a micro SD Card adapter and a BMP390
//Created by Liam Gessman
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Adafruit_MPU6050.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
//set to your local pressure (zeros out altitude so doesn't really matter)
#define SEALEVELPRESSURE_HPA (1013.25)

static const int RXPin = 0, TXPin = 1;
static const uint32_t GPSBaud = 9600;
//chip select for your SD card reader
const int chipSelect = 10;

const int mainChute = 6;
const int drougeChute = 7;

const int mainDeploy = 300;

float packet[15];
//data packet array
float splt[5];
float nsplt[5];

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
//mpu integers
float gx;
float gy;
float gz;
float ax;
float ay;
float az;

float lat;
float lon;
int sat;

int dig[4];

//for for loops
int samp = 10;
//change this for number of samples for altitude average
int average();
int pulse();
int split(int pul); 
int pack();
int data();
//averaging function


Adafruit_BMP3XX bmp;
Adafruit_MPU6050 mpu;
TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(GPSBaud);
  delay(3000);
  while(!Serial);
  Serial.println("Horizen OS V1.5");
  delay(500);
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    return;
  }
  if(!bmp.begin_I2C()) {
    Serial.println("BMP390 Not found!");
    while(true);
  }
  if(!mpu.begin()){
    Serial.println("Failed to find MPU6050!");
    while(true);
  }
  delay(5000);

  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_100_HZ);

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

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
}

String arrayToString(float *array, int size) {
  String result = "";
  for (int i = 0; i < size; i++) {
      result += String(array[i], 6); // Ensure 6 decimal places
      if (i < size - 1) {
          result += ","; // Add comma separator except for the last element
      }
  }
  return result;
}


int split(int pul){
  
  int n = 0;
  int r = pul;
  int t = pul;
  int digits = 0; while (t != 0) { t /= 10; digits++; }
    while(r != 0){
      splt[n] = r % 10;
      r = r / 10;
      n += 1;
    }
    n = 0;
    while(digits != 0){
      nsplt[t] = splt[n];
      Serial.println(nsplt[t]);
      t -= 1;
      n += 1;

    }


}
int pulse(){

}
int pack(){

}
int data(){

}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
}

  //split(1234);
  
  for(int a = 1; a < samp; a++){
    sum = sum + bmp.readAltitude(SEALEVELPRESSURE_HPA)*3.28 -altZ;
  }
  sum = sum / samp;
  prev = cur;
  cur = sum;

  tim = tim + .1;
  dis = cur - prev;
  speed = dis / tim;

  gx = g.gyro.x;
  gy = g.gyro.y;
  gz = g.gyro.z;
  ax = a.acceleration.x;
  ay = a.acceleration.y;
  az = a.acceleration.z;
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000) {
    lastUpdate = millis();
    if (gps.location.isUpdated()){
      lat = (gps.location.lat());
      lon = (gps.location.lng());
    } else {
      lat = -1;
      lon = -1;
    }
    if (gps.satellites.isUpdated()){
      sat = (gps.satellites.value());
    }
  }
    

  

  


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
  packet[6]=gx;
  packet[7]=gy;
  packet[8]=gz;
  packet[9]=ax;
  packet[10]=ay;
  packet[11]=az;
  packet[12]=(lat);
  packet[13]=(lon);
  packet[14]=sat;

  int arraySize = sizeof(packet) / sizeof(packet[0]);

  String result = arrayToString(packet, arraySize);

  if(dataFile){
    //dataFile.println(result);
    Serial.println(result);
    dataFile.close();
  }

  delay(10);
  sum = 0;
}
