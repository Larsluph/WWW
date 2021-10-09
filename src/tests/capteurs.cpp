#include <SoftwareSerial.h>
#include <Wire.h>
#include "DS1307.h"
#include <SPI.h>
#include <SD.h>
 
const int chipSelect = 4; // (SD card reader model)
const int analogPin=2; // Luminosity captor on A2 port on Grove Shield
SoftwareSerial SoftSerial(2, 3); // Serial already used for serial communication GPS connected on D2 port on Grove Shield
DS1307 clock;//define a object of DS1307 class RTC Clock on I2C port on Grove Shield

String gpsData;
bool t;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  SoftSerial.begin(9600); // Open SoftwareSerial for GPS

  //Initialize Clock
  clock.begin();
  clock.fillByYMD(2013,1,19);//Jan 19,2013
  clock.fillByHMS(15,28,30);//15:28 30"
  clock.fillDayOfWeek(SAT);//Saturday
  clock.setTime();//write time to the RTC chip

}

void loop() {
  // make a string for assembling the data to log:
  String dataString = "";
  // Read time from RTC CLock
  dataString+=getTime() + " ; ";
  
  // Luminosity captor reading
  int sensor = analogRead(analogPin);
  dataString += String(sensor);
  dataString += " ; ";
  
  // GPS Reading
  gpsData="";
  if (SoftSerial.available())                     // if data is coming from software serial port ==> data is coming from SoftSerial GPS
  {
      t=true;
      while(t){
        gpsData = SoftSerial.readStringUntil('\n');
        if (gpsData.startsWith("$GPGGA",0)){
          t=false; 
        }
      }
  }
  dataString += gpsData;
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}

String getTime()
{
    String time="";
    clock.getTime();
    time+=String(clock.hour, DEC);
    time+=String(":");
    time+=String(clock.minute, DEC);
    time+=String(":");
    time+=String(clock.second, DEC);
    time+=String("  ");
    time+=String(clock.month, DEC);
    time+=String("/");
    time+=String(clock.dayOfMonth, DEC);
    time+=String("/");
    time+=String(clock.year+2000, DEC);
    time+=String(" ");
    time+=String(clock.dayOfMonth);
    time+=String("*");
    switch (clock.dayOfWeek)// Friendly printout the weekday
    {
        case MON:
        time+=String("MON");
        break;
        case TUE:
        time+=String("TUE");
        break;
        case WED:
        time+=String("WED");
        break;
        case THU:
        time+=String("THU");
        break;
        case FRI:
        time+=String("FRI");
        break;
        case SAT:
        time+=String("SAT");
        break;
        case SUN:
        time+=String("SUN");
        break;
    }
    time+=String(" ");
    return time;
}
