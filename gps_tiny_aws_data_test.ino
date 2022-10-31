
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



void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.setDebugOutput(true);

   ss.begin(GPSBaud);

}

void loop() {
  // put your main code here, to run repeatedly:

  String toAwsData = "";
    toAwsData = toAwsData + "{" +
                "\"lat\":" + (gps.location.isValid() ?  "\""+ String(gps.location.lat(),7) + "\"," : "null,") +
                "\"lng\":" + (gps.location.isValid() ? "\""+String(gps.location.lng(),7) + "\"," : "null,") +
                "\"alt\":" + (gps.altitude.isValid() ? "\""+String(gps.altitude.meters(),7)+ "\"," : "null,") +
                "\"crs\":" + (gps.course.isValid() ? "\""+String(gps.course.deg())+ "\"," : "null,") +
                "\"spd\":" + (gps.speed.isValid() ? "\""+String(gps.speed.kmph(),3)+ "\"" : "null") +

                "}";

 Serial.println(toAwsData);

   smartDelay(1000);

}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
