// include the library code:
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

// counter setup
const int start_stop_button = 4;
const int reset_pin = 5;
const int increase_pin = 6;
const int decrease_pin = 7;
const int wind_switch_increase = 8;
const int wind_switch_decrease = 9;

const int moter_sensor_analog_pin = A0;
const int moter_sensor_digital_pin = 3;
int moter_relay_pin = 10;

int windSwithcIncreaseButtonState = 0;
int lastWindSwithcIncreaseButtonState = 0;

int windSwithcDecreaseButtonState = 0;
int lastWindSwithcDecreaseButtonState = 0;

int decreaseButtonState = 0;
int lastDecreaseButtonState = 0;

int startButtonState = 0;
int moterRelaySate = 0;

int moterSensorState = 0;
int lastMoterSensorState = 1;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int hallSensorData = 0;
int centerX = 100;  // X position for the center of the rotating circle
int centerY = 32;   // Y position (centered vertically)
int radius = 10;    // Radius of the circle
float angle = 0;    // Initial angle for the rotating circle
float speed = 0.1;  // Rotation speed (adjust for faster or slower rotation)

int setup_data = 0;
int count_data = 0;
int setup_data_address = 0;
int count_data_address = 2;
// int dataCount = 0000;

long buttonTimer = 0;
long longPressTime = 1000;

boolean buttonActive = false;
boolean longPressActive = false;
void setup() {
  Serial.begin(9600);
  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed if the display isn't found
  }
  // Clear the buffer


  pinMode(increase_pin, INPUT);
  pinMode(decrease_pin, INPUT);
  pinMode(reset_pin, INPUT);
  pinMode(start_stop_button, INPUT);
  pinMode(moter_sensor_analog_pin, INPUT);
  pinMode(moter_sensor_digital_pin, INPUT);
  pinMode(wind_switch_increase, INPUT);
  pinMode(wind_switch_decrease, INPUT);
  pinMode(moter_relay_pin, OUTPUT);

  setup_data = readIntFromEEPROM(setup_data_address);
  count_data = readIntFromEEPROM(count_data_address);
  display.clearDisplay();
  display.setTextSize(1);  // Normal text size
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(48, 10);  // Row 2
  display.println(F("SRIMAA"));
  display.setCursor(33, 30);  // Row 2
  display.println(F("ELECTRICALS"));
  display.setCursor(23, 50);  // Row 2
  display.println(F("www.iobytes.in"));
  display.display();
  delay(5000);
}

void loop()

{
  //Serial.println(setup_data);
  //Serial.println(count_data);
  if (setup_data < 9999 && digitalRead(increase_pin)) {
    setup_data += 1;
    writeIntIntoEEPROM(setup_data_address, setup_data);
  }

  decreaseButtonState = digitalRead(decrease_pin);

  if (decreaseButtonState != lastDecreaseButtonState) {
    if (decreaseButtonState == HIGH) {
      if (setup_data > 0) {
        setup_data -= 1;
        writeIntIntoEEPROM(setup_data_address, setup_data);
      }
    }
    delay(10);
  }
  lastDecreaseButtonState = decreaseButtonState;

  // crompton code

  windSwithcIncreaseButtonState = digitalRead(wind_switch_increase);

  if (windSwithcIncreaseButtonState != lastWindSwithcIncreaseButtonState) {
    if (windSwithcIncreaseButtonState == HIGH) {
      if (setup_data > 0) {
        setup_data *= 2;
        writeIntIntoEEPROM(setup_data_address, setup_data);
      }
    }
    delay(10);
  }
  lastWindSwithcIncreaseButtonState = windSwithcIncreaseButtonState;

  windSwithcDecreaseButtonState = digitalRead(wind_switch_decrease);

  if (windSwithcDecreaseButtonState != lastWindSwithcDecreaseButtonState) {
    if (windSwithcDecreaseButtonState == HIGH) {
      if (setup_data > 0) {
        setup_data /= 2;
        writeIntIntoEEPROM(setup_data_address, setup_data);
      }
    }
    delay(10);
  }
  lastWindSwithcDecreaseButtonState = windSwithcDecreaseButtonState;

  //END crompton code

  if (startButtonState == 0 && digitalRead(start_stop_button) == HIGH && (setup_data - count_data) > 4) {
    startButtonState = 1;
    moterRelaySate = !moterRelaySate;
  }
  if (startButtonState == 1 && digitalRead(start_stop_button) == LOW) {
    startButtonState = 0;
  }
  digitalWrite(moter_relay_pin, moterRelaySate);

  // sensing logic
  moterSensorState = digitalRead(moter_sensor_digital_pin);
  // Serial.println("state");
  // Serial.println(moterSensorState);
  // Serial.println("last state");
  // Serial.println(lastMoterSensorState);

  if (moterSensorState != lastMoterSensorState) {
    if (moterSensorState == HIGH) {
      if (count_data < setup_data) {
        if ((setup_data - count_data) < 4) {
          moterRelaySate = 0;
        }
        count_data++;
        writeIntIntoEEPROM(count_data_address, count_data);
      }
      if (count_data == setup_data) {
        display.clearDisplay();
        display.setCursor(0, 32);  // Row 3
        display.println(F("1 Set Completed..."));
        display.display();
        delay(5000);
        display.clearDisplay();
        count_data = 0;
        writeIntIntoEEPROM(count_data_address, count_data);
      }
    }
    delay(10);
  }
  lastMoterSensorState = moterSensorState;
  // reset code
  if (digitalRead(reset_pin) == HIGH) {
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }

    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      longPressActive = true;
      setup_data = 0;
      count_data = 0;
      writeIntIntoEEPROM(setup_data_address, setup_data);
      writeIntIntoEEPROM(count_data_address, count_data);
    }
  } else {
    if (buttonActive == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {
        count_data = 0;
        writeIntIntoEEPROM(count_data_address, count_data);
      }
      buttonActive = false;
    }
  }
  // end of reset button
  refreshOled(moterRelaySate, setup_data, count_data);
}

////////////////////////////////////////////////////////////

void writeIntIntoEEPROM(int address, int number) {
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

int readIntFromEEPROM(int address) {
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

void refreshOled(int mtrRelaySate, int setupData, int countData) {
  // Clear the previous screen contents
  display.clearDisplay();

  // Draw 3 rows of text
  display.setTextSize(2);  // Normal text size
  display.setTextColor(SSD1306_WHITE);

  // Row 1: "Set Count" with the value read from EEPROM
  display.setCursor(0, 2);  // Row 1
  display.print(F("TURNS: "));
  display.println(setupData);

  // Row 2: Displaying static text
  display.setCursor(0, 24);  // Row 2
  display.print(F("COUNT: "));
  display.println(countData);
  if (mtrRelaySate == 1) {
    display.setCursor(0, 48);  // Row 3
    display.print(F("Moter: R"));
  }else{
    display.setCursor(0, 48);  // Row 3
    display.print(F("Moter: S"));
  }

  display.display();


  delay(20);
}
