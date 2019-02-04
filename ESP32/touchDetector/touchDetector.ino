//  https://garretlab.web.fc2.com/arduino/esp32/lab/touch_sensor/index.html
//  http://mag.switch-science.com/2018/04/10/espr-32-usage/

int count = 0;


void setup() {
  Serial.begin(115200);
  pinMode(19, OUTPUT);
}

void loop() {
  touchAttachInterrupt(T0, LEDon, 10);
  digitalWrite(19, LOW);
}


void touchedT0() {
  Serial.print(count);  
  Serial.println(": T0 touched");
  count++;
}

void LEDon() {
  digitalWrite(19, HIGH);
}
