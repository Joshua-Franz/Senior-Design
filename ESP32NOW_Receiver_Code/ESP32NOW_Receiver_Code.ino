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

  if (shouldDispense) { // Kayak dispensing
    switch(kayakNumber) {
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
    switch(kayakNumber) {
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
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");  
}

void dispenseKayakOne() {
  sendData.isOperationCompleted = true;
}

void dispenseKayakTwo() {
  sendData.isOperationCompleted = true;
}

void dispenseKayakThree() {
  sendData.isOperationCompleted = true;
}

void dispenseKayakFour() {
  sendData.isOperationCompleted = true;
}

void dispenseKayakFive() {
  sendData.isOperationCompleted = true;
}

void dispenseKayakSix() {
  sendData.isOperationCompleted = true;
}

void retrieveKayakOne() {
  sendData.isOperationCompleted = true;
}

void retrieveKayakTwo() {
  sendData.isOperationCompleted = true;
}

void retrieveKayakThree() {
  sendData.isOperationCompleted = true;
}

void retrieveKayakFour() {
  sendData.isOperationCompleted = true;
}

void retrieveKayakFive() {
  sendData.isOperationCompleted = true;
}

void retrieveKayakSix() {
  sendData.isOperationCompleted = true;
}

void retractMagnet() {
  
}

void extendMagnet() {
  
}
