#include <GyverPower.h>
#include <powerConstants.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <dht.h>

#define DHT11_PIN 7 //DHT temp + air humidity sensor pin
#define PIR_PIN 2 //Movement sensor pin
#define AIR_VALUE 590 //Colebrate air value
#define WATER_VALUE 270 //Colebrate water value
#define ENABLED_TICKS_COUNT 10 //Count of ticks to be enabled before switch off
#define SOIL_DATA_PIN A0

unsigned int soilMoistureValue = 0; //Value of soil humidity
unsigned int soilmoisturepercent = 0; //Value of soil humidity in percents

bool displayEnabled = true;
unsigned long tickTime = 1000; //Tick is 1 second
unsigned long longSleepTime = 10 * 60 * 1000; //We sleep for 10 minutes
volatile long enabledTickCounter = ENABLED_TICKS_COUNT;

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
dht DHT;

void setup()
{
  pinMode(PIR_PIN, INPUT_PULLUP);
  power.setSleepMode(POWERDOWN_SLEEP);

  lcd.init();
  if (displayEnabled) {
    lcd.backlight();
  } else {
    lcd.noBacklight();
  }
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), displayEnable, RISING);
}

void loop(){
  bool isSomethingMovingAround = digitalRead(PIR_PIN);
  if (isSomethingMovingAround) {
    enabledTickCounter = ENABLED_TICKS_COUNT;
  }

  enabledTickCounter--;
  if (enabledTickCounter > 0) {
    keepDisplayOn();
    checkSensorsAndPrintResult();
    power.sleepDelay(tickTime);
    return;
  }

  keepDisplayOff();
  power.sleepDelay(longSleepTime);
}

void keepDisplayOn() {
  if (displayEnabled) {
    return;
  }

  lcd.backlight();
  displayEnabled = true;
}

void keepDisplayOff() {
  if (!displayEnabled) {
    return;
  }

  lcd.noBacklight();
  displayEnabled = false;
}

void checkSensorsAndPrintResult() {
  DHT.read11(DHT11_PIN);
  soilMoistureValue = analogRead(SOIL_DATA_PIN);
  soilmoisturepercent = map(soilMoistureValue, AIR_VALUE, WATER_VALUE, 0, 100);

  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print("Soil ");
  if (soilmoisturepercent >= 100)
  {
    lcd.print("100%");
  }
  else if (soilmoisturepercent <= 0)
  {
    lcd.print("0%");
  } else if (soilmoisturepercent > 0 && soilmoisturepercent < 100)
  {
    lcd.print(soilmoisturepercent);
    lcd.print("%");
  }

  lcd.print(" Air ");
  lcd.print((int) DHT.humidity);
  lcd.print("%");


  lcd.setCursor(0, 1);
  lcd.print("Temperature ");
  lcd.print((int) DHT.temperature);
  lcd.write(223);
}

void displayEnable() {
  enabledTickCounter = ENABLED_TICKS_COUNT;
  power.wakeUp();
}