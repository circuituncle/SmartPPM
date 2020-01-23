#include <ESP8266WiFi.h>

#include <WifiLocation.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

const char* ssid     = "Connectify-me";
const char* password = "zxcv.321";

const char* googleApiKey = "AIzaSyAG2C65n75bXvUpBUIOshu_iYASP-PN8Ho";
WifiLocation location(googleApiKey);

const char* host = "192.168.2.16";

int measurePin = 0;
 
int samplingTime = 280;
int deltaTime = 40;
 
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

long previousMillis=0;
long interval=50000;
int count=0;
int headState=0;
 
void setup(){
  Serial.begin(115200);

  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  accel.setRange(ADXL345_RANGE_16_G);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());

}

int value = 0;

void loop(){
  delayMicroseconds(samplingTime);
 
  voMeasured = analogRead(measurePin); // read the dust value
 
  delayMicroseconds(deltaTime);
 
  // 0 - 3.3V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (3.3 / 1024);
 
  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = 0.17 * calcVoltage;
 
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
  Serial.print(event.acceleration.x); Serial.print(" ");
  float tempx=event.acceleration.x;
  Serial.print(event.acceleration.y); Serial.print(" ");
  Serial.print(event.acceleration.z); Serial.println(" ");
  Serial.print("tempx=");
  Serial.println(tempx);

  delay(1000); 

  WiFiClient client;
  const int httpPort = 80;

    location_t loc = location.getGeoFromWiFi();
    Serial.println("Location request data");
    Serial.println(location.getSurroundingWiFiJson());
    Serial.println("Latitude: " + String(loc.lat, 7));
    Serial.println("Longitude: " + String(loc.lon, 7));
    Serial.println("Accuracy: " + String(loc.accuracy));
  unsigned long currentMillis = millis();


  delay(3000);
  Serial.print("tempx=");
  Serial.println(tempx);
  if(tempx<-7) {
    Serial.println("Up...............................................................");
    headState=1;
  }
  else if(tempx>-7) {
    Serial.println("Down............................................................");
    if(headState==1){
      headState=0;
      count++;
    }


  Serial.print("connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String s1 = "/wardi/addPotholeData2.php?p_id=2&lat=";
 // float s2 = lo
  String s3 = "&long=";

  String url2 = s1 + String(loc.lat,7) + s3 + String(loc.lon,7);

  client.print(String("GET ") + url2 + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: Keep-Alive\r\n\r\n");
  delay(200);
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  
  }

    
  }
  Serial.println(currentMillis - previousMillis);
  if(currentMillis - previousMillis > interval){
    previousMillis = currentMillis;
    if(count>2){
      Serial.print("Yes....................................................................");

  Serial.print("connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String s1 = "/wardi/addPotholeData2.php?p_id=2&lat=";
 // float s2 = lo
  String s3 = "&long=";

  String url2 = s1 + String(loc.lat,7) + s3 + String(loc.lon,7);

  client.print(String("GET ") + url2 + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: Keep-Alive\r\n\r\n");
  delay(200);
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  
  }

    }
    else{
      Serial.print("No..");

    }
    count=0;
  }
  
  Serial.println();

  ++value;
 
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

   String s5 = "/wardi/addPollutionData2.php?p_id=2&level=";
   String s6 = "&lat=";
  // float s2 = lo
    String s7 = "&long=";
  
   String url = s5 + String(dustDensity) +s6 + String(loc.lat,7) + s7 + String(loc.lon,7);

  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  delay(200);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: Keep-Alive\r\n\r\n");
  delay(200);
  
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  delay(200);
  

  Serial.println();
  Serial.println("closing connection");

}
