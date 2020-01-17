#include "BluetoothSerial.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
  
BluetoothSerial blueTooth;

//int measurePin = 6;
int measurePin = 2;
int ledPower = 12;
 
int samplingTime = 280;
int deltaTime = 40;
 
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

long previousMillis=0;
long interval=5000;
int count=0;
int headState=0;
 
void setup(){
  Serial.begin(38400);
  pinMode(ledPower,OUTPUT);
  blueTooth.begin("ESP32Bluetooth");         // Initializing bluetooth with Bluetooth device name "ESP32Bluetooth"

  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  accel.setRange(ADXL345_RANGE_16_G); 
}
 
void loop(){
  delayMicroseconds(samplingTime);
 
  voMeasured = analogRead(measurePin); // read the dust value 
  delayMicroseconds(deltaTime);
 
  // 0 - 3.3V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (3.3 / 1024);
 
  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = 0.17 * calcVoltage - 0.1;
 
  Serial.print("Raw Signal Value (0-1023): ");
  Serial.print(voMeasured);
 
  Serial.print(" - Voltage: ");
  Serial.print(calcVoltage);
 
  Serial.print(" - Dust Density: ");
  Serial.println(dustDensity);
 
  delay(200);
  sensors_event_t event; 
  accel.getEvent(&event);
  
  Serial.print(voMeasured); Serial.print(" ");
  Serial.print(dustDensity); Serial.print(" ");

  unsigned long currentMillis = millis();
  if(event.acceleration.x<-6) {
    //Checking Node up movement of head 
    headState=1;
  }
  else if(event.acceleration.x>-6) {
    //Checking Node down movement of head 
    if(headState==1){
      headState=0;
      count++;
    }
  }

  if(currentMillis - previousMillis > interval){
    previousMillis = currentMillis;
    blueTooth.print(voMeasured); blueTooth.print(","); 
    if(count>2){
      Serial.print("Yes..");    //Pothole reported
      blueTooth.print("Yes,");
    }
    else{
      Serial.print("No..");     //No Pothole reported
      blueTooth.print("No,"); 

    }
    count=0;
  }
  
  Serial.println();
}
