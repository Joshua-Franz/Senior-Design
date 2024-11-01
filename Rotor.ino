// Pin assignments
const int zeroSwitchPin = 11;         // Zero switch connected to D9
const int RotorStepPin = 2;          // Step pin connected to pin 2
const int RotorDirPin = 5;           // Direction pin connected to pin 5
const int RotorEnablePin = 6;        // Enable pin to control stepper motor power
const int switch1Pin = 7;   // Limit switch 1 connected to D7
const int switch2Pin = 8;   // Limit switch 2 connected to D8
const int extendPin = 9;    // Extend action pin connected to D9
const int stepPin = 3;      // Step pin connected to pin 3
const int dirPin = 4;       // Direction pin connected to pin 4
const int enablePin = 10;   // Enable pin to control stepper motor power
const int Relay = 12;

// Variables to store switch states
int switch1State = HIGH;    // HIGH means unpressed (assuming pull-up configuration)
int switch2State = HIGH;    // HIGH means unpressed (assuming pull-up configuration)
int extendState = HIGH;     // HIGH means unpressed (assuming pull-up configuration)
const int ExtendstepDelay = 1000; // Delay between steps in microseconds

// Stepper motor setup
const int positions = 6;             // Number of desired positions (1 to 6)
int currentPosition = 0;             // Current position (0 = zero position)

// Step counts relative to zero position
int stepsToPosition[positions] = {21, 60, 85, 120, 180, 215}; 

// State variable to check if angle is zeroed
bool isZeroed = false;

// Slow startup step delay
const int slowStartDelay = 4000; // Delay for slow startup rotation in microseconds

// Normal stepping delay
const int stepDelay = 4000; // Delay for normal stepping in microseconds

