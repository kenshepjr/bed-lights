#include <RTClib.h>             // for I2C communication
#include <Wire.h>               // for RTC

// LED Pins and definitions
#define RIGHT_BLUEPIN 3
#define RIGHT_REDPIN 5
#define RIGHT_GREENPIN 6
#define LEFT_BLUEPIN 9
#define LEFT_REDPIN 10
#define LEFT_GREENPIN 11

RTC_DS3231 rtc;                 // create rtc for the DS3231 RTC module, address is fixed at 0x68

// Motion Sensor variables
int LEFT_msensor = 13;                     // enter pin numbers here
int RIGHT_msensor = 2;                    // enter pin numbers here                // may not be needed but will keep here
char t[32];
long int calibrationTime = 60;
const int ACTIVE = HIGH;
int LEFT_mval = 12;
int RIGHT_mval = 7;

// Calculate the number of steps and the increment at each step
// Define LED Constants
const int LED_OFF = 0;
const int LED_ON = 256 ; 
const int late_hour_on = 20;
const int early_hour_on = 7;

// Add these two new variables
bool LEFT_LED_ON = false;
bool RIGHT_LED_ON = false;

void setup()
{
  
  Wire.begin();
  rtc.begin();                  // initialize RTC object

//  if(rtc.lostPower())           // comment this out and just upload the adjust time 1st. then reupload with these uncommented out. 
  {
    rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
  }

  // LED pin outputs
  pinMode(LEFT_REDPIN, OUTPUT);
  pinMode(LEFT_BLUEPIN, OUTPUT);
  pinMode(LEFT_GREENPIN, OUTPUT);
  pinMode(RIGHT_REDPIN, OUTPUT);
  pinMode(RIGHT_BLUEPIN, OUTPUT);
  pinMode(RIGHT_GREENPIN, OUTPUT);
  
  // Motion Sensor inputs 
  pinMode(RIGHT_msensor, INPUT);
  pinMode(LEFT_msensor, INPUT);

  Serial.begin(9600);
  Serial.println();
  Serial.println("Waiting for the sensor to warm up.");
  // delay(calibrationTime * 1000); // Convert the time from seconds to milliseconds. 1 minute calibration time
  Serial.println("SENSOR ACTIVE");

}

void loop()
{
  int LEFT_mval = digitalRead(LEFT_msensor);
  int RIGHT_mval = digitalRead(RIGHT_msensor);  

  DateTime now = rtc.now();
  sprintf(t, "%02d:%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());  
  Serial.print(F("Date/Time: "));
  Serial.println(t); 

  if (now.hour() >= late_hour_on || now.hour() <= early_hour_on) {
    if (LEFT_mval == ACTIVE && !LEFT_LED_ON) {
      for (int r = 0; r <= LED_ON; r++) {
        float C = LED_ON/20;
        float B = log(200)/LED_ON;
        float A = C/(exp(B*LED_ON));
        float fadeup = A*exp(B*r);
        analogWrite(LEFT_REDPIN, fadeup);
        LEFT_LED_ON = true;
      }

    } else if (LEFT_mval == LOW && LEFT_LED_ON) {
      analogWrite(LEFT_REDPIN, LED_OFF);
      LEFT_LED_ON = false;
    }

    if (RIGHT_mval == ACTIVE && !RIGHT_LED_ON) {
      for (int r = 0; r <= LED_ON; r++) {
        float C = LED_ON/20;
        float B = log(200)/LED_ON;
        float A = C/(exp(B*LED_ON));
        float fadeup = A*exp(B*r);
        analogWrite(RIGHT_REDPIN, fadeup);
        RIGHT_LED_ON = true;
      }

    } else if (RIGHT_mval == LOW && RIGHT_LED_ON) {
      analogWrite(RIGHT_REDPIN, LED_OFF);
      RIGHT_LED_ON = false;
    }
  } else {
    analogWrite(LEFT_REDPIN, LED_OFF);
    analogWrite(RIGHT_REDPIN, LED_OFF);
    LEFT_LED_ON = false;
    RIGHT_LED_ON = false;
  }
}