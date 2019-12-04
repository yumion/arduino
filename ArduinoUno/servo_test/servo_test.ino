#include <Servo.h>
// angle_range: 10~170 
Servo servo1;
#define PIN 10

int angle = 90;
int angle_0 = 0;

void setup() {
  Serial.begin(9600);
  servo1.attach(PIN);
  servo1.write(angle);
  pinMode(PIN, OUTPUT);
}

void loop() {
  servo1.write(angle);
  Serial.println(angle);
  delay(3000);
  servo1.write(angle_0);
  Serial.println(angle_0);
  delay(3000);
}
