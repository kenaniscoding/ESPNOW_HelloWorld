#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
// MAC Address of responder - edit as required
// tx mac address
uint8_t broadcastAddress[] = {0x58, 0x8C, 0x81, 0xA9, 0x6A, 0xB0};
//uint8_t broadcastAddress[] = {0x94,0xA9,0x90,0x6D,0xB7,0x10};
//uint8_t broadcastAddress[] = {0x58,0x8C,0x81,0xA9,0x6A,0xB0};
// Must match the receiver structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
// FIXED: Updated first parameter type to esp_now_send_info_t to match ESP32 Core v3.x.x
void OnDataSent(const esp_now_send_info_t *info, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Transmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Clear peerInfo memory layout to prevent random garbage fields 
  // from breaking peer registration or causing "Delivery Fail" bugs.
  memset(&peerInfo, 0, sizeof(peerInfo));
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  Serial.print("ESP32C3 TX Board MAC Address: ");
  Serial.println(WiFi.macAddress());
}
 
void loop() {
  // Set values to send
  strcpy(myData.a, "I'm alive");
  myData.b = random(1, 20);
  myData.c = 1.2;
  myData.d = false;
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}