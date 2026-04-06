

#include <Arduino.h>

#define DEFAULT_THRESHOLD 40
#define NUM_PINS           7
#define DEBOUNCE_MS       50

int threshold = DEFAULT_THRESHOLD;  

// Touch pins T2–T8 
const int TOUCH_PINS[NUM_PINS] = { T2, T3, T4, T5, T7, T8, T9};
const char* NOTE_NAMES[NUM_PINS] = { "Do", "Re", "Mi", "Fa", "So", "La", "Ti" };

bool  touchActive[NUM_PINS]   = { false };
unsigned long lastChange[NUM_PINS] = { 0 };
int   currentMode = 1;       // 1 = Sine, 2 = Sawtooth

const int SEQ_TARGET[5] = { 0, 1, 2, 3, 4 };
int  seqBuf[5]  = { -1, -1, -1, -1, -1 };
int  seqHead    = 0;

void recordNote(int noteIdx) {
  seqBuf[seqHead] = noteIdx;
  seqHead = (seqHead + 1) % 5;

  bool match = true;
  for (int i = 0; i < 5; i++) {
    if (seqBuf[(seqHead + i) % 5] != SEQ_TARGET[i]) {
      match = false;
      break;
    }
  }

  if (match) {

    currentMode = (currentMode == 1) ? 2 : 1;
    Serial.print("MODE:");
    Serial.println(currentMode);
  
    for (int i = 0; i < 5; i++) seqBuf[i] = -1;
    seqHead = 0;
  }
}


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Fabric Instrument Ready!");
  Serial.print("MODE:");
  Serial.println(currentMode);
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.startsWith("THRESH:")) {
      int newVal = cmd.substring(7).toInt();
      if (newVal >= 5 && newVal <= 80) {
        threshold = newVal;
        Serial.print("THRESH_ACK:");
        Serial.println(threshold);
      }
    }
  }

  unsigned long now = millis();

  for (int i = 0; i < NUM_PINS; i++) {

    if (now - lastChange[i] < DEBOUNCE_MS) continue;

    int val = touchRead(TOUCH_PINS[i]);

    if (val < threshold && !touchActive[i]) {
  
      touchActive[i] = true;
      lastChange[i]  = now;

      Serial.print("T");
      Serial.print(i + 1);
      Serial.print(":");
      Serial.println(val);

      recordNote(i);

    } else if (val >= threshold && touchActive[i]) {
   
      touchActive[i] = false;
      lastChange[i]  = now;

      Serial.print("T");
      Serial.print(i + 1);
      Serial.println(":OFF");
    }
  }

  delay(20);  