void setup() {
  // Set up the serial monitor
  Serial.begin(9600);

  // Set up zero switch pin as input with internal pull-up resistor
  pinMode(switch1Pin, INPUT_PULLUP);
  pinMode(switch2Pin, INPUT_PULLUP);
  pinMode(extendPin, INPUT_PULLUP);
  pinMode(zeroSwitchPin, INPUT_PULLUP);

  // Set up stepper motor pins as outputs
  pinMode(RotorStepPin, OUTPUT);
  pinMode(RotorDirPin, OUTPUT);
  pinMode(RotorEnablePin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(Relay, OUTPUT);
  // Set a fixed direction (only one direction is allowed)
  digitalWrite(RotorDirPin, HIGH); // Set direction to a constant

  // Disable stepper motor at the beginning (motor off)
  digitalWrite(RotorEnablePin, HIGH);  // HIGH typically disables the motor
  digitalWrite(enablePin, HIGH);  // HIGH typically disables the motor (check your driver)  
 
  // Start rotating the motor until zero switch is pressed
  rotateUntilZeroed();
  


  
  Serial.println("Motor is zeroed. Enter a new position number (1 to 6) to move to.");
}
//rotateUntilZeroed
//moveToAngle
//MagnetOn
//MagnetOff
//extendAndRetract
//moveStepper
//moveToPosition1
//moveToPosition2
//moveToPosition3
//moveToPosition4
//moveToPosition5
//moveToPosition6
void loop() {
  // Read the state of the Extend button
  extendState = digitalRead(extendPin);

  // Call extendAndRetract function if Extend button is pressed
  if (extendState == LOW) {
    extendAndRetract();
  }

  // Check for serial input
  if (Serial.available() > 0) {
    String inputString = Serial.readStringUntil('\n'); // Read input as a string until newline
    inputString.trim(); // Remove any extra whitespace

    // Call the appropriate function based on input
    if (inputString == "moveToPosition1") {
      moveToPosition1();
    } else if (inputString == "moveToPosition2") {
      moveToPosition2();
    } else if (inputString == "moveToPosition3") {
      moveToPosition3();
    } else if (inputString == "moveToPosition4") {
      moveToPosition4();
    } else if (inputString == "moveToPosition5") {
      moveToPosition5();
    } else if (inputString == "moveToPosition6") {
      moveToPosition6();
    } else if (inputString == "rotateUntilZeroed") {
      rotateUntilZeroed();
    } else if (inputString == "extendAndRetract") {
      extendAndRetract();
    } else if (inputString == "MagnetOn") {
      MagnetOn();
      Serial.println("Magnet is ON.");
    } else if (inputString == "MagnetOff") {
      MagnetOff();
      Serial.println("Magnet is OFF.");
    } else {
      Serial.println("Invalid command! Available commands are:");
      Serial.println("moveToPosition1, moveToPosition2, moveToPosition3, moveToPosition4, moveToPosition5, moveToPosition6");
      Serial.println("rotateUntilZeroed, extendAndRetract, MagnetOn, MagnetOff");
    }
  }
}


// Function to rotate the motor until the zero switch is pressed
void rotateUntilZeroed() {
  digitalWrite(RotorEnablePin, LOW); // Enable the motor

  // Rotate continuously until the switch is pressed
  while (digitalRead(zeroSwitchPin) == HIGH) {
    // Step the motor slowly
    digitalWrite(RotorStepPin, HIGH);
    delayMicroseconds(slowStartDelay); // Longer delay for slow rotation
    digitalWrite(RotorStepPin, LOW);
    delayMicroseconds(slowStartDelay); // Longer delay for slow rotation
  }
  
  // Disable the motor after zeroing
  delay(500);
  digitalWrite(RotorEnablePin, HIGH);
  isZeroed = true; // Mark as zeroed
  currentPosition = 0; // Set current position to zero
  Serial.println("Zero switch pressed. Motor zeroed.");
}

// Function to move the motor to a specific position
void moveToAngle(int targetSteps) {
  // Ensure the motor is zeroed before moving
  if (!isZeroed) {
    rotateUntilZeroed();
  }

  // Enable the motor
  digitalWrite(RotorEnablePin, LOW);

  // Step the motor the required number of steps
  for (int i = 0; i < targetSteps; i++) {
    digitalWrite(RotorStepPin, HIGH);
    delayMicroseconds(stepDelay); // Regular delay for stepping
    digitalWrite(RotorStepPin, LOW);
    delayMicroseconds(stepDelay); // Regular delay for stepping
  }

  // Disable the motor after movement
  delay(500);
  digitalWrite(RotorEnablePin, HIGH);

  // Reset isZeroed to false to force re-zeroing on the next move
  isZeroed = false;
}

// Individual functions to move to each position based on the steps required
void moveToPosition1() {
  moveToAngle(stepsToPosition[0]); // Move to position 1
  Serial.println("Moved to position 1.");
}
void moveToPosition2() {
  moveToAngle(stepsToPosition[1]); // Move to position 2
  Serial.println("Moved to position 2.");
}
void moveToPosition3() {
  moveToAngle(stepsToPosition[2]); // Move to position 3
  Serial.println("Moved to position 3.");
}
void moveToPosition4() {
  moveToAngle(stepsToPosition[3]); // Move to position 4
  Serial.println("Moved to position 4.");
}
void moveToPosition5() {
  moveToAngle(stepsToPosition[4]); // Move to position 5
  Serial.println("Moved to position 5.");
}
void moveToPosition6() {
  moveToAngle(stepsToPosition[5]); // Move to position 6
  Serial.println("Moved to position 6.");
}
void moveStepper() {
  // Make one step
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(ExtendstepDelay);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(ExtendstepDelay);
}
void extendAndRetract() {
  Serial.println("Starting extend and retract");

  // Enable the stepper motor
  digitalWrite(enablePin, LOW);  // LOW typically enables the motor (check your driver)

  // Set direction to extend (move forward)
  digitalWrite(dirPin, HIGH);

  // Move the motor forward until switch2 is pressed
  switch2State = digitalRead(switch2Pin);
  while (switch2State == LOW) {
    moveStepper();
    switch2State = digitalRead(switch2Pin);
  }

  // Stop for 1 second at the end
  delay(1000);

  // Reverse direction to retract
  Serial.println("Retracting");
  digitalWrite(dirPin, LOW); // Set direction to retract

  // Move the motor backward until switch1 is pressed
  switch1State = digitalRead(switch1Pin);
  while (switch1State == LOW) {
    moveStepper();
    switch1State = digitalRead(switch1Pin);
  }

  // Disable the stepper motor once done retracting
  digitalWrite(enablePin, HIGH);  // Disable the motor (motor off)
  Serial.println("Cycle complete");
}
void MagnetOn(){
  digitalWrite(Relay, HIGH);
}
void MagnetOff(){
   digitalWrite(Relay, LOW);
}
