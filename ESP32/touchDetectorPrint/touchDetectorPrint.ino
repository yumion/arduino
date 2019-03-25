
int count = 0;
int touch = 0;
 
void touchedT3() {
//  Serial.print(count);  
  touch = touchRead(T3);
  Serial.print(touch);
  Serial.println(": T0 touched");
  count++;
  delay(1000);
}
 
void setup() {
  Serial.begin(115200);
//  touchAttachInterrupt(T3, touchedT3, 30);
}
 
void loop() {
  touch = touchRead(T3);
  Serial.println(touch);
  delay(1000);
}
