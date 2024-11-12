#include <esp_now.h>
#include <WiFi.h>

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// WIFI TRANSMITTING SETUP
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// MAC Address of the UI/LCD ESP32
uint8_t broadcastAddress[] = {0x08, 0xA6, 0xF7, 0xB0, 0xC8, 0xE8};

// Structure containing the type of data sent to receiver ESP32
typedef struct send_message {
  //bool isGoodToGo;            //motor side ready to send/receive
  bool isOperationCompleted;  //motors finished requested operation
  int errorCode;              //0 = no errors. supports debugging
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
    bool shouldDispense;
    int kayakNumber;
} receive_message;

// Create a receive_message called recvData
receive_message recvData;

// Pin assignments
const int zeroSwitchPin = 34;         // Zero switch connected to D9
const int RotorStepPin = 22;          // Step pin connected to pin 2
const int RotorDirPin = 21;           // Direction pin connected to pin 5
const int RotorEnablePin = 23;        // Enable pin to control stepper motor power
const int switch1Pin = 35;   // Limit switch 1 connected to D7
const int switch2Pin = 32;   // Limit switch 2 connected to D8
const int extendPin = 9;    // Extend action pin connected to D9
const int stepPin = 18;      // Step pin connected to pin 3
const int dirPin = 13;       // Direction pin connected to pin 4
const int enablePin = 19;   // Enable pin to control stepper motor power
const int Relay = 12;
// Stepper motor setup
const int slowStartDelay = 6000;      // Delay for slow startup rotation in microseconds
const int StepDelay = 4000;
// Step counts relative to zero position (You can change these based on your setup)
int stepsToPosition[6] = {21, 60, 85, 120, 180, 215};  // Positions (relative to zero)

// Function to rotate the motor until the zero switch is pressed


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&recvData, incomingData, sizeof(recvData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Bool: ");
  Serial.println(recvData.shouldDispense);
  Serial.print("Int: ");
  Serial.println(recvData.kayakNumber);
  Serial.println();

  sendData.isOperationCompleted = false;
  sendData.errorCode = 0;

  if (recvData.shouldDispense) { // Kayak dispensing
    switch(recvData.kayakNumber) {
    case 1: {
      dispenseKayakOne();
      break;
    } case 2: {
      dispenseKayakTwo();
      break;
    } case 3: {
      dispenseKayakThree();
      break;
    } case 4: {
      dispenseKayakFour();
      break;
    } case 5: {
      dispenseKayakFive();
      break;
    } case 6: {
      dispenseKayakSix();
      break;
    } default: {
      break;
    }
    }
  } else { // Kayak retrieval
    switch(recvData.kayakNumber) {
    case 1: {
      retrieveKayakOne();
      break;
    } case 2: {
      retrieveKayakTwo();
      break;
    } case 3: {
      retrieveKayakThree();
      break;
    } case 4: {
      retrieveKayakFour();
      break;
    } case 5: {
      retrieveKayakFive();
      break;
    } case 6: {
      retrieveKayakSix();
      break;
    } default: {
      break;
    }
    }
  }
}
 
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // Set device as a Wi-Fi Station
  pinMode(switch1Pin, INPUT_PULLUP);
  pinMode(switch2Pin, INPUT_PULLUP);
  pinMode(extendPin, INPUT_PULLUP);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(Relay, OUTPUT);
  pinMode(zeroSwitchPin, INPUT_PULLUP);  // Set zeroSwitchPin as INPUT with pull-up enabled
  pinMode(RotorStepPin, OUTPUT);  // Set step pin as OUTPUT
  pinMode(RotorDirPin, OUTPUT);   // Set direction pin as OUTPUT
  pinMode(RotorEnablePin, OUTPUT); // Set enable pin as OUTPUT

  digitalWrite(RotorDirPin, HIGH);  // Set direction to a constant
  digitalWrite(enablePin, HIGH);
  digitalWrite(RotorEnablePin, HIGH);  // HIGH typically disables the motor

  Serial.println("System initialized. Waiting for commands.");

  if (esp_now_init() != ESP_OK) {
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

  sendData.isOperationCompleted = false;
  sendData.errorCode = 0;
}
 
void loop() {
  delay(2000);
  sendData.errorCode = sendData.errorCode + 1;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendData, sizeof(sendData));
  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }
    // Check if there's data available on the serial monitor
  if (Serial.available() > 0) {
    String inputString = Serial.readStringUntil('\n'); // Read input as a string until newline
    inputString.trim();  // Remove any extra whitespace

    // Process the input and move the motor to the corresponding position
    if (inputString == "moveToPosition1") {
      moveToPosition(stepsToPosition[0]);
    } else if (inputString == "moveToPosition2") {
      moveToPosition(stepsToPosition[1]);
    } else if (inputString == "moveToPosition3") {
      moveToPosition(stepsToPosition[2]);
    } else if (inputString == "moveToPosition4") {
      moveToPosition(stepsToPosition[3]);
    } else if (inputString == "moveToPosition5") {
      moveToPosition(stepsToPosition[4]);
    } else if (inputString == "moveToPosition6") {
      moveToPosition(stepsToPosition[5]);
    } else if (inputString == "zero") {
      rotateUntilZeroed();  // Call to zero the motor
    } else {
      Serial.println("Invalid command! Available commands are:");
      Serial.println("moveToPosition1, moveToPosition2, moveToPosition3, moveToPosition4, moveToPosition5, moveToPosition6");
      Serial.println("zero");
    }
  }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");  
}

