

void setup() {
  Serial.begin(115200);
}


void loop() {
  delay(2000);
  if (Serial.available() > 0){
    Serial.setTimeout(5000);
    char ch = Serial.read();
    String str = Serial.readStringUntil('\0');
    if(ch == '3'){
    Serial.print("ch: ");
    Serial.print(ch);
    }
    int deg = atoi(str);
    Serial.print(", deg: ");
    Serial.print(deg);
    Serial.print("\n");
  }
  Serial.println("out");
  
}
