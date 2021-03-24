// include the library code:
#include <LiquidCrystal.h>
#include <EEPROM.h>

const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//counter setup
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

int motor_status_led_pin_start = 6;
int motor_status_led_pin_stop = 5;

int num = 0;
int data_address = 0;
int num_address = 2;
// int dataCount = 0000;

void setup()
{
    Serial.begin(9600);
    //lcd setup
    lcd.begin(16, 2);
    lcd.print("Hey Tanmaya");
    // LED output

    pinMode(increase_pin, INPUT);
    pinMode(decrease_pin, INPUT);
    pinMode(reset_pin, INPUT);
    pinMode(start_stop_button, INPUT);
    pinMode(moter_sensor_pin, INPUT);
    pinMode(moter_relay_pin, OUTPUT);
    pinMode(motor_status_led_pin_start, OUTPUT);
    pinMode(motor_status_led_pin_stop, OUTPUT);

    num = readIntFromEEPROM(num_address);
    delay(1000);
    lcd.clear();
    lcd.print("Initialized...");
    delay(2000);
    lcd.clear();
}

void loop()

{
    Serial.println(num);
    if (num < 9999 && digitalRead(increase_pin))
    {
        num += 1;
        // dataCount = num;
        writeIntIntoEEPROM(data_address, num);
        writeIntIntoEEPROM(num_address, num);
        writeToDisplay(0, num);
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
                writeToDisplay(0, num);
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
        digitalWrite(motor_status_led_pin_start, HIGH);
        digitalWrite(motor_status_led_pin_stop, LOW);
    }
    else if (moterRelaySate == 0)
    {
        digitalWrite(motor_status_led_pin_stop, HIGH);
        digitalWrite(motor_status_led_pin_start, LOW);
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
                writeIntIntoEEPROM(num_address, readIntFromEEPROM(data_address));
            }
        }
        delay(10);
    }
    lastMoterSensorState = moterSensorState;

    if (digitalRead(reset_pin))
    {
        num = 0;
        writeToDisplay(0, num);
    }

    writeToDisplay(1, num);
    Serial.println(num);
}

////////////////////////////////////////////////////////////

void writeIntIntoEEPROM(int address, int number)
{
    EEPROM.write(address, number >> 8);
    EEPROM.write(address + 1, number & 0xFF);
}

int readIntFromEEPROM(int address)
{
    return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}
void writeToDisplay(int row_no, int num)
{
    if (row_no == 0)
    {
        lcd.setCursor(0, row_no);
        lcd.print("Setup :");
    }
    else
    {
        lcd.setCursor(0, row_no);
        lcd.print("Count :");
    }

    lcd.setCursor(8, row_no);
    lcd.print(num / 1000);
    lcd.setCursor(9, row_no);
    lcd.print(num / 100 % 10);
    lcd.setCursor(10, row_no);
    lcd.print(num / 10 % 10);
    lcd.setCursor(11, row_no);
    lcd.print(num % 10);
}
