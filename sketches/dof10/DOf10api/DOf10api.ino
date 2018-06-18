/*ensure to install the SdFat Library from:
https://github.com/greiman/SdFat
*/
//#include <SD.h>    //Loose this in favour of SdFat Library for timestamp
#include <SdFat.h>   //New SdFat library
SdFat SD;            //Add this line for backwards compatability with coding
#include <Wire.h>
#include <RTClib.h>
#include <SPI.h>
//#include <Wire.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_LSM303.h>
//#include <Adafruit_LSM303_U.h>

char filename = "test.csv";

const int chipSelect = 10;
RTC_DS1307 RTC;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


/* Assign a unique ID to this sensor at the same time */
Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

void displaySensorDetails(void)
{
  sensor_t sensor;
  gyro.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" rad/s");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" rad/s");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" rad/s");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
  accel.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup(void) 
{

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Set up the SD card
  
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  
  Serial.println("card initialized.");

  //Set up the gyroscope
  Serial.println("Gyroscope Test"); Serial.println("");
  
  /* Enable auto-ranging */
  gyro.enableAutoRange(true);
  
  /* Initialise the sensor */
  if(!gyro.begin())
  {
    /* There was a problem detecting the L3GD20 ... check your connections */
    Serial.println("Ooops, no L3GD20 detected ... Check your wiring!");
    while(1);
  }

  //Set up the accelerometer
   Serial.println("Accelerometer Test"); Serial.println("");

  accel.enableAutoRange(true);

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  
  /* Display some basic information on these sensors */
  displaySensorDetails();
}

void loop(void) 
{
  /* Get a new sensor event */ 
  DateTime now = RTC.now();
  sensors_event_t gyro_event,accel_event; 
    
  gyro.getEvent(&gyro_event); 
  /* Display the results (speed is measured in rad/s) */
  /*
  Serial.print("X: "); Serial.print(gyro_event.gyro.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(gyro_event.gyro.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(gyro_event.gyro.z); Serial.print("  ");
  Serial.println("rad/s ");
  */
  
  accel.getEvent(&accel_event); 
  /* Display the results (speed is measured in m/s2) */
  /*
  Serial.print("X: "); Serial.print(accel_event.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(accel_event.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(accel_event.acceleration.z); Serial.print("  ");
  Serial.println("m/s^2 ");
  */

   // make a string for assembling the average data values:
  String dataString = "";
  dataString=String(gyro_event.gyro.x)+","+String(gyro_event.gyro.y)+","+String(gyro_event.gyro.z)+","+String(accel_event.acceleration.x)+","+String(accel_event.acceleration.y)+","+String(accel_event.acceleration.z);
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  //File dataFile = SD.open("L3GD20_037.csv", FILE_WRITE);
  File dataFile = SD.open(filename, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
      // log time
    dataFile.print(now.unixtime()); // seconds since 1/1/1970
    dataFile.print(", ");
    //dataFile.print('"');
    dataFile.print(now.year(), DEC);
    dataFile.print("/");
    dataFile.print(now.month(), DEC);
    dataFile.print("/");
    dataFile.print(now.day(), DEC);
    dataFile.print(" ");
    dataFile.print(now.hour(), DEC);
    dataFile.print(":");
    dataFile.print(now.minute(), DEC);
    dataFile.print(":");
    dataFile.print(now.second(), DEC);
    //dataFile.print('"');
    dataFile.print(',');
    dataFile.println(dataString);
    dataFile.close();
  }
  
  delay(2);
}

