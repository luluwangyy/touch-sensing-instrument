#include <Arduino.h>

#define THRESHOLD 40

bool touch1detected = false;
bool touch2detected = false;
bool touch1active = false;
bool touch2active = false;
uint8_t touch1Val = 0;
uint8_t touch2Val = 0;

void IRAM_ATTR gotTouch1() {
  touch1detected = true;
  touch1Val = touchRead(T2);  // GPIO2
}

void IRAM_ATTR gotTouch2() {
  touch2detected = true;
  touch2Val = touchRead(T9);  // GPIO32
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Fabric Ready!");
  touchAttachInterrupt(T7, gotTouch1, THRESHOLD);
  touchAttachInterrupt(T9, gotTouch2, THRESHOLD);
}

void loop() {

  int v1 = touchRead(T2);
  if (v1 < THRESHOLD && !touch1active) {
    touch1active = true;
    Serial.print("T1:");
    Serial.println(v1);
  } else if (v1 >= THRESHOLD && touch1active) {
    touch1active = false;
    Serial.println("T1:OFF");
  }


  int v2 = touchRead(T9);
  if (v2 < THRESHOLD && !touch2active) {
    touch2active = true;
    Serial.print("T2:");
    Serial.println(v2);
  } else if (v2 >= THRESHOLD && touch2active) {
    touch2active = false;
    Serial.println("T2:OFF");
  }

  delay(50);
}