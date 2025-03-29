#include <RTClib.h>             // for I2C communication
#include <Wire.h>               // for RTC

// LED Pins and definitions
#define RIGHT_BLUEPIN 3
#define RIGHT_REDPIN 5
#define RIGHT_GREENPIN 6
#define LEFT_BLUEPIN 9
#define LEFT_REDPIN 10
#define LEFT_GREENPIN 11
#define LED_ON_TIME 5000  // 5000 ms = 5 seconds
#define DELAY_STEP 10 // 10ms between each step

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
const int LED_ON =  255; //256/20;
const int late_hour_on = 7;
const int early_hour_on = 7;
const int numSteps = LED_ON_TIME / DELAY_STEP;
int increment = LED_ON / numSteps;

// Variables to store the current LED value and the last time it was updated
int currentLEDValue = 0;
unsigned long lastUpdateTime = 0;

void setup()
{
  
  Wire.begin();
  rtc.begin();                  // initialize RTC object

 //if(rtc.lostPower())           // comment this out and just upload the adjust time 1st. then reupload with these uncommented out. 
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

  //   // Print the state of the motion sensors
  // Serial.print("Left sensor: ");
  // Serial.println(LEFT_mval);
  // Serial.print("Right sensor: ");
  // Serial.println(RIGHT_mval);

  int r;
  int LEFT_mval = digitalRead(LEFT_msensor);
  int RIGHT_mval = digitalRead(RIGHT_msensor);  

  DateTime now = rtc.now();
  sprintf(t, "%02d:%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());  
  Serial.print(F("Date/Time: "));
  Serial.println(t); 
 

//If the time is 8 pm determine if leds should be on our not
  if (now.hour() >= late_hour_on) {

// If the right IR sensor is trigger, then turn LEDS on
    if (LEFT_mval == ACTIVE || RIGHT_mval == ACTIVE) {
      Serial.println(LEFT_mval);
      for (r=0; r <= 255; r++) {
        // Serial.println("r");
        // Serial.println(r);
        increment += exp(log(255)*r/255);
     // Check if it's time to update the LED
        if (millis() - lastUpdateTime >= DELAY_STEP) {
          // Serial.println("Made it 2 the second iff statement");
          // Serial.println("Last update time");
          // Serial.println(lastUpdateTime);
          // Serial.println("Current LED value");
          // Serial.println(currentLEDValue);
          // Serial.println("Increment");
          // Serial.println(increment);
          // Increase the LED value and update the time
          currentLEDValue += increment;
          lastUpdateTime = millis();


          // Write the new value to the LED
          analogWrite(RIGHT_REDPIN, currentLEDValue);
          analogWrite(LEFT_REDPIN, currentLEDValue);
          if (LEFT_mval == ACTIVE) {
            
          }
        } 
      }

    }

    else {
      Serial.println(LEFT_mval);
      analogWrite(RIGHT_REDPIN, LED_OFF);
      analogWrite(LEFT_REDPIN, LED_OFF);
    }
  }

//if the sensor is before 7 am this allows the option for on and off. 
  else if (now.hour() <= early_hour_on) {  
  if (LEFT_mval == ACTIVE || RIGHT_mval == ACTIVE) {
      if (millis() - lastUpdateTime >= DELAY_STEP) {
        // Increase the LED value and update the time
        currentLEDValue += increment;
        lastUpdateTime = millis();

        // Write the new value to the LED
        analogWrite(RIGHT_REDPIN, currentLEDValue);
        analogWrite(LEFT_REDPIN, currentLEDValue);
      }
    }
//otherwise the left LEDS should be off
    else {
      analogWrite(LEFT_REDPIN, LED_OFF);
      analogWrite(RIGHT_REDPIN, LED_OFF);
    }

  }
//This initially sets the LEDS to be off
  else {
    analogWrite(LEFT_REDPIN, LED_OFF);
    analogWrite(LEFT_BLUEPIN, LED_OFF);
    analogWrite(LEFT_GREENPIN, LED_OFF);
    analogWrite(RIGHT_REDPIN, LED_OFF);
    analogWrite(RIGHT_BLUEPIN, LED_OFF);
    analogWrite(RIGHT_GREENPIN, LED_OFF);
  }
}