/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>

#define LEDC_CHANNEL_0     0
#define LEDC_CHANNEL_1     1

// use 13 bit precission for LEDC timer
#define LEDC_TIMER_13_BIT  9

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     490

#define LED_PIN   27
#define LED_PIN_2 25

int cutoffDuty = 0;
bool turboBool = false;
float turboDiv = 6;
float bckTurboDiv = turboDiv * 2.5;

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int ctrlX;
    //int ctrlY;
    //bool propOn;

    bool fwd;
    bool bck;
    bool both;
    bool turboA;
    bool turboB;
    
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
//  Serial.print(myData.ctrlX);
//  Serial.print(" : ");
//  Serial.print(myData.fwd);
//  Serial.print(" : ");
//  Serial.print(myData.bck);
//  Serial.print(" : ");
//  Serial.print(myData.turboA);
//  Serial.print(" : ");
//  Serial.println(myData.turboB);
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);
  
  ledcSetup(LEDC_CHANNEL_1, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN_2, LEDC_CHANNEL_1);
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
  ledcWrite(0, 307);
  ledcWrite(1, 307);
  delay(10000);
}
 //*pow(((float(myData.ctrlX-2048))/float(3096)),3)
void loop() {
  if (myData.turboA == true){
    if (myData.turboB == true){
      turboDiv = 1;
    } else {
      turboDiv = 3;
    }
  } else if (myData.turboB == true){
    
  } else {
    turboDiv = 6;
  }
  int bckTurboDiv = turboDiv * 2.5;
  
  if(myData.fwd == 1){
    if (myData.ctrlX > 2100){
      cutoffDuty = ((myData.ctrlX-2048)*pow(((float(myData.ctrlX-2048))/float(1050)),3))/float(2.05078125);
      if (cutoffDuty > 512) cutoffDuty = 512;
      //Serial.println(cutoffDuty);
      ledcWrite(0,307+(190/turboDiv));
      ledcWrite(1,307+int((190-(cutoffDuty)*float(0.357421875))/turboDiv));
    } else if (myData.ctrlX < 1700){
      cutoffDuty = ((myData.ctrlX-400)*pow(((float(myData.ctrlX-400))/float(1200)),3))/float(2.783203125);
      cutoffDuty = 512-cutoffDuty;
    if (cutoffDuty < 0) cutoffDuty = 0;
      ledcWrite(1,307+(190/turboDiv));
      ledcWrite(0,307+int((190-(cutoffDuty)*float(0.357421875))/turboDiv));
      //Serial.println(cutoffDuty);
    } else {
      cutoffDuty = 0;
      ledcWrite(0,307+(190/turboDiv));
      ledcWrite(1,307+(190/turboDiv));
    }
   } else if (myData.bck == 1){
      if (myData.ctrlX > 2100){
      cutoffDuty = ((myData.ctrlX-2048)*pow(((float(myData.ctrlX-2048))/float(1050)),3))/float(2.05078125);
      if (cutoffDuty > 512) cutoffDuty = 512;
      //Serial.println(cutoffDuty);
      ledcWrite(0,307-(190/bckTurboDiv));
      ledcWrite(1,307-int((190-(cutoffDuty)*float(0.357421875))/bckTurboDiv));
    } else if (myData.ctrlX < 1700){
      cutoffDuty = ((myData.ctrlX-400)*pow(((float(myData.ctrlX-400))/float(1200)),3))/float(2.783203125);
      cutoffDuty = 512-cutoffDuty;
    if (cutoffDuty < 0) cutoffDuty = 0;
      ledcWrite(1,307-(190/bckTurboDiv));
      ledcWrite(0,307-int((190-(cutoffDuty)*float(0.357421875))/bckTurboDiv));
      //Serial.println(cutoffDuty);
    } else {
      cutoffDuty = 0;
      ledcWrite(0,307-(190/bckTurboDiv));
      ledcWrite(1,307-(190/bckTurboDiv));
    }
    } else {
      ledcWrite(0,307);
      ledcWrite(1,307);
  }
  delay(5);
}