void dispenseKayakOne() {
  moveToPosition(stepsToPosition[0]);
  delay(500);
  MagnetOn();
  delay(500);
  extend();
  delay(500);
  MagnetOff();
  delay(500);
  retract();
  delay(500);
  sendData.isOperationCompleted = true;
   
}

void dispenseKayakTwo() {
  moveToPosition(stepsToPosition[1]);
  sendData.isOperationCompleted = true;
}

void dispenseKayakThree() {
  moveToPosition(stepsToPosition[2]);
  sendData.isOperationCompleted = true;
}

void dispenseKayakFour() {
  moveToPosition(stepsToPosition[3]);
  sendData.isOperationCompleted = true;
}

void dispenseKayakFive() {
  moveToPosition(stepsToPosition[4]);
  sendData.isOperationCompleted = true;
}

void dispenseKayakSix() {
  moveToPosition(stepsToPosition[5]);
  sendData.isOperationCompleted = true;
}

void retrieveKayakOne() {
  moveToPosition(stepsToPosition[0]);
    extend();
  delay(500);
  MagnetOn();
  delay(500);
  retract();
  delay(500);
  MagnetOff();
  sendData.isOperationCompleted = true;
}

void retrieveKayakTwo() {
  moveToPosition(stepsToPosition[1]);
    extend();
  delay(500);
  MagnetOn();
  delay(500);
  retract();
  delay(500);
  MagnetOff();
  sendData.isOperationCompleted = true;
}

void retrieveKayakThree() {
  moveToPosition(stepsToPosition[2]);
    extend();
  delay(500);
  MagnetOn();
  delay(500);
  retract();
  delay(500);
  MagnetOff();
  sendData.isOperationCompleted = true;
}

void retrieveKayakFour() {
  moveToPosition(stepsToPosition[3]);
  extend();
  delay(500);
  MagnetOn();
  delay(500);
  retract();
  delay(500);
  MagnetOff();
  sendData.isOperationCompleted = true;
}

void retrieveKayakFive() {
  moveToPosition(stepsToPosition[4]);
  extend();
  delay(500);
  MagnetOn();
  delay(500);
  retract();
  delay(500);
  MagnetOff();
  sendData.isOperationCompleted = true;
}

void retrieveKayakSix() {
  moveToPosition(stepsToPosition[5]);
  extend();
  delay(500);
  MagnetOn();
  delay(500);
  retract();
  delay(500);
  MagnetOff();
  sendData.isOperationCompleted = true;
}

void retractMagnet() {
  
}

void extendMagnet() {
  
}
void rotateUntilZeroed() {
  Serial.println("Rotating to zero position...");

  digitalWrite(RotorEnablePin, LOW);  // Enable the motor

  // Rotate continuously until the switch is pressed (switch should be LOW)
  while (digitalRead(zeroSwitchPin) == HIGH) {
    // Step the motor slowly
    digitalWrite(RotorStepPin, HIGH);
    delayMicroseconds(slowStartDelay);  // Longer delay for slow rotation
    digitalWrite(RotorStepPin, LOW);
    delayMicroseconds(slowStartDelay);  // Longer delay for slow rotation
  }

  // Disable the motor after zeroing
  delay(500);
  digitalWrite(RotorEnablePin, HIGH);
  Serial.println("Zero switch pressed. Motor zeroed.");
}

// Function to rotate the motor to a specific position (number of steps)
void moveToPosition(int targetSteps) {
  // Zero the motor before every move
  rotateUntilZeroed();  // Zero the motor if not already zeroed

  Serial.print("Moving to position: ");
  Serial.println(targetSteps);

  digitalWrite(RotorEnablePin, LOW);  // Enable the motor

  // Set direction based on the target position
  if (targetSteps > 0) {
    digitalWrite(RotorDirPin, HIGH);  // Rotate in a specific direction
  } else {
    digitalWrite(RotorDirPin, LOW);   // Rotate in the opposite direction
  }

  // Move the motor to the target position
  for (int i = 0; i < abs(targetSteps); i++) {
    digitalWrite(RotorStepPin, HIGH);
    delayMicroseconds(slowStartDelay);  // Regular delay for stepping
    digitalWrite(RotorStepPin, LOW);
    delayMicroseconds(slowStartDelay);  // Regular delay for stepping
  }

  // Leave the motor enabled for 500ms (half a second) after moving
  delay(500);  // Delay to leave motor on

  // Disable the motor after the delay
  digitalWrite(RotorEnablePin, HIGH);

  Serial.print("Motor moved to position: ");
  Serial.println(targetSteps);
}
void retract() {
  Serial.println("Retracting");

  digitalWrite(enablePin, LOW); // Enable the motor
  digitalWrite(dirPin, LOW); // Set direction to retract

  // Move backward until switch1 is pressed
  while (digitalRead(switch1Pin) == HIGH) {
    moveStepper();
  }

  delay(500);  // Pause briefly after reaching the end
  digitalWrite(enablePin, HIGH); // Disable the motor
  Serial.println("Retracted to the limit.");
}
void extend() {
  Serial.println("Extending");

  digitalWrite(enablePin, LOW); // Enable the motor
  digitalWrite(dirPin, HIGH); // Set direction to extend

  // Move forward until switch2 is pressed
  while (digitalRead(switch2Pin) == HIGH) {
    moveStepper();
  }

  delay(500);  // Pause briefly after reaching the end
  digitalWrite(enablePin, HIGH); // Disable the motor
  Serial.println("Extended to the limit.");
}
void MagnetOff(){
   digitalWrite(Relay, LOW);
}
void MagnetOn(){
  digitalWrite(Relay, HIGH);
}
void moveStepper() {
  // Make one step
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(StepDelay);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(StepDelay);
}
