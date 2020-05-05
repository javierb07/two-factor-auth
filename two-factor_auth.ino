// IE-59000 Electromechanical Robotic Systems
// Two-Factor Authentication - Javier Belmonte

#include <Wire.h>                   // Include Wire library (required for I2C devices)
#include <LiquidCrystal_I2C.h>      // Include LiquidCrystal_I2C library 
#include <Servo.h>                  // Include servo library to handle servomotors

LiquidCrystal_I2C lcd(0x27, 16, 2); // Configure LiquidCrystal_I2C library with 0x27 address, 16 columns and 2 rows

char insertPass[] = "Insert Password"; // The fisrt string to print on the LCD
char welcome[] = "Welcome";            // String to welcome the right user
char wrongPass[] = "Wrong Password";   // String for wrong password
String password = "123";               // The password
String Name = "JAVIER";                // The string to hold the name of the user
String retName = "";                   // The string returned from Python program
String inputPass = "   ";              // The string to hold the input
String lastInput = "   ";
char checkingFace[] = "Checking face"; // The string to show if facial recogniztion is being done
char intruderRec[] = "Intruder!";      // The string to show if an intruder is detected
int buttonLeft = 11;      // Pin where the number decrement button will be attached
int buttonMid = 12;       // Pin where the number increment button will be attached
int buttonRight = 13;     // Pin where the change digit and submit button will be attached
// Declare variables for the states of the push buttons
bool buttonLeftState = LOW;
bool buttonMidState = LOW;
bool buttonRightState = LOW;
// Debouncing variables
unsigned long lastDebounceTime = 0;    // The last time the output pin was toggled
unsigned long debounceDelay = 50;      // The debounce time; increase if the output flickers
// Variables to keep track of the last state of the buttons
bool lastButtonLeftState = LOW;
bool lastButtonMidState = LOW;
bool lastButtonRightState = LOW;
int buzzer = 3;           // Pin where the buzzer will be attached
bool buzzState = true;
unsigned int frequency = 1000;         // Frequency for the buzzer 
int digCounter = 0;       // Counter to keep track of which digit it's been worked on
int digit = 0;            // Initialize the digit for the entered input
// Variables for refreshing the LCD
unsigned long lastRefreshTime = 0;  
unsigned long refreshTime  = 0;
long tim = 1000;
// Define the states that the System can bein
const int waiting = 100;        // Waiting for input from the user (password)
const int incorrectPass = 101;  // User has entered an incorrect password
const int correctPass = 102;    // User has entered a correct password
const int intruder = 103;       // An intruder has been detected
const int recognized = 104;     // The correct person has been recognized
int state = waiting;      // Set the initial state as waiting for input
Servo lock;               // Create servo object to control a servo
int servoPin = 5;          // Pin where the servo will be connected

void setup() {
  Serial.begin(9600);     // Initialize serial channel at 9600 bauds
  lcd.init();             // Initialize I2C LCD module
  lcd.backlight();        // Turn backlight ON
  lcd.clear();            // Clears the LCD screen and positions the cursor in the upper-left corner.
  lock.attach(servoPin);  // Attaches the servo on pin 5 to the servo object
  pinMode(buttonLeft, INPUT_PULLUP);  // Set the left button as input will pull-up resistor
  pinMode(buttonMid, INPUT_PULLUP);   // Set the middle button as input will pull-up resistor
  pinMode(buttonRight, INPUT_PULLUP); // Set the right button as input will pull-up resistor
  pinMode(buzzer, OUTPUT);            // Set the buzze as OUTPUT
}

