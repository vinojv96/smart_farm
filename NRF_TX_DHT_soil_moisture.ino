/*
************************************verified*************VinoJV*************
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "DHT.h"
#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22 
const int AirValue = 620;   //you need to replace this value with Value_1
const int WaterValue = 290;  //you need to replace this value with Value_2
int intervals = (AirValue - WaterValue)/3;
int soilMoistureValue = 0;

RF24 radio(8, 10); // CE, CSN
const byte address[6] = "00001";
struct package
{
  float temperature ;
  float humidity ;
  float moisture ;
};
typedef struct package Package;
Package data;
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  Serial.begin(9600);
  dht.begin();
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.openWritingPipe(address);
  radio.stopListening();
  Serial.println(F("Ready"));
  delay(1000);
}

void loop() {
  readSensor();
  delay(1000);
  readSoilSensor();
 Serial.println(data.humidity);
 Serial.println(data.temperature);
 Serial.println(data.moisture);
 radio.write(&data, sizeof(data)); 
  delay(60000);
}
void readSensor()
{
  data.humidity = dht.readHumidity();
  data.temperature = dht.readTemperature();
  }
void readSoilSensor()
{
  soilMoistureValue = analogRead(A0);
   Serial.println(soilMoistureValue);
   data.moisture = soilMoistureValue;
    if(soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
      {
         Serial.println("Very Wet");
      }
    else if(soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
      {
        Serial.println("Wet");
      }
    else if(soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
      {
      Serial.println("Dry");
      }
 }
