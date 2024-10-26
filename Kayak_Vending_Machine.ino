#include <LiquidCrystal_I2C.h>
#include <Key.h>
#include <Keypad.h>

// Set the correct I2C address and LCD dimensions
const byte LCD_ROWS = 4;
const byte LCD_COLS = 20;

LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS); // 0x27 is the default I2C address, change it if necessary

// create keypad
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {6, 5, 4, 3};
byte colPins[COLS] = {9, 8, 7};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// set pin numbers
const int ledPin = 13;
const int buttonPin = 12;

// initialize global variables
bool isKayakFree[6] = {true,true,true,true,true,true};
bool isMotorActive = false;
byte freeKayaks = 6;
byte kayakToRent = 0;
byte menu = 1;
byte previousMenu = 1;
/* ~~~~~~~ MENU KEY ~~~~~~~
 * 0: Main Menu
 * 1: Kayak Availability
 * 2: Rental Options
 * 3: Return A Kayak
 * 4: Actively Returning Kayak
 * 5: Problem Returning Kayak
 * 6: Actively Renting Kayak
 * 7: Problem Renting Kayak
 * 8: Vending Machine Retrieving Kayak
 * 9: Vending Machine Dispensing Kayak
 * 10: Awaiting Payment
 */

// ~~~~~~~ SETUP AND LOOP ~~~~~~~ //

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  lcd.init();            // Initialize the LCD
  lcd.backlight();       // Turn on the backlight
  showAvailableKayaks();
}

void loop() {
  readKeypad();
  readPaymentButton();
}

// ~~~~~~~ FUNCTIONS ~~~~~~~ //

void readKeypad() {
  char key = keypad.getKey();
  
  if (key) {
    switchMenu(key);    
    //Serial.println((String)"Key Pressed: " + key);
  }
}

void switchMenu(char keyName) {
  String keyPressed = (String)keyName;
  
  switch (menu) {
  case 0: { // Main Menu
      if (keyPressed == "1") {
        showAvailableKayaks();
      }
      else if (keyPressed == "2") {
        showRentalOptions();
      }
      else if (keyPressed == "3") {
        showKayakReturn();
      }
      
      break;
  } case 1: { // Kayak Availability
      if (keyPressed == "#") {
        showMainMenu();
      }
      else if (keyPressed == "1") {
        if (isKayakFree[0]) {
          showActivelyRentingKayak(1);
        }
        else {
          showActivelyReturningKayak(1);
        }
      }
      else if (keyPressed == "2") {
        if (isKayakFree[1]) {
          showActivelyRentingKayak(2);
        }
        else {
          showActivelyReturningKayak(2);
        }
      }
      else if (keyPressed == "3") {
        if (isKayakFree[2]) {
          showActivelyRentingKayak(3);
        }
        else {
          showActivelyReturningKayak(3);
        }
      }
      else if (keyPressed == "4") {
        if (isKayakFree[3]) {
          showActivelyRentingKayak(4);
        }
        else {
          showActivelyReturningKayak(4);
        }
      }
      else if (keyPressed == "5") {
        if (isKayakFree[4]) {
          showActivelyRentingKayak(5);
        }
        else {
          showActivelyReturningKayak(5);
        }
      }
      else if (keyPressed == "6") {
        if (isKayakFree[5]) {
          showActivelyRentingKayak(6);
        }
        else {
          showActivelyReturningKayak(6);
        }
      }
      
      break;
  } case 2: { // Rental Options
      if (keyPressed == "1") {
        if (isKayakFree[0]) {
          showActivelyRentingKayak(1);
        }
        else {
          showProblemRentingKayak(1);
        }
      }
      else if (keyPressed == "2") {
        if (isKayakFree[1]) {
          showActivelyRentingKayak(2);
        }
        else {
          showProblemRentingKayak(2);
        }
      }
      else if (keyPressed == "3") {
        if (isKayakFree[2]) {
          showActivelyRentingKayak(3);
        }
        else {
          showProblemRentingKayak(3);
        }
      }
      else if (keyPressed == "4") {
        if (isKayakFree[3]) {
          showActivelyRentingKayak(4);
        }
        else {
          showProblemRentingKayak(4);
        }
      }
      else if (keyPressed == "5") {
        if (isKayakFree[4]) {
          showActivelyRentingKayak(5);
        }
        else {
          showProblemRentingKayak(5);
        }
      }
      else if (keyPressed == "6") {
        if (isKayakFree[5]) {
          showActivelyRentingKayak(6);
        }
        else {
          showProblemRentingKayak(6);
        }
      }
      else if (keyPressed == "#") {
        showMainMenu();
      }
      
      break;
  } case 3: { // Return A Kayak
      if (keyPressed == "1") {
        if (!isKayakFree[0]) {
          showActivelyReturningKayak(1);
        }
        else {
          showProblemReturningKayak(1);
        }
      }
      else if (keyPressed == "2") {
        if (!isKayakFree[1]) {
          showActivelyReturningKayak(2);
        }
        else {
          showProblemReturningKayak(2);
        }
      }
      else if (keyPressed == "3") {
        if (!isKayakFree[2]) {
          showActivelyReturningKayak(3);
        }
        else {
          showProblemReturningKayak(3);
        }
      }
      else if (keyPressed == "4") {
        if (!isKayakFree[3]) {
          showActivelyReturningKayak(4);
        }
        else {
          showProblemReturningKayak(4);
        }
      }
      else if (keyPressed == "5") {
        if (!isKayakFree[4]) {
          showActivelyReturningKayak(5);
        }
        else {
          showProblemReturningKayak(5);
        }
      }
      else if (keyPressed == "6") {
        if (!isKayakFree[5]) {
          showActivelyReturningKayak(6);
        }
        else {
          showProblemReturningKayak(6);
        }
      }
      else if (keyPressed == "#") {
        showMainMenu();
      }
      
      break;
  } case 4: { // Actively Returning Kayak
      if (keyPressed == "*") {
        backButtonPressed();
      }
      else if (keyPressed == "0") {
        showRetrieving();
      }
      else if (keyPressed == "#") {
        showMainMenu();
      }
            
      break;
  } case 5: { // Problem Returning Kayak
      if (keyPressed == "*") {
        backButtonPressed();
      }
      else if (keyPressed == "#") {
        showMainMenu();
      }
            
      break;
  } case 6: { // Actively Renting Kayak
      const char * keyChar = keyPressed.c_str();
      
      if (atoi(keyChar) == kayakToRent) {
        showPaymentScreen();
      }
      else if (keyPressed == "*") {
        backButtonPressed();
      }
      else if (keyPressed == "#") {
        showMainMenu();
      }
            
      break;
  } case 7: { // Problem Renting Kayak
      if (keyPressed == "*") {
        backButtonPressed();
      }
      else if (keyPressed == "#") {
        showMainMenu();
      }
            
      break;
  } case 8: { // Vending Machine Retrieving Kayak
      if (!isMotorActive) {
        showAvailableKayaks();
      }
            
      break;
  } case 9: { // Vending Machine Dispensing Kayak
      if (!isMotorActive) {
        showAvailableKayaks();
      }
            
      break;
  } case 10: { // Awaiting Payment
      if (keyPressed == "*") {
        backButtonPressed();
      }
            
      break;
  } default: {
      break;
  }
  }
}

