void setup() {
  Serial.begin(115200);
  pinMode(4, OUTPUT);
}

int thumb = 0;

void loop() {
  thumb = analogRead(4);
  Serial.print(thumb);
  delay(1000);
  
  if (thumb < 1300) {
    Serial.println("I");
  }
  else if (thumb >= 1300 && thumb < 2800) {
    Serial.println("A");
  }
  else if (thumb >= 2800 && thumb < 3200) {
    Serial.println("KA");
  }
  else if (thumb >= 3200 && thumb < 4000) {
    Serial.println("SA");
  }
   else if (thumb >= 4000) {
    Serial.println("SE");
  }
}

