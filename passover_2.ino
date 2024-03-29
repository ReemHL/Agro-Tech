//Thingspeak:
// #include <ESP8266WiFi.h>
#include <WiFi.h>

#include "ThingSpeak.h"
unsigned long myChannelNumber = 1708074;
const char * myWriteAPIKey = "D6FJZQJT602R3XAA";

const char* ssid = ""; // your wifi SSID name
const char* password = "" ;// wifi pasword
 
const char* server = "api.thingspeak.com";

WiFiClient client;


//SHT31:
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();

//soil moisture:
const int dry = 2790; // value for dry sensor
const int wet = 954; // value for wet sensor
//soil moisture led:
const int LED = 13;      // the pin for the LED

void setup() {
  Serial.begin(9600);
  //Thingspeak and wifi:
  WiFi.disconnect();
  delay(10);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

    ThingSpeak.begin(client);
 
  WiFi.begin(ssid, password);
  
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("NodeMcu connected to wifi...");
  Serial.println(ssid);
  Serial.println();

  //SHT31:
    if (! sht31.begin(0x44)) {
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
  //soil moisture:included up
  //soil led:
  pinMode(LED, OUTPUT);

  
}
void loop() 
{
  
  //SHT31:
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (! isnan(t))  {
    Serial.print("Temp *C = "); Serial.println(t);
  }
  else  {
    Serial.println("Failed to read temperature");
  }
  
  if (! isnan(h))  {
  Serial.print("Hum. % = "); Serial.println(h);
  }
  else  {
  Serial.println("Failed to read humidity");
  }
  
  Serial.println();
  delay(40000);

  //VPD:
  int base1 = 10;
  float power1 = (7.5*t)/(237.3+t);
  float answer1 = pow(base1, power1);
  
  float P_sat = 610.7*answer1;
  float VPD_pascal = (P_sat*(100-h))/100;
  float VPD_kilo_pascal = VPD_pascal/1000;

  //soil moisture:
   int sensorVal = analogRead(A0);
   int percentageHumididy = map(sensorVal, wet, dry, 100, 0); 

  //soil led:
  if (percentageHumididy <= 10)  {
     digitalWrite(LED, HIGH);   // turn LED on
  }  else  {
  digitalWrite(LED, LOW);   // turn LED off
  }
 
   Serial.print(percentageHumididy);
   Serial.println("%");
   
   delay(40000);
  
  //Thingspeak:
  ThingSpeak.setField(1,t); // SHT temp
  ThingSpeak.setField(2,h); // SHT humidity
  ThingSpeak.setField(3,percentageHumididy); //soil moisture %
  ThingSpeak.setField(4,VPD_kilo_pascal); // VPD in kilopascal
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  Serial.println("uploaded to Thingspeak server....");

  client.stop();
 
  Serial.println("Waiting to upload next reading...");
  Serial.println();
  
  // thingspeak free needs minimum 15 sec delay between updates 
  // payed can do 1sec

 
  
  delay(40000);
}