void showMainMenu() {
  previousMenu = menu;
  menu = 0;
  lcd.clear();
  lcd.print("1:AVAILABLE KAYAKS");
  lcd.setCursor(0, 1);
  lcd.print("2:RENT A KAYAK");
  lcd.setCursor(0, 2);
  lcd.print("3:RETURN A KAYAK");
}

void showAvailableKayaks() {
  previousMenu = menu;
  menu = 1;
  lcd.clear();
  lcd.print("  AVAILABLE KAYAKS  ");
  lcd.setCursor(0, 1);

  for (int kayak = 0; kayak < 3; kayak++)
  {
    lcd.print((kayak + 1) + (String)":");
    
    if (isKayakFree[kayak])
    {
      lcd.print("FREE");
    }
    else
    {
      lcd.print("GONE");
    }

    lcd.print(" ");
  }
  
  lcd.setCursor(0, 2);

  for (int kayak = 3; kayak < 5; kayak++)
  {
    lcd.print((kayak + 1) + (String)":");
    
    if (isKayakFree[kayak])
    {
      lcd.print("FREE");
    }
    else
    {
      lcd.print("GONE");
    }
    
    lcd.print(" ");
  }
  
  lcd.print("6:");
    
    if (isKayakFree[5])
    {
      lcd.print("FREE");
    }
    else
    {
      lcd.print("GONE");
    }
  
  lcd.setCursor(0, 3);
  lcd.print("  PRESS # FOR MENU  ");
}

void showRentalOptions() {
  previousMenu = menu;
  menu = 2;
  lcd.clear();

  if (freeKayaks) {
    lcd.print("ENTER THE NUMBER OF");
    lcd.setCursor(0, 1);
    lcd.print("THE KAYAK YOU WISH");
    lcd.setCursor(0, 2);
    lcd.print("TO RENT (1-6)");
  }
  else {
    lcd.print("NO AVAILABLE KAYAKS.");
    lcd.setCursor(0, 1);
    lcd.print("PLEASE WAIT UNTIL A");
    lcd.setCursor(0, 2);
    lcd.print("KAYAK IS RETURNED.");
  }
  
  lcd.setCursor(0, 3);
  lcd.print("  PRESS # FOR MENU  ");
}

void showKayakReturn() {
  previousMenu = menu;
  menu = 3;
  lcd.clear();

  if (freeKayaks == 6) {
    lcd.print("NO KAYAKS CURRENTLY");
    lcd.setCursor(0, 1);
    lcd.print("RENTED OUT. PLEASE");
    lcd.setCursor(0, 2);
    lcd.print("RENT A KAYAK FIRST.");
  }
  else {
    lcd.print("ENTER THE NUMBER OF");
    lcd.setCursor(0, 1);
    lcd.print("THE KAYAK YOU RENTED");
    lcd.setCursor(0, 2);
    lcd.print("(1-6)");
  }
  
  lcd.setCursor(0, 3);
  lcd.print("  PRESS # FOR MENU  ");
}

