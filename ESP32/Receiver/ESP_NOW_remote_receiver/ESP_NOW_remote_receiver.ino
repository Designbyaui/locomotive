// ESP-NOW  receiver + motors

#include <esp_now.h>  
#include <WiFi.h>
#include <Stepper.h>
#define STEPS 360 //steering stepper motor steps from one end to another end
#define MAX_SPEED 255 // drive motor max speed 0-255 
#define RGB_BUILTIN 21 // Built in RGB LED pin
Stepper stepper(STEPS, 3, 4, 5, 6); // Init Steering stepper motor on pins 3-6

//drive motor
#define A1 1    
#define A2 2 
#define FRQ 30000 //analogfreq to help with PWM noise, did not help a lot thogh 

int previous = 125; //Middle point of steering stepper. Used to keep track of position
long stepsglobal = 0; //Stepper debug value

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
      Serial.begin(115200); //Debug serial
  //motor
    pinMode(A1, OUTPUT); //Drive motor outputs
    pinMode(A2, OUTPUT); //Drive motor outputs
    pinMode(RGB_BUILTIN, OUTPUT);
   // analogWriteFrequency(A1,FRQ); //analogfreq to help with PWM noise, did not help a lot thogh 
   // analogWriteFrequency(A2,FRQ);
    stepper.setSpeed(40); //Set steering motor speed
  
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
#ifdef RGB_BUILTIN
  rgbLedWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0);  // Red
  delay(100);
  rgbLedWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0);  // Green
  delay(100);
  rgbLedWrite(RGB_BUILTIN, 0, 0, RGB_BRIGHTNESS);  // Blue
  delay(100);
  rgbLedWrite(RGB_BUILTIN, 0, 0, 0);  // Off / black
  delay(100);
#endif
}
void commands() {
//*******************START OF Driving commands*************************
  int val = 0;
 if (myData.xVal < 120){ 
  val = constrain(120-myData.xVal,0,120);
  (Serial.print("Reverse"));
  (Serial.println(val));
  analogWrite(A1,map(val,0,120,0,MAX_SPEED));
  analogWrite(A2,0); 
 }
 else if (myData.xVal > 130){
  val = constrain(myData.xVal-130,0,120);
  (Serial.print("Forward"));
  (Serial.println(val));
  analogWrite(A2,map(val,0,120,0,MAX_SPEED));
  analogWrite(A1,0); 
 }
 else{
  (Serial.println("STOP"));
  analogWrite(A2,0);
  analogWrite(A1,0); 
 }
//*******************END OF Driving commands*************************

//*******************START OF Steering commands*************************
if (myData.yVal < (previous-15)){
 stepper.step(-20);
 stepsglobal -= 1;
 previous -= 17;
 Serial.print("Right");
 Serial.println(previous);
}
else if (myData.yVal > (previous+15)){
  stepper.step(20);
  stepsglobal += 1;
  previous += 17;
  Serial.print("Left");
 Serial.println(previous);
}
else
{
  digitalWrite(3,LOW);
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW); 
} 

//*******************END OF Steering commands*************************
}
