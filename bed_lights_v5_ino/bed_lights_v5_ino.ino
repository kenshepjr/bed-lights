#include <RTClib.h>             // for I2C communication
#include <Wire.h>               // for RTC

// LED Pins and definitions
#define RIGHT_BLUEPIN 3
#define RIGHT_REDPIN 5
#define RIGHT_GREENPIN 6
#define LEFT_BLUEPIN 9 // the connection to the led tracks is wonky
#define LEFT_REDPIN 10
#define LEFT_GREENPIN 11

RTC_DS3231 rtc;                 // create rtc for the DS3231 RTC module, address is fixed at 0x68

// Motion Sensor variables
int LEFT_msensor = 8;                     // enter pin numbers here
int RIGHT_msensor = 7;                    // enter pin numbers here                // may not be needed but will keep here
char t[32];
long int calibrationTime = 60;
const int ACTIVE = HIGH;
int LEFT_mval = 12;
int RIGHT_mval = 7;

// Define LED Constants
const int LED_OFF = 0;
const int brightness = 256; 
const int hour_on = 19;
const int hour_off = 7;
const int all_LEDS_on = hour_on;
const int all_LEDS_off = hour_on + 2;
const int red_LEDS_on = all_LEDS_off;
const int red_LEDS_off = hour_off;

// Add these two new variables
bool LED_ON = false;


void setup()
{
  
  Wire.begin();
  rtc.begin();                  // initialize RTC object

  // //////////////// upload this the block of code the first time then comment this code out the second time. 
  // DateTime compileTime = DateTime(F(__DATE__), F(__TIME__));
  // DateTime adjustedTime = compileTime + TimeSpan(0, 0, 0, 6); // Add 6 seconds for system differences
  // rtc.adjust(adjustedTime);
  // //////////////// end of first block

  ///////// upload this block of code the second time and comment the first block of code
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    DateTime adjustedTime = rtc.now() + TimeSpan(0, 0, 0, 6); // Add 8 seconds
    rtc.adjust(adjustedTime);
  }
  ////////////

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

  turn_off_all_LEDS();

}

void loop() {
  int LEFT_mval = digitalRead(LEFT_msensor);
  int RIGHT_mval = digitalRead(RIGHT_msensor);  
  char test[32];
  // sprintf(test, "left mval = %01d, right mval = %01d", LEFT_mval, RIGHT_mval);
  // Serial.println(test);

  DateTime now = rtc.now();
  sprintf(t, "%02d:%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.second(), now.year(), now.month(), now.day());  
  Serial.print(F("Date/Time: "));
  Serial.println(t); 

  // // this is to check the connections for the sensors
  // Serial.println("left");
  // Serial.println(LEFT_mval);
  // Serial.println("right");
  // Serial.println(RIGHT_mval);

// Active hours for the LEDS to be on
  if (now.hour() >= hour_on || now.hour() <= hour_off) {
    // All leds on to walk into the room and go to bed
    if (now.hour() >= all_LEDS_on && now.hour() < all_LEDS_off) {
      if ((LEFT_mval == ACTIVE || RIGHT_mval == ACTIVE) && !LED_ON) {
        turn_on_all_LEDS();
        LED_ON = true;
        // Serial.println("Made it to the if all_led on stage");

      } else if (LEFT_mval == LOW && RIGHT_mval == LOW && LED_ON) {
          turn_off_all_LEDS();
          LED_ON = false;
          // Serial.println("Made it to the else if all_led on stage");
      }
    } else if (now.hour() >= red_LEDS_on || now.hour() <= red_LEDS_off) {
      if ((LEFT_mval == ACTIVE || RIGHT_mval == ACTIVE) && !LED_ON) {
        for (int r = 0; r <= brightness; r++) {
          float C = brightness/20;
          float B = log(200)/brightness;
          float A = C/(exp(B*brightness));
          float fadeup = A*exp(B*r);
          analogWrite(LEFT_REDPIN, fadeup);
          analogWrite(RIGHT_REDPIN, fadeup);
        }
        // analogWrite(LEFT_BLUEPIN, 255);
        // analogWrite(RIGHT_BLUEPIN, 255);
        LED_ON = true;
        // Serial.println("Made it to the  red leds if statement");
      } else if (LEFT_mval == LOW && RIGHT_mval == LOW && LED_ON) {
          turn_off_all_LEDS();
          LED_ON = false;
          Serial.println("Made it to the else red else if statement");
      }
    } 
  } else {
      turn_off_all_LEDS();
      LED_ON = false;
  }
}

// Define function here
void turn_off_all_LEDS() {
  analogWrite(LEFT_BLUEPIN, LED_OFF);
  analogWrite(LEFT_REDPIN, LED_OFF);
  analogWrite(LEFT_GREENPIN, LED_OFF);
  analogWrite(RIGHT_BLUEPIN, LED_OFF);
  analogWrite(RIGHT_REDPIN, LED_OFF);
  analogWrite(RIGHT_GREENPIN, LED_OFF);
}

void turn_on_all_LEDS() {
  analogWrite(LEFT_BLUEPIN, 255);
  analogWrite(LEFT_REDPIN, 255);
  analogWrite(LEFT_GREENPIN, 255);
  analogWrite(RIGHT_BLUEPIN, 255);
  analogWrite(RIGHT_REDPIN, 255);
  analogWrite(RIGHT_GREENPIN, 255);
}
