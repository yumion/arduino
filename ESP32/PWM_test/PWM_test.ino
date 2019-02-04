#include "esp_system.h"

void setup() { 
  Serial.begin(115200);
  ledcSetup(0, 50, 10);  // 0ch 50 Hz 10bit resolution
  ledcAttachPin(15, 0); // 15pin, 0ch
}

int min = 26;  // (26/1024)*20ms ≒ 0.5 ms  (-90°)
int max = 123; // (123/1024)*20ms ≒ 2.4 ms (+90°)
int n = min;

void loop() {
  ledcWrite(0, n);
  n+=5;
  if (n > max) n = min;
  delay(500);
}
