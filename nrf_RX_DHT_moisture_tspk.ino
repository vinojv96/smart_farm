/*
 * ********************verified****************VinoJV***************************
 */
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <Wire.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
RF24 radio(2, 4); // CE, CSN
//----------- credentials ----------------------
String apiKey = "your api key";
const char* ssid = "your SSID";
const char* password = "your_password";
WiFiClient client;
int gatewayID = EEPROM.read(0);
const char* server = "api.thingspeak.com";
const byte address[6] = "00001";
float remoteHumidity = 0.0;
float remoteTemperature = 0.0;
float remoteMoisture;
//------------------data package----------------
struct package
{
  float temperature ;
  float humidity ;
  float moisture ;
};
typedef struct package Package;
Package data;

void setup() {
  pinMode(5, OUTPUT);
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(data));
    remoteTemperature = data.temperature;
    remoteHumidity = data.humidity;
    remoteMoisture = data.moisture;
    float moisture_level = data.moisture;
    if(moisture_level>500)
    {
    digitalWrite(5, LOW);
    delay(1000);
    }
    else digitalWrite(5, HIGH);
    
//-------------------------------------------------------  
    Serial.print("\nPackage:");
    Serial.print("\n");
    Serial.println(data.temperature);
    Serial.println(data.humidity);
    Serial.println(moisture_level);
    Serial.println("--- Data retrieved from device ---");
//-----------------------------------------------------------------------------
      if (client.connect(server, 80)) { // "184.106.153.149" or api.thingspeak.com
        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(data.temperature);
        postStr += "&field2=";
        postStr += String(data.humidity);
        postStr += "&field3=";
        postStr += String(moisture_level);
        postStr += "\r\n\r\n";
//----------------------- Posting Data------------------------------------------------   
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);
        delay(500);
//------------------------ Showing Data Sent to Cloud ---------------------------------    
        Serial.println("---- Data sent to Thingspeak ----");
        Serial.print("Potato Field: ");
        Serial.print(", Temperature:");
        Serial.print(data.temperature);
        Serial.print(", Humidity:");
        Serial.print(data.humidity);
        Serial.print(", Moisture:");
        Serial.print(moisture_level);
        
      }
      client.stop();
    }
   else {
    Serial.println("no msg");
  }
  // delay is due to ThingSpeak's limitations
  delay(60000);

}
