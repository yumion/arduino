const int PIN_LED = 6;
const uint8_t speeds = 30;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // データが送られてきたら
  if (Serial.available() > 0) {
    int data = Serial.read();
    Serial.println(data);
    if (data == 1) {
       digitalWrite(PIN_LED, HIGH);
       analogWrite(10,speeds);
       analogWrite(11,0);
        
    }

    if (data == 2) {
        digitalWrite(PIN_LED, LOW);
        analogWrite(10,0);
        analogWrite(11,speeds);
    }
  }
}
