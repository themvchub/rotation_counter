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

int setup_data = 0;
int count_data = 0;
int setup_data_address = 0;
int count_data_address = 2;
// int dataCount = 0000;

long buttonTimer = 0;
long longPressTime = 1000;

boolean buttonActive = false;
boolean longPressActive = false;
void setup()
{
    //lcd setup
    lcd.begin(16, 2);
    lcd.print("SRIMAA");
    lcd.setCursor(0, 1);
    lcd.print("ELECTRICALS");
    // LED output

    pinMode(increase_pin, INPUT);
    pinMode(decrease_pin, INPUT);
    pinMode(reset_pin, INPUT);
    pinMode(start_stop_button, INPUT);
    pinMode(moter_sensor_pin, INPUT);
    pinMode(moter_relay_pin, OUTPUT);
    pinMode(motor_status_led_pin_start, OUTPUT);
    pinMode(motor_status_led_pin_stop, OUTPUT);

    setup_data = readIntFromEEPROM(setup_data_address);
    count_data = readIntFromEEPROM(count_data_address);
    delay(2000);
    lcd.clear();
    lcd.print("Initialized...");
    delay(2000);
    lcd.clear();
}

void loop()

{
    if (setup_data < 9999 && digitalRead(increase_pin))
    {
        setup_data += 1;
        writeIntIntoEEPROM(setup_data_address, setup_data);
    }

    decreaseButtonState = digitalRead(decrease_pin);

    if (decreaseButtonState != lastDecreaseButtonState)
    {
        if (decreaseButtonState == HIGH)
        {
            if (setup_data > 0)
            {
                setup_data -= 1;
                writeIntIntoEEPROM(setup_data_address, setup_data);
            }
        }
        delay(10);
    }
    lastDecreaseButtonState = decreaseButtonState;

    if (startButtonState == 0 && digitalRead(start_stop_button) == HIGH && (setup_data - count_data) > 4)
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
    //sensing logic
    moterSensorState = digitalRead(moter_sensor_pin);

    if (moterSensorState != lastMoterSensorState)
    {
        if (moterSensorState == HIGH)
        {
            if (count_data < setup_data)
            {
                if ((setup_data - count_data) < 4)
                {
                    moterRelaySate = 0;
                }
                count_data++;
                writeIntIntoEEPROM(count_data_address, count_data);
            }
            if (count_data == setup_data)
            {
                lcd.setCursor(0, 1);
                lcd.print("1 SET COMPLETED");

                delay(5000);
                lcd.clear();
                count_data = 0;
                writeIntIntoEEPROM(count_data_address, count_data);
            }
        }
        delay(10);
    }
    lastMoterSensorState = moterSensorState;
    //reset code
    if (digitalRead(reset_pin) == HIGH)
    {
        if (buttonActive == false)
        {
            buttonActive = true;
            buttonTimer = millis();
        }

        if ((millis() - buttonTimer > longPressTime) && (longPressActive == false))
        {
            longPressActive = true;
            setup_data = 0;
            count_data = 0;
            writeIntIntoEEPROM(setup_data_address, setup_data);
            writeIntIntoEEPROM(count_data_address, count_data);
        }
    }
    else
    {
        if (buttonActive == true)
        {
            if (longPressActive == true)
            {
                longPressActive = false;
            }
            else
            {
                count_data = 0;
                writeIntIntoEEPROM(count_data_address, count_data);
            }
            buttonActive = false;
        }
    }
    //end of reset button
    writeToDisplay(0, setup_data);
    writeToDisplay(1, count_data);
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
