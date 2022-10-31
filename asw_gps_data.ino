/*Developed by M V Subrahmanyam - https://www.linkedin.com/in/veera-subrahmanyam-mediboina-b63997145/
Project: AWS | NodeMCU ESP32 Tutorials
Electronics Innovation - www.electronicsinnovation.com

GitHub - https://github.com/VeeruSubbuAmi
YouTube - http://bit.ly/Electronics_Innovation

Upload date: 07 October 2019

AWS Iot Core

This example needs https://github.com/esp8266/arduino-esp8266fs-plugin

It connects to AWS IoT server then:
- publishes "hello world" to the topic "outTopic" every two seconds
- subscribes to the topic "inTopic", printing out any messages
*/

#include "FS.h"
#include <ESP8266WiFi.h>;
#include <PubSubClient.h>;
#include <NTPClient.h>;
#include <WiFiUdp.h>;

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
/*
   This sample code demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

// 
// D1 to Rx of gps
// D2 to Tx of Gps
//
static const int RXPin = 4, TXPin = 5;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);


// Update these with values suitable for your network.

const char *ssid = "skynet";
const char *password = "9806131711";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

const char *AWS_endpoint = "a3an4l5rg1sm5p-ats.iot.us-west-2.amazonaws.com"; // MQTT broker ip

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
WiFiClientSecure espClient;
PubSubClient client(AWS_endpoint, 8883, callback, espClient); // set MQTT port number to 8883 as per //standard
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  espClient.setBufferSizes(512, 512);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  while (!timeClient.update())
  {
    timeClient.forceUpdate();
  }

  espClient.setX509Time(timeClient.getEpochTime());
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("GPS_testing"))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      char buf[256];
      espClient.getLastSSLError(buf, 256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  setup_wifi();
  delay(1000);
  if (!SPIFFS.begin())
  {
    Serial.println("Failed to mount file system");
    return;
  }

  Serial.print("Heap: ");
  Serial.println(ESP.getFreeHeap());

  // Load certificate file
  File cert = SPIFFS.open("/cert.der", "r"); // replace cert.crt eith your uploaded file name
  if (!cert)
  {
    Serial.println("Failed to open cert file");
  }
  else
    Serial.println("Success to open cert file");

  delay(1000);

  if (espClient.loadCertificate(cert))
    Serial.println("cert loaded");
  else
    Serial.println("cert not loaded");

  // Load private key file
  File private_key = SPIFFS.open("/private.der", "r"); // replace private eith your uploaded file name
  if (!private_key)
  {
    Serial.println("Failed to open private cert file");
  }
  else
    Serial.println("Success to open private cert file");

  delay(1000);

  if (espClient.loadPrivateKey(private_key))
    Serial.println("private key loaded");
  else
    Serial.println("private key not loaded");

  // Load CA file
  File ca = SPIFFS.open("/ca.der", "r"); // replace ca eith your uploaded file name
  if (!ca)
  {
    Serial.println("Failed to open ca ");
  }
  else
    Serial.println("Success to open ca");

  delay(1000);

  if (espClient.loadCACert(ca))
    Serial.println("ca loaded");
  else
    Serial.println("ca failed");


    // gps set up
     ss.begin(GPSBaud);


  Serial.print("Heap: ");
  Serial.println(ESP.getFreeHeap());


  
}


String toAwsData = "";

String support_str = "";

 
void loop()
{


 
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000)
  {
    lastMsg = now;
    ++value;
    snprintf(msg, 75, "{\"message\": \"hello world #%ld\"}", value);
    Serial.print("Publish message: ");
//    Serial.println(msg);
//    client.publish("outTopic", msg);

    gps.location.isValid()  ? client.publish("outTopic", toAwsData.c_str()): Serial.println(toAwsData); 

    Serial.println(toAwsData);
    Serial.print("Heap: ");
    Serial.println(ESP.getFreeHeap()); // Low heap can cause problems
  }
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(100);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
  delay(100);                      // wait for a second


//
// static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
//
//  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
//  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
//  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
//  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
//  printInt(gps.location.age(), gps.location.isValid(), 5);
//  printDateTime(gps.date, gps.time);
//  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
//  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
//  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
//  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);

//  unsigned long distanceKmToLondon =
//    (unsigned long)TinyGPSPlus::distanceBetween(
//      gps.location.lat(),
//      gps.location.lng(),
//      LONDON_LAT, 
//      LONDON_LON) / 1000;
//  printInt(distanceKmToLondon, gps.location.isValid(), 9);
//
//  double courseToLondon =
//    TinyGPSPlus::courseTo(
//      gps.location.lat(),
//      gps.location.lng(),
//      LONDON_LAT, 
//      LONDON_LON);
//
//  printFloat(courseToLondon, gps.location.isValid(), 7, 2);
//
//  const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);
//
//  printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);
//
//  printInt(gps.charsProcessed(), true, 6);
//  printInt(gps.sentencesWithFix(), true, 10);
//  printInt(gps.failedChecksum(), true, 9);
//  Serial.println();

//String toAwsData = ""+
//
////  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
////  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
////  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
////  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
////  printInt(gps.location.age(), gps.location.isValid(), 5);
////  printDateTime(gps.date, gps.time);
////  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
////  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
////  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
////  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);
//
//  "satellites :"+( gps.satellites.isValid()? gps.satellites.value()  :  "NaN")+
//  "hdop :"+(gps.hdop.isValid() ?  gps.hdop.hdop():  "NaN")+
//  "lat :"+(gps.location.isValid()? gps.location.lat() :  "NaN")+
//  "lng :"+(gps.location.isValid()?  gps.location.lng() :  "NaN")+
//  "age :"+(gps.location.isValid()? gps.location.age() :  "NaN")+
////  "date :"+(?  :  "NaN")+
////  "time :"+(?  :  "NaN")+
//  "altitude :"+( gps.altitude.isValid() ? (gps.altitude.meters()  :  "NaN")+
//  " course:"+( gps.course.isValid() ?   gps.course.deg() :  "NaN")+
//  " speed:"+( gps.speed.isValid()? gps.speed.kmph()  :  "NaN")+
////  "course :"+( gps.course.isValid()?  :  "NaN")+
////  " :"+(?  :  "NaN")+
//// (gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ")+
//
// "";



    toAwsData = support_str + "{" +
                "\"lat\":" + (gps.location.isValid() ?  "\""+ String(gps.location.lat(),7) + "\"," : "null,") +
                "\"lng\":" + (gps.location.isValid() ? "\""+String(gps.location.lng(),7) + "\"," : "null,") +
                "\"alt\":" + (gps.altitude.isValid() ? "\""+String(gps.altitude.meters(),3)+ "\"," : "null,") +
                  "\"age\":" + (gps.location.isValid() ? "\""+String(gps.location.age()) + "\"," : "null,") +
                "\"crs\":" + (gps.course.isValid() ? "\""+String(gps.course.deg(),3)+ "\"," : "null,") +
                "\"spd\":" + (gps.speed.isValid() ? "\""+String(gps.speed.kmph(),2)+ "\"" : "null") +

                "}";
                
smartDelay(1000);

  

  

  if (millis() > 5000 && gps.charsProcessed() < 10)
    {Serial.println(F("No GPS data received: check wiring"));}
}





//// This custom version of delay() ensures that the gps object
//// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
//
//static void printFloat(float val, bool valid, int len, int prec)
//{
//  if (!valid)
//  {
//    while (len-- > 1)
//      Serial.print('*');
//    Serial.print(' ');
//  }
//  else
//  {
//    Serial.print(val, prec);
//    int vi = abs((int)val);
//    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
//    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
//    for (int i=flen; i<len; ++i)
//      Serial.print(' ');
//  }
//  smartDelay(0);
//}
//
//static void printInt(unsigned long val, bool valid, int len)
//{
//  char sz[32] = "*****************";
//  if (valid)
//    sprintf(sz, "%ld", val);
//  sz[len] = 0;
//  for (int i=strlen(sz); i<len; ++i)
//    sz[i] = ' ';
//  if (len > 0) 
//    sz[len-1] = ' ';
//  Serial.print(sz);
//  smartDelay(0);
//}
//
//static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
//{
//  if (!d.isValid())
//  {
//    Serial.print(F("********** "));
//  }
//  else
//  {
//    char sz[32];
//    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
//    Serial.print(sz);
//  }
//  
//  if (!t.isValid())
//  {
//    Serial.print(F("******** "));
//  }
//  else
//  {
//    char sz[32];
//    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
//    Serial.print(sz);
//  }
//
//  printInt(d.age(), d.isValid(), 5);
//  smartDelay(0);
//}
//
//static void printStr(const char *str, int len)
//{
//  int slen = strlen(str);
//  for (int i=0; i<len; ++i)
//    Serial.print(i<slen ? str[i] : ' ');
//  smartDelay(0);
//}