void loop() {
  switch (state)  // Finite state machine to handle two-facot authentication
  {
    case waiting: // Waiting and receiving input from the user
      lock.write(0);
      lcd.setCursor(0,0);      // Set the cursor to column 0, line 0
      lcd.print(insertPass);   // Print the first line
      inputPass[digCounter] = intToChar(digit)[0];
      if (inputPass != lastInput)
      {
        lcd.clear();             // Clears the LCD screen and positions the cursor in the upper-left corner.
        lcd.setCursor(0,0);      // Set the cursor to column 0, line 0
        lcd.print(insertPass);   // Print the first line
        lcd.setCursor(0,1);      // Set the cursor to column 0, line 1
        lcd.print(inputPass);    // Print the input 
        lastInput = inputPass;
      }
      // Read the state of the buttons
      buttonLeftState = digitalRead(buttonLeft);
      buttonMidState = digitalRead(buttonMid);
      buttonRightState = digitalRead(buttonRight);
      // If the state of any of the buttons is now different from what it was (a button has been pressed)
      if( lastButtonLeftState != buttonLeftState || lastButtonMidState != buttonMidState || lastButtonRightState != buttonRightState){
        // Check if enough time has passed for debouncing
        if ((millis() - lastDebounceTime) > debounceDelay) {
          lastDebounceTime = millis();  // Update the last debounce time
          // If the left button is pressed decrement the number
          if (buttonLeftState == LOW) {
            digit--;
            if(digit<0) { digit = 9;} // Don't go to negative values
          } 
          // If the right button is pressed increment the number
          else if (buttonRightState == LOW)
          {
            digit++;
            if(digit>9) { digit = 0;} // Don't go to two digits
          }
          // If the middle button has been pressed go to the next digit until all digits are done and compare the input with the password
          else if (buttonMidState == LOW) { digCounter++; }
          if (digCounter >= 3) { 
            if (inputPass == password) { // If the password is correct
              state = correctPass;
              lcd.clear();               // Clears the LCD screen and positions the cursor in the upper-left corner.
              Serial.print("correct_pass");
            }  
            else {                       // If the password is incorrect
              state = incorrectPass;
              lcd.clear();               // Clears the LCD screen and positions the cursor in the upper-left corner.
            }                     
          }
        }
      }
      // Update the values of the last button state variables 
      lastButtonLeftState = buttonLeftState;
      lastButtonMidState = buttonMidState;
      lastButtonRightState = buttonRightState;    
      break;
    case incorrectPass:  // An incorrect password has been entered
        lcd.setCursor(0,0);      // Set the cursor to column 0, line 0
        lcd.print(wrongPass);    // Print the first line
        buzz();
      break;
    case correctPass:
      lcd.setCursor(0,0);        // Set the cursor to column 0, line 0
      lcd.print(checkingFace);   // Print the first line
      // Tell Python to check for face
      if (Serial.available())// If something is available in the serial channel
        {
          retName = Serial.readString();  // Gets one byte from serial buffer
          //retName = retName.substring(0,retName.length()-1);
          if (retName == Name) { state = recognized; }  // If the correct person was recognized
          else { state = intruder; }      // If an intruder was detected
          lcd.clear();                    // Clears the LCD screen 
        }
      break;
    case intruder:
      lcd.setCursor(0,0);      // Set the cursor to column 0, line 0
      lcd.print(intruderRec);  // Print the first line
      buzz();
      break;
    case recognized:
      lcd.setCursor(0,0);      // Set the cursor to column 0, line 0
      lcd.print(welcome);      // Print the first line
      lcd.setCursor(0,1);      // Set the cursor to column 0, line 1
      lcd.print(Name);         // Print the input 
      lock.write(180);
      if (digitalRead(buttonRight) == LOW) { // Reset the system
        state = waiting;
        digCounter = 0;
        inputPass = "   ";
        digit = 0;
      }
      break;
  }
}

void buzz(){
  refreshTime = millis();
  if(buzzState){
    if (refreshTime - lastRefreshTime > tim) { 
      lastRefreshTime = refreshTime;
      tone(buzzer, frequency); // Start to buzz
      buzzState = false;
    }
  }
  else {
    if(refreshTime - lastRefreshTime > tim) {
       lastRefreshTime = refreshTime;
       buzzState = true;
       noTone(buzzer);          // Stop sound...
    }
  }
  if (digitalRead(buttonRight) == LOW) { // Reset the system
    state = waiting;
    digCounter = 0;
    inputPass = "   ";
    digit = 0;
    noTone(buzzer);          // Stop sound...
  }
}

String intToChar(int digit){
  char b[1];
  String str;
  str=String(digit);
  str.toCharArray(b,1);
  return str;
}
