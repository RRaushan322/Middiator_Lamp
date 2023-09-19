#include <esp_now.h>
#include <WiFi.h>

// Variables for test data
  int led_status=0;
  int datarev=0;
  int change_data=0;
#define LED_PIN 5
#define BUTTON_PIN 4
byte lastButtonState = LOW;
byte ledState = LOW;
unsigned long debounceDuration = 50; // millis
unsigned long lastTimeButtonStateChanged = 0;

// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0xEC, 0xFA, 0xBC, 0x5E, 0xDD, 0xC6};

// Define a data structure
typedef struct struct_message {
  char a[32];
  int b;
} struct_message;

// Create a structured object
struct_message myData1;
struct_message myData2;

// Peer info
esp_now_peer_info_t peerInfo;

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  datarev =0;
}
void ledchange(){
  digitalWrite(LED_PIN, ledState);
  change_data=0;
}
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData2, incomingData, sizeof(myData2));
  Serial.print("Data received: ");
  Serial.println(len);
  Serial.print("Character Value: ");
  Serial.println(myData2.a);
  Serial.print("Integer Value: ");
  Serial.println(myData2.b);
  datarev =1;
}

void setup() {
  
  // Set up Serial Monitor
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {

  // Create test data
   if (millis() - lastTimeButtonStateChanged > debounceDuration) {
    byte buttonState = digitalRead(BUTTON_PIN);
    if (buttonState != lastButtonState) {
      lastTimeButtonStateChanged = millis();
      lastButtonState = buttonState;
      if (buttonState == LOW) {
        ledState = (ledState == HIGH) ? LOW: HIGH;
        change_data=1;
      }
    }
  }
  // Generate a random integer


  
  // Format structured data
  strcpy(myData1.a, "NODE B");
  if(digitalRead(LED_PIN)==HIGH){
    led_status=1;
  }
  else if(digitalRead(LED_PIN)==LOW){
    led_status=0;
  }
  myData1.b = led_status;
  
  // Send message via ESP-NOW
 if(change_data==1){
 ledchange();
 esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData1, sizeof(myData1));
}
esp_now_register_recv_cb(OnDataRecv);

 if(datarev ==1){ // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData2, sizeof(myData2));
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
  delay(2000);
}
}