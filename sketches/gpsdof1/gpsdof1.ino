#include <SdFat.h>   //New SdFat library
SdFat SD;            //Add this line for backwards compatability with coding
//#include <Wire.h>
#include <RTClib.h>
//#include <SPI.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_LSM303.h>
//#include <Adafruit_LSM303_U.h>

#include <Adafruit_GPS.h>
//#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);

#define GPSECHO  true

const int chipSelect = 10;
RTC_DS1307 RTC;
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

/* Assign a unique ID to theDOF10 sensor at the same time */
Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);


void setup()  
{
   
   Serial.begin(9600);
   //Serial.begin(115200);
   
   while (!Serial) {
    //Serial.print("Waiting for serial port to connect...");
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Set up the SD card
  
  //Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  
  //Serial.println("card initialized.");

////Set up the gyroscope
//  Serial.println("Gyroscope Test"); Serial.println("");
//  
//  /* Enable auto-ranging */
//  gyro.enableAutoRange(true);
//  
//  /* Initialise the sensor */
//  if(!gyro.begin())
//  {
//    /* There was a problem detecting the L3GD20 ... check your connections */
//    Serial.println("Ooops, no L3GD20 detected ... Check your wiring!");
//    while(1);
//  }

  //Set up the accelerometer
  // Serial.println("Accelerometer Test"); Serial.println("");

  accel.enableAutoRange(true);

  /* Initialise the sensor */
 // if(!accel.begin())
 // {
    /* There was a problem detecting the ADXL345 ... check your connections */
    //Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
  //  while(1);
 // }



  Serial.begin(115200);
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  
  //Serial.println("Adafruit GPS library basic test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  
  // Turn on RMC and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(true);

  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);
  //Serial.println("Hello");
}

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

uint32_t timer = millis();
void loop()                     // run over and over again
{
  //Serial.println("Hello loop");
  DateTime now = RTC.now();
  sensors_event_t accel_event;
  accel.getEvent(&accel_event);

  String dataString = "";
//  dataString=String(gyro_event.gyro.x)+","+String(gyro_event.gyro.y)+","+String(gyro_event.gyro.z)+","+String(accel_event.acceleration.x)+","+String(accel_event.acceleration.y)+","+String(accel_event.acceleration.z);
  //dataString=String(accel_event.acceleration.x)+","+String(accel_event.acceleration.y)+","+String(accel_event.acceleration.z);  
  dataString=String(accel_event.acceleration.x);
  //Serial.println(dataString);
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("L3GD20_038.csv", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
      // log time
    //dataFile.print(now.unixtime()); // seconds since 1/1/1970
    //dataFile.print(", ");
    //dataFile.print('"');
    //dataFile.print(now.year(), DEC);
    //dataFile.print("/");
    dataFile.print(now.month(), DEC);
    //dataFile.print("/");
    dataFile.print(now.day(), DEC);
    //dataFile.print(" ");
    dataFile.print(now.hour(), DEC);
    //dataFile.print(":");
    dataFile.print(now.minute(), DEC);
    //dataFile.print(":");
    dataFile.print(now.second(), DEC);
    //dataFile.print('"');
    dataFile.print(',');
    dataFile.println(dataString);
    dataFile.close();
  }
  
  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
  //if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    //char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    //if (GPSECHO)
      //if (c) Serial.print(c);
  //}
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) { 
    timer = millis(); // reset the timer
    
    
//    Serial.print("\nTime: ");
//    Serial.print(GPS.hour, DEC); Serial.print(':');
//    Serial.print(GPS.minute, DEC); Serial.print(':');
//    Serial.print(GPS.seconds, DEC); Serial.print('.');
//    Serial.println(GPS.milliseconds);
//    
//    Serial.print("Date: ");
//    Serial.print(GPS.day, DEC); Serial.print('/');
//    Serial.print(GPS.month, DEC); Serial.print("/20");
//    Serial.println(GPS.year, DEC);
    
//    Serial.print("Fix: "); Serial.print((int)GPS.fix);
//    Serial.print(" quality: "); Serial.println((int)GPS.fixquality); 

    if (GPS.fix) {
//      Serial.print("Location: ");
//      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
//      Serial.print(", "); 
//      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
//      Serial.print("Location (in degrees, works with Google Maps): ");
//      Serial.print(GPS.latitudeDegrees, 4);
      dataFile.print(GPS.latitudeDegrees, 4);
//      Serial.print(", "); 
      dataFile.print(", "); 
//      Serial.println(GPS.longitudeDegrees, 4);
      dataFile.print(GPS.longitudeDegrees, 4);
      
      //Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      //Serial.print("Angle: "); Serial.println(GPS.angle);
//      Serial.print("Altitude: "); Serial.println(GPS.altitude);
//      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
  }
}
