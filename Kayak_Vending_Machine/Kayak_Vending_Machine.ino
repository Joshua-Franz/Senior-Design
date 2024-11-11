#include <esp_now.h>
#include <LiquidCrystal_I2C.h>
#include <Key.h>
#include <Keypad.h>
#include <WiFi.h>

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// WIFI TRANSMITTING SETUP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// MAC Address of the motor-controlling ESP32
uint8_t broadcastAddress[] = {0x2C, 0xBC, 0xBB, 0x4B, 0xF3, 0x40};

// Structure containing the type of data sent to receiver ESP32
typedef struct send_message {
  bool shouldDispense;  //if false, should retrieve
  int kayakNumber;      //which kayak slot (1-6) to operate on
} send_message;

// Variable to store the message values
send_message sendData;

// Variable to store info about receiver ESP32
esp_now_peer_info_t peerInfo;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// WIFI RECEIVING SETUP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Structure to receive data must match the sender structure
typedef struct receive_message {
  //bool isGoodToGo;
  bool isOperationCompleted;
  int errorCode;
} receive_message;

// Create a receive_message called recvData
receive_message recvData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&recvData, incomingData, sizeof(recvData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  //Serial.print("Bool: ");
  //Serial.println(recvData.isGoodToGo);
  Serial.print("Bool: ");
  Serial.println(recvData.isOperationCompleted);
  Serial.print("Int: ");
  Serial.println(recvData.errorCode);
  Serial.println();
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// LCD DISPLAY SETUP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Set the correct I2C address and LCD dimensions
const byte LCD_ROWS = 4;
const byte LCD_COLS = 20;
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS); // 0x27 is the default I2C address, change it if necessary

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// KEYPAD SETUP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// create keypad
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {26, 27, 14, 13};
byte colPins[COLS] = {32, 33, 25};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// set pin numbers
const int ledPin = 19;
const int buttonPin = 18;

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
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // Set device as a Wi-Fi Station

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register send and receive behavior
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  
  sendData.shouldDispense = true;
  sendData.kayakNumber = 0;
  
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  lcd.init();            // Initialize the LCD
  lcd.backlight();       // Turn on the backlight
  //showAvailableKayaks();
}

void loop() {
  //readKeypad();
  //readPaymentButton();

  sendData.kayakNumber = sendData.kayakNumber + 1;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendData, sizeof(sendData));
  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }
  delay(2000);
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
  
  retrieveKayak(kayakToRent);

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
    dispenseKayak(kayakToRent);
    
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
  
  dispenseKayak(kayakToRent);
  
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

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");  
}

void dispenseKayak(int kayakNum) {
  recvData.isOperationCompleted = false;
  sendData.shouldDispense = true;
  sendData.kayakNumber = kayakNum;

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendData, sizeof(sendData));
  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }

  while(!recvData.isOperationCompleted) {
    delay(100);
  }

  if (sendData.errorCode) {
    Serial.println("error dispensing kayak");
    return;
  } else {
    Serial.println("kayak dispensed successfully");
  }
}

void retrieveKayak(int kayakNum) {
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
