#include "RTClib.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include "DHT.h"

// rtc object
RTC_DS1307 rtc;

// servo_motor
Servo servo_motor;

// lcd object for the display, 0month20 -> i2c address
LiquidCrystal_I2C lcd(0x20, 20, 4);

//  dht11 conf
#define DHTPIN 13
#define DHTYPE DHT11

DHT dht(DHTPIN, DHTYPE);
float temp;
float h;

// button variables
int buttonPin = 10;
int button_Read;

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };


int year, month, day, hour, min, sec;
String dayOfWeek;
String dayOrNight;

void setup() {
  Wire.begin();
  Serial.begin(9600);

  // start dht11 sensor
  dht.begin();

  //  20 colums and 4 rows
  lcd.begin(20, 4);

  //  9 is the analog output pin for the motor
  servo_motor.attach(9);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  // set the Time
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");

    // get real time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // set the pin for the button
  pinMode(buttonPin, INPUT);
}

void loop() {

  delay(300);
  // temperature calculation
  temp = dht.readTemperature();

  // humidity calculate
  h = dht.readHumidity();

  DateTime now = rtc.now();

  year = now.year();
  month = now.month();
  day = now.day();
  dayOfWeek = daysOfTheWeek[now.dayOfTheWeek()];
  hour = now.hour();
  min = now.minute();
  sec = now.second();

  // to know the time is PM or AM
  AM_PM();

  // show the date on lcd
  lcd_print();
  delay(700);

  lcd.clear();
  dht_read();
  delay(700);

  // function for serial moniter print
  serial_print();

  // to check if it is raining right now or not.
  if (isItRaining() == false) {

    // time for start and stop the motor
    if (hour >= 10 && hour < 16) {
      motorRun(0);
    } else {
      motorRun(90);
    }

  } else {
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.println("it's raining now.");
    delay(1000);
    lcd.clear();
    lcd.println("Window is closed");
    delay(1000);
    lcd.clear();
    lcd.println("if you wanna open it, click on the button");
    delay(1000);
  }

  // if buttonCheck is true window will open
  if (buttonCheck() == HIGH) {
    motorRun(0);
    lcd.clear();
    lcd.print("click the button to close it");
    delay(500);
    
  } else {
    motorRun(90);
    lcd.clear();
    lcd.print("click the button to open it");
    delay(500);
  }

  delay(500);
  lcd.clear();
}


// function for serial moniter

void serial_print() {

  // print year - month - day
  Serial.println(hour);

  Serial.print(year, DEC);
  Serial.print('/');
  Serial.print(month, DEC);
  Serial.print('/');
  Serial.print(day, DEC);
  Serial.print(" (");
  Serial.print(dayOfWeek);
  Serial.print(") ");

  // print hour - minute - second
  Serial.print(hour, DEC);
  Serial.print(':');
  Serial.print(min, DEC);
  Serial.print(':');
  Serial.print(sec, DEC);
  Serial.print(dayOrNight);
  Serial.println();
  Serial.println();
}

// function for print on lcd

void lcd_print() {
  lcd.print(year);
  lcd.print('/');
  lcd.print(month);
  lcd.print('/');
  lcd.println(day);
  lcd.println(dayOfWeek);
  lcd.setCursor(0, 2);
  lcd.print(hour, DEC);
  lcd.print('/');
  lcd.print(min);
  lcd.print('/');
  lcd.print(sec);
  lcd.println(dayOrNight);
  delay(1000);
}

void dht_read() {
  lcd.print("TEMP: ");
  lcd.print(temp);
  lcd.println(" C");
  lcd.setCursor(0, 2);
  lcd.println("Humidity: ");
  lcd.print(h);

  Serial.print("TEMP: ");
  Serial.println(temp);
  Serial.print("Humidaty: ");
  Serial.print(h);
}

// to check if it raining or not
bool isItRaining() {
  return h >= 90.0 ? true : false;
}

bool buttonCheck() {
  button_Read = digitalRead(buttonPin);
  return button_Read > 0 ? true : false;
}

void motorRun(byte num) {
  servo_motor.write(num);
  delay(15);
}

// to know the time is PM or AM
void AM_PM() {
  if (hour >= 12) {
    dayOrNight = " PM";
  } else {
    dayOrNight = " AM";
  }
}