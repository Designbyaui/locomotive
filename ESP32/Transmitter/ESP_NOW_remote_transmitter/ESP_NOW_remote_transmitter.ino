#include <esp_now.h>
#include <WiFi.h>
#include <ezButton.h>

#define VRX_PIN  1
#define VRY_PIN  2 
#define SW_PIN   0
// & 3.3V    & GND
ezButton button(SW_PIN);
#define LED_PIN  21

int xValue = 0;
int yValue = 0;
int bValue = 0; 
int flag = 0;

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x24,0xec,0x4a,0x2f,0x2a,0xf0};  //24:ec:4a:2f:2a:f0   ESP32-C3 Device1
//uint8_t broadcastAddress[] = {0x34,0x85,0x18,0x26,0xC6,0x88};  //34:85:18:26:C6:88   ESP32-C3 Device2

//uint8_t broadcastAddress[] = {0xC8,0xC9,0xA3,0xC9,0x14,0xEC};  //C8:C9:A3:C9:14:EC   ESP32-Wroom


// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int xVal;
  int yVal;
  int bVal;
} struct_message;

// Create a struct_message called myData & peer
struct_message myData;
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  status == ESP_NOW_SEND_SUCCESS ? digitalWrite(LED_PIN, HIGH) : digitalWrite(LED_PIN, LOW);
}
 
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  button.setDebounceTime(100);
  
  //pinMode(VRX_PIN, INPUT);
  //pinMode(VRY_PIN, INPUT);
  //pinMode(SW_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
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
  // Make values to send
  button.loop(); // MUST call the loop() function first

  // read analog X and Y analog values
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);

  myData.xVal = map(xValue, 0, 4095, 0, 255);
  myData.yVal = map(yValue, 0, 4095, 0, 255);
  
  //myData.xVal = xValue;
  //myData.yVal = yValue;
  //myData.xVal = analogRead(VRX_PIN);
  //myData.yVal = analogRead(VRY_PIN);


  // Read the button value
  bValue = button.getState();
  delay(20);
  myData.bVal = bValue;
  //myData.bVal = button.getState();


Serial.print(myData.xVal);Serial.print("  ");Serial.print(myData.yVal);Serial.print("  ");Serial.println(myData.bVal);
//Serial.println("");
  
  if (button.isPressed()) {
    delay(20);
    Serial.println("The button is pressed");
  }
  if (button.isReleased()) {
    Serial.println("The button is released");
  }

/*
  Serial.print(xValue);
  Serial.print("   ");
  Serial.print(yValue);
  Serial.print("   ");
  Serial.println(bValue);
  */
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(100);

}
