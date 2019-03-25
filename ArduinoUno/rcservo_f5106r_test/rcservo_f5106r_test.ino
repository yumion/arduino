#include <Servo.h>
/* STOP:1500+-5us
 * CW:1500~700us
 * CCW:1500~2300us
 */
Servo sv;
int PIN = 10;
int pulse_w = 700;
int pulse_w_0 = 2300;

void setup() {
  Serial.begin(9600);
  sv.attach(PIN, 700, 2300);
  sv.writeMicroseconds(1500);
  pinMode(PIN, OUTPUT);
}

void loop() {
  sv.writeMicroseconds(pulse_w);
  Serial.println(sv.readMicroseconds());
  delay(3000);
  sv.writeMicroseconds(1500);
  delay(1000);
  sv.writeMicroseconds(pulse_w_0);
  Serial.println(sv.readMicroseconds());
  delay(3000);
  sv.writeMicroseconds(1500);
  delay(1000);
}