void showActivelyReturningKayak(byte kayakNumber) {
  previousMenu = menu;
  menu = 4;
  kayakToRent = kayakNumber;
  lcd.clear();
  lcd.print("Please place Kayak ");
  lcd.print(kayakNumber);
  lcd.setCursor(0, 1);
  lcd.print("in the return tray.");
  lcd.setCursor(0, 2);
  lcd.print("Press 0 when done.");
  lcd.setCursor(0, 3);
  lcd.print("*BACK          #MENU");
}

void showProblemReturningKayak(byte kayakNumber) {
  previousMenu = menu;
  menu = 5;
  lcd.clear();
  lcd.print("Unable to return");
  lcd.setCursor(0, 1);
  lcd.print("kayak ");
  lcd.print(kayakNumber);
  lcd.print(" since it not");
  lcd.setCursor(0, 2);
  lcd.print("currently rented.");
  lcd.setCursor(0, 3);
  lcd.print("*BACK          #MENU");
}

void showActivelyRentingKayak(byte kayakNumber) {
  previousMenu = menu;
  menu = 6;
  kayakToRent = kayakNumber;
  lcd.clear();
  lcd.print("Renting Kayak ");
  lcd.print(kayakNumber);
  lcd.setCursor(0, 1);
  lcd.print("Press ");
  lcd.print(kayakNumber);
  lcd.print(" again");
  lcd.setCursor(0, 2);
  lcd.print("to confirm.");
  lcd.setCursor(0, 3);
  lcd.print("*BACK          #MENU");
}

void showProblemRentingKayak(byte kayakNumber) {
  previousMenu = menu;
  menu = 7;
  lcd.clear();
  lcd.print("Cannot rent kayak ");
  lcd.print(kayakNumber);
  lcd.setCursor(0, 1);
  lcd.print("since it is already");
  lcd.setCursor(0, 2);
  lcd.print("rented.");
  lcd.setCursor(0, 3);
  lcd.print("*BACK          #MENU");
}

void showRetrieving() {
  isMotorActive = true;
  previousMenu = menu;
  menu = 8;
  lcd.clear();
  lcd.print("Retrieving kayak...");
  
  retrieveKayak();

  if (isReturnCorrect())
  {
    freeKayaks += 1;
    isKayakFree[kayakToRent - 1] = true;
    kayakToRent = 0;
    lcd.clear();
    lcd.print("Kayak successfully");
    lcd.setCursor(0, 1);
    lcd.print("returned. Press any");
    lcd.setCursor(0, 2);
    lcd.print("key to continue.");
  }
  else
  {
    dispenseKayak();
    
    lcd.clear();
    lcd.print("Incorrect kayak");
    lcd.setCursor(0, 1);
    lcd.print("returned. Press any");
    lcd.setCursor(0, 2);
    lcd.print("key to return to");
    lcd.setCursor(0, 3);
    lcd.print("availability screen.");
  }
  
  isMotorActive = false;
}

void showDispensing() {
  isMotorActive = true;
  previousMenu = menu;
  menu = 9;
  lcd.clear();
  lcd.print("Dispensing kayak...");
  
  dispenseKayak();
  
  freeKayaks -= 1;
  isKayakFree[kayakToRent - 1] = false;
  kayakToRent = 0;
  lcd.clear();
  lcd.print("Kayak successfully");
  lcd.setCursor(0, 1);
  lcd.print("rented. Press any");
  lcd.setCursor(0, 2);
  lcd.print("key to continue.");
  isMotorActive = false;
}

void showPaymentScreen() {
  previousMenu = menu;
  menu = 10;
  lcd.clear();
  lcd.print("Press red button");
  lcd.setCursor(0, 1);
  lcd.print("to pay for kayak");
}

void backButtonPressed() {
  //Serial.println(previousMenu);
  switch (previousMenu) {
  case 1: {
      showAvailableKayaks();
      break;
  } case 2: {
      showRentalOptions();
      break;
  } case 3: {
      showKayakReturn();
      break;
  } default: {
      break;
  }
  }
}

void readPaymentButton() {
  bool paymentButtonState = digitalRead(buttonPin);
  //Serial.println((String)"Button State: " + paymentButtonState);

  if (paymentButtonState == HIGH) {
    // turn LED on
    digitalWrite(ledPin, HIGH);

    if (menu == 10)
    {
      showDispensing();
    }
  }
  else {
    // turn LED off
    digitalWrite(ledPin, LOW);
  }
}

void dispenseKayak() {
  // placeholder for the vending machine dispensing kayak
  // to the user after they "pay" to rent it
  delay(1000);
}

void retrieveKayak() {
  // placeholder for the vending machine retrieving kayak
  // once user places it on the return tray and presses "0"
  delay(1000);
}

bool isReturnCorrect() {
  // placeholder for the vending machine checking to see if
  // the kayak the user returned is indeed the correct one
  // (via QR code)
  delay(1000);
  return 1;
}
