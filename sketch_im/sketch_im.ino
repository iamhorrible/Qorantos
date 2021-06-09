
// Include Libraries
#include "Arduino.h"
#include "LiquidCrystal.h"
#include "HX711.h"
#include "Button.h"


// Pin Definitions
#define LCD_PIN_RS  7
#define LCD_PIN_E 6
#define LCD_PIN_DB4 2
#define LCD_PIN_DB5 3
#define LCD_PIN_DB6 4
#define LCD_PIN_DB7 5
#define SCALE_PIN_DAT 9
#define SCALE_PIN_CLK 8
#define TOGGLESWITCH_PIN_2  11

byte soundPin = 10;    // connect this to P-E of sound module with some resistor from 300 to 1K



// Global variables and defines

// object initialization
LiquidCrystal lcd(LCD_PIN_RS, LCD_PIN_E, LCD_PIN_DB4, LCD_PIN_DB5, LCD_PIN_DB6, LCD_PIN_DB7);

HX711 scale(SCALE_PIN_DAT, SCALE_PIN_CLK);
#define calibration_factor 2280 //This value is obtained using the SparkFun_HX711_Calibration sketch https://learn.sparkfun.com/tutorials/load-cell-amplifier-hx711-breakout-hookup-guide?_ga=2.77038550.2126325781.1526891300-303225217.1493631967
int weightOnScale;

Button toggleSwitch(TOGGLESWITCH_PIN_2);
bool toggleSwitchVal;

bool alarmActivated = false;


// define vars for testing menu
const int timeout = 10000;       //define timeout of 10 sec
char menuOption = 0;
long time0;

// init strings for alarm
String alarmOff = "Alarm OFF";
String alarmOn = "Alarm On";
String alarmTriggered = "Weight Removed!";



void setup()
{
  Serial.begin(9600);
  while (!Serial) ; // wait for serial port to connect. Needed for native USB
  Serial.println("start");

  // LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print(alarmOff);

  // scale
  scale.set_scale(calibration_factor);
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

  // button
  toggleSwitch.init();
  toggleSwitchVal = toggleSwitch.read();

  // sound
  pinMode(soundPin, OUTPUT);
  digitalWrite(soundPin, 0);

}


void loop()
{

  bool switchValLoop = toggleSwitch.read();

  if (switchValLoop != toggleSwitchVal && !alarmActivated) {
    weightOnScale = round(scale.get_units());
    lcd.clear();
    lcd.print(alarmOn);
    toggleSwitchVal = switchValLoop;
    alarmActivated = true;
    digitalWrite(soundPin, 0);
  }
  else if (switchValLoop != toggleSwitchVal && alarmActivated) {
    lcd.clear();
    lcd.print(alarmOff);
    toggleSwitchVal = switchValLoop;
    alarmActivated = false;
    digitalWrite(soundPin, 0);
  }

  if (alarmActivated) {
    int weightCheck = round(scale.get_units());
    if (weightCheck < weightOnScale) {
      lcd.clear();
      lcd.print(alarmTriggered);
      digitalWrite(soundPin, 1);
    }
  }
}
