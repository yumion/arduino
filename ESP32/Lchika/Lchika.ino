#define PIN 19

void setup() {
  Serial.begin(115200);
  pinMode(PIN, OUTPUT);

}

void loop() {
  digitalWrite(PIN, HIGH);
  Serial.println("HIGH");
  delay(1000);
  digitalWrite(PIN, LOW);
  Serial.println("LOW");
  delay(1000);
}
