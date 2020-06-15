
/* DHT22 temperature and humidity data are uploaded to thingspeak using
 * WeMOS ESP8266 MCU.
 * 
 * http://embedded-lab.com/blog/esp8266-temperature-humidity-logger-deep-sleep-enabled/
 * small changes by Nicu FLORICA (niq_ro)
 */
 
#include <ESP8266WiFi.h>
//#include <DHT.h>
#include "DHTesp.h" - https://www.arduinolibraries.info/libraries/dht-sensor-library-for-es-px
#include "Math.h"
#define DHTTYPE DHT22
#define DHTPIN  4   // DHT22 data pin connects to D4

// Time to sleep (in seconds):
const int sleepTimeS = 600;  // sleep time in seconds
//uint32_t sleepTimeuS;        // sleep time in microseconds

//DHT dht(DHTPIN, DHTTYPE, 18); // 11 works fine for ESP8266
DHTesp dht;

float prevTemp = 0;
const char* server = "api.thingspeak.com";
String apiKey ="Api Key";
const char* MY_SSID = "ssid name"; 
const char* MY_PWD = "ssid password";
int sent = 0;

  float temperature,humidity;
  float temperature0,humidity0;
  
void setup() {
  delay(3000);
  Serial.begin(115200);
//  sleepTimeuS = sleepTimeS * 1000000; 
//  dht.begin();
dht.setup(DHTPIN, DHTesp::DHTTYPE); // Connect DHT sensor to GPIOxx
  connectWifi();
  
}

void loop() {

  //char buffer[10];
  /*
  humidity = dht.readHumidity();          // Read humidity (percent)
  t1 = dht.readTemperature(true);     // Read temperature as Fahrenheit
    // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(t1)) {
     Serial.println("Failed to read from DHT sensor!");
      return;
  }
  */

 humidity = dht.getHumidity();
 temperature = dht.getTemperature();
// t = t-1.7; // correction

if (isnan(humidity) || isnan(temperature)) {
Serial.println("Failed to read from DHT sensor!");
temperature = temperature0;
humidity = humidity0;
return;
}
  
  Serial.print(String(sent)+" Temperature: ");
  Serial.println(temperature);
  Serial.print(String(sent)+" Humidity: ");
  Serial.println(humidity);
  sendTeperatureTS(temperature, humidity);
  Serial.println("deepsleep");
  ESP.deepSleep(sleepTimeS * 1000000);  //deepsleep defination 
//  ESP.deepSleep(sleepTimeS*1000000, WAKE_RF_DEFAULT); // Sleep for 60 seconds
//   ESP.deepSleep(sleepTimeuS, WAKE_RF_DEFAULT); // Sleep for 60 seconds 
   delay(100);
}

void connectWifi()
{
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");  

  temperature0 = temperature;
  humidity0 = humidity;
}//end connect

void sendTeperatureTS(float temp1, float temp2)
{  
   WiFiClient client;
  
   if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
   Serial.println("WiFi Client connected ");
   
   String postStr = apiKey;
   postStr += "&field1=";
   postStr += String(temp1);
   postStr += "&field2=";
   postStr += String(temp2);
   postStr += "\r\n\r\n";
   
   client.print("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n\n");
   client.print(postStr);
   delay(1000);
   
   }//end if
   sent++;
 client.stop();
}//end send
