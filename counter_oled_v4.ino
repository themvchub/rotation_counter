#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Keypad.h>


// OLED Display Settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Keypad Settings
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 32, 33, 25, 26 };  // ESP32 GPIOs for rows
byte colPins[COLS] = { 27, 14, 12 };      // ESP32 GPIOs for columns

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Motor and Sensor Pins
#define MOTOR_PIN 5
#define SENSOR_PIN 34
#define S_S_BTN_PIN 35

// Variables
int targetCount = 0;    // User-specified target count
int rotationCount = 0;  // Current rotation count
bool motorRunning = false;
bool startButtonState = false;
int moterSensorState = 0;
int lastMoterSensorState = 1;

// Button State Tracking
bool lastButtonState = LOW;  // Previous state of the button
bool buttonPressed = false;  // Tracks if the button was pressed

// Function to Update OLED Display
void updateDisplay(int address = 0, String message = "") {
  display.clearDisplay();

  // Line 1: Set Value
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("SETUP:");
  display.println(targetCount);

  // Line 2: Current Count
  display.setCursor(0, 16);
  display.print("COUNT:");
  display.println(rotationCount);

  // Line 3: Motor Status
  display.setCursor(0, 32);
  display.print("MOTOR:");
  display.println(motorRunning ? "Run" : "Stop");

  // Line 4: Additional Message (Optional)
  if (message != "") {
    display.setCursor(address, 53);
    display.setTextSize(1);
    display.println(message);
  }

  display.display();
}

void setup() {
  // Initialize serial monitor
  Serial.begin(115200);

  // Initialize OLED display
  if (!display.begin(0x3C)) {  // Default I2C address for SH1106
    Serial.println(F("OLED initialization failed"));
    for (;;)
      ;  // Halt execution if OLED fails
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(50, 16);  // Row 3
  display.println(F("SRIMAA"));
  display.setCursor(35, 32);
  display.println(F("ELECTRICALS"));
  display.display();
  delay(5000);
  display.clearDisplay();
  updateDisplay();

  // Initialize motor and sensor
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(S_S_BTN_PIN, INPUT_PULLUP);
  digitalWrite(MOTOR_PIN, LOW);
}

void loop() {
  static String inputNumber = "";  // For capturing user input from the keypad
  char key = keypad.getKey();

  // Handle Keypad Input
  if (key && !motorRunning) {
    if (key >= '0' && key <= '9') {    // Numeric keys
      if (inputNumber.length() < 4) {  // Max 4 digits
        inputNumber += key;
        targetCount = inputNumber.toInt();
      }

    } else if (key == '#') {  // Confirm target value
      rotationCount = 0;

    } else if (key == '*') {
      inputNumber = "";
      targetCount = 0;
    }
    updateDisplay();
  }

  // Handle Start/Stop Button (State Change Detection)
  bool currentButtonState = digitalRead(S_S_BTN_PIN);
  if (currentButtonState == HIGH && lastButtonState == LOW) {
    buttonPressed = true;  // Button was pressed
  }

  if (currentButtonState == LOW && lastButtonState == HIGH && buttonPressed) {
    buttonPressed = false;                 // Button released
    startButtonState = !startButtonState;  // Toggle start button state

    if (startButtonState && (targetCount - rotationCount) > 4) {
      startMotor();
    } else {
      stopMotor();
    }
  }
  lastButtonState = currentButtonState;

  // Handle Sensor Input
  moterSensorState = digitalRead(SENSOR_PIN);

  if (moterSensorState != lastMoterSensorState) {
    if (moterSensorState == LOW) {
      if (rotationCount < targetCount) {
        if ((targetCount - rotationCount) < 4) {
          stopMotor();
        }
        rotationCount++;
        updateDisplay();
        // Add debounce
        if (rotationCount == targetCount) {
          rotationCount = 0;
          cleanupDisplay();
        }
      }
    }
    lastMoterSensorState = moterSensorState;
  }
}

// Function to Start the Motor
void startMotor() {
  updateDisplay(25, "Lunch Control");
  delay(1000);
  for (int i = 3; i > 0; i--) {
    updateDisplay(62, String(i));
    delay(1000);
  }
  updateDisplay(48, "READY");
  delay(1000);
  motorRunning = true;
  digitalWrite(MOTOR_PIN, HIGH);
  updateDisplay();
}

// Function to Stop the Motor
void stopMotor() {
  motorRunning = false;
  digitalWrite(MOTOR_PIN, LOW);
  updateDisplay();
}

void cleanupDisplay() {
  display.clearDisplay();
  display.setCursor(35, 16);  // Row 2
  display.println(F("1 Set"));
  display.setCursor(10, 48);  // Row 4
  display.println(F("COMPLETED"));
  display.display();
  delay(5000);
  updateDisplay();
}
