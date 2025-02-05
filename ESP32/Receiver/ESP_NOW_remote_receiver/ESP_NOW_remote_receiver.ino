// ESP-NOW  receiver + motors

#include <esp_now.h>
#include <WiFi.h>
#include <Stepper.h>
#define STEPS 100
Stepper stepper(STEPS, 3, 4, 5, 6);

//motor
#define A1 1    
#define A2 2 
#define FRQ 300000 //analogfreq
unsigned int delaytime = 10;
int previous = 0;


typedef struct struct_message {
    int xVal;
    int yVal;
    int bVal;
} struct_message;
struct_message myData;


const int DEBOUNCE_DELAY = 50;   // the debounce time; increase if the output flickers

// button debounce
int lastSteadyState = LOW;
int lastState = LOW;
unsigned long lastDebounceTime = 0;
int flag = 0;


 void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  
  commands();
}
 
void setup() {
      Serial.begin(115200);
  //motor
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);
   // analogWriteFrequency(A1,FRQ);
   // analogWriteFrequency(A2,FRQ);
    stepper.setSpeed(30);


  
 // Serial.begin(115200);
  delay(100);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}


void loop() {
}
void commands() {
  int val = 0;
 if (myData.xVal < 120){
  val = constrain(120-myData.xVal,0,120);
  (Serial.print("Reverse"));
  (Serial.println(val));
  analogWrite(A1,map(val,0,120,0,55));
  analogWrite(A2,0); 
 }
 else if (myData.xVal > 130){
  val = constrain(myData.xVal-130,0,120);
  (Serial.print("Forward"));
  (Serial.println(val));
  analogWrite(A2,map(val,0,120,0,55));
  analogWrite(A1,0); 
 }
 else{
  (Serial.println("STOP"));
  analogWrite(A2,0);
  analogWrite(A1,0); 
 }
if (myData.yVal < 120){
 stepper.step(-constrain(120-myData.yVal,0,120));
 
}
else if (myData.yVal > 130){
  stepper.step(constrain(myData.yVal-130,0,120));
}
  digitalWrite(3,LOW);
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);   
}
