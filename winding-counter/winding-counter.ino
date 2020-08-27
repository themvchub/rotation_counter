#include <EEPROM.h>
int SS_DP;
const int SS_A = 6;
const int SS_B = 7;
const int SS_C = 8;
const int SS_D = 9;
const int SS_E = 10;
const int SS_F = 11;
const int SS_G = 12;
int Gnd_1 = 2;
int Gnd_2 = 3;
int Gnd_3 = 4;
int Gnd_4 = 5;
const int increase_pin = A0;
const int decrease_pin = A1;
const int reset_pin = A2;
const int start_stop_button = A3;

int moter_sensor_pin = A4;
int moter_relay_pin = A5;
int moter_swithing_led_pin = 13;

int decreaseButtonState = 0;
int lastDecreaseButtonState = 0;

int startButtonState = 0;
int moterRelaySate = 0;

int moterSensorState = 0;
int lastMoterSensorState = 0;

int num = 0000;
int data_address = 0;
int num_address = 2;
// int dataCount = 0000;

void setup()
{

  // LED output

  pinMode(SS_DP, OUTPUT);
  pinMode(SS_A, OUTPUT);
  pinMode(SS_B, OUTPUT);
  pinMode(SS_C, OUTPUT);
  pinMode(SS_D, OUTPUT);
  pinMode(SS_E, OUTPUT);
  pinMode(SS_F, OUTPUT);
  pinMode(SS_G, OUTPUT);
  pinMode(Gnd_1, OUTPUT);
  pinMode(Gnd_2, OUTPUT);
  pinMode(Gnd_3, OUTPUT);
  pinMode(Gnd_4, OUTPUT);
  pinMode(increase_pin, INPUT);
  pinMode(decrease_pin, INPUT);
  pinMode(reset_pin, INPUT);
  pinMode(start_stop_button, INPUT);
  pinMode(moter_sensor_pin, INPUT);
  pinMode(moter_relay_pin, OUTPUT);
  pinMode(moter_swithing_led_pin, OUTPUT);

  num = readIntFromEEPROM(num_address);
}

void loop()
{
  if (num < 9999 && digitalRead(increase_pin))
  {
    num += 1;
    // dataCount = num;
    writeIntIntoEEPROM(data_address, num);
    writeIntIntoEEPROM(num_address, num);
  }

  decreaseButtonState = digitalRead(decrease_pin);

  if (decreaseButtonState != lastDecreaseButtonState)
  {
    if (decreaseButtonState == HIGH)
    {
      if (num > 0)
      {
        num -= 1;
        // dataCount = num;
        writeIntIntoEEPROM(data_address, num);
        writeIntIntoEEPROM(num_address, num);
      }
    }
    delay(10);
  }
  lastDecreaseButtonState = decreaseButtonState;

  if (startButtonState == 0 && digitalRead(start_stop_button) == HIGH && num > 4)
  {
    startButtonState = 1;
    moterRelaySate = !moterRelaySate;
  }
  if (startButtonState == 1 && digitalRead(start_stop_button) == LOW)
  {
    startButtonState = 0;
  }
  digitalWrite(moter_relay_pin, moterRelaySate);
  if (moterRelaySate == 1)
  {
    digitalWrite(moter_swithing_led_pin, HIGH);
  }
  else if (moterRelaySate == 0)
  {
    digitalWrite(moter_swithing_led_pin, LOW);
  }

  moterSensorState = digitalRead(moter_sensor_pin);

  if (moterSensorState != lastMoterSensorState)
  {
    if (moterSensorState == HIGH)
    {
      if (num > 0)
      {
        if (num <= 5)
        {
          moterRelaySate = 0;
        }
        num--;
        // dataCount++;
        writeIntIntoEEPROM(num_address, num);
      }
      if (num == 0)
      {
        // writeIntIntoEEPROM(data_address, dataCount);
        // dataCount = 0000;
        delay(2000);
        num = readIntFromEEPROM(data_address);
        writeIntIntoEEPROM(num_address,readIntFromEEPROM(data_address));
      }
    }
    delay(10);
  }
  lastMoterSensorState = moterSensorState;

  if (digitalRead(reset_pin))
  {
    num = 0000;
  }

  blink(Gnd_1, num / 1000);
  blink(Gnd_2, num / 100 % 10);
  blink(Gnd_3, num / 10 % 10);
  blink(Gnd_4, num % 10);
}

////////////////////////////////////////////////////////////
void blink(int pinName, unsigned int n)
{
  displayDigit(n);
  digitalWrite(pinName, HIGH);
  delay(1);
  digitalWrite(pinName, LOW);
  delay(1);
}

void writeIntIntoEEPROM(int address, int number)
{
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

int readIntFromEEPROM(int address)
{
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

void displayDigit(unsigned int n)
{
  //// Clear the display
  digitalWrite(SS_DP, LOW);
  digitalWrite(SS_A, LOW);
  digitalWrite(SS_B, LOW);
  digitalWrite(SS_C, LOW);
  digitalWrite(SS_D, LOW);
  digitalWrite(SS_E, LOW);
  digitalWrite(SS_F, LOW);
  digitalWrite(SS_G, LOW);

  switch (n)
  {
  case 0:
    digitalWrite(SS_G, HIGH);
    break;
  case 1:
    digitalWrite(SS_A, HIGH);
    digitalWrite(SS_D, HIGH);
    digitalWrite(SS_E, HIGH);
    digitalWrite(SS_F, HIGH);
    digitalWrite(SS_G, HIGH);
    break;
  case 2:
    digitalWrite(SS_C, HIGH);
    digitalWrite(SS_F, HIGH);
    break;
  case 3:
    digitalWrite(SS_E, HIGH);
    digitalWrite(SS_F, HIGH);
    break;
  case 4:
    digitalWrite(SS_A, HIGH);
    digitalWrite(SS_E, HIGH);
    digitalWrite(SS_D, HIGH);
    break;
  case 5:
    digitalWrite(SS_B, HIGH);
    digitalWrite(SS_E, HIGH);
    break;
  case 6:
    digitalWrite(SS_B, HIGH);
    break;
  case 7:
    digitalWrite(SS_D, HIGH);
    digitalWrite(SS_E, HIGH);
    digitalWrite(SS_F, HIGH);
    digitalWrite(SS_G, HIGH);
    break;
  case 8:
    break;
  case 9:
    digitalWrite(SS_E, HIGH);

    break;
  default:
    digitalWrite(SS_DP, HIGH); // error chacking
  }
}
