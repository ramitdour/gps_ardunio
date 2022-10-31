#include <SoftwareSerial.h>
SoftwareSerial ss(4, 5);


// D1 to Rx of gps
// D2 to Tx of Gps
//



void setup() {
    Serial.begin(115200);
  ss.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");

}

void loop() {
  // put your main code here, to run repeatedly:

   while (ss.available() > 0){

      Serial.write(ss.read());
    }

}
