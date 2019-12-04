#define AIN1 3  // 右輪
#define AIN2 11
#define BIN1 5  // 左輪
#define BIN2 6

int i = 0;
char buf[30];
int channel[7];

/* sample (default)
 * right wheel, left wheel 
 * 100,100e
 */

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);
  TCCR2B &= B11111000;  // 3,11番ピンの周波数を980Hzに変更
  TCCR2B |= B00000011;  // r=32の場合
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
}


void dc_motor_digital(int r_speeds, int l_speeds, int period=100) {
  // IN1
  digitalWrite(AIN1, LOW);
  digitalWrite(BIN1, LOW);
  if (r_speeds > l_speeds) {
    delay(period - r_speeds);
    digitalWrite(AIN1, HIGH); 
    delay(r_speeds - l_speeds);
    if (l_speeds != 0) {
     digitalWrite(BIN1, HIGH); 
    }
    delay(l_speeds);
  }
  else {
    delay(period - l_speeds);
    if (r_speeds != 0) {
     digitalWrite(BIN1, HIGH); 
    }
    delay(l_speeds - r_speeds);
    if (l_speeds != 0) {
     digitalWrite(AIN1, HIGH); 
    }
    delay(r_speeds);
  }
  // IN2
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN2, LOW);
}


void loop() {
  /* serialで受け取った文字列をモータのパラメータに変換 */
  while (Serial.available()) {
    buf[i] = Serial.read();
    Serial.println(i);
    if (buf[i] == 'e') {
      buf[i] = '\0';
      //Serial.println(buf);
      channel[0] = atoi(strtok(buf, ","));
      for(int n=1; n<6; n++){
        channel[n] = atoi(strtok(NULL, ","));
      }
      for(int n=0; n<6; n++){
        Serial.print("ch");
        Serial.print(n);
        Serial.print(": ");
        Serial.println(channel[n]);
      }
      i = 0;
    }
    else {
      i++;
    }
  }
  /* dc motor */
//  dc_motor(channel[0], channel[1]);
  dc_motor_digital(channel[0], channel[1]);

}


void dc_motor(int r_speeds, int l_speeds) {
  analogWrite(AIN1, l_speeds);
  analogWrite(AIN2, 0);
  analogWrite(BIN1, r_speeds);
  analogWrite(BIN2, 0);
}


void digital_pwm(int pin, float duty, float period=100000) {
  float duty_rate = duty / 255.0;
  float pulse_width = period * duty_rate;
  digitalWrite(pin, LOW);
  delayMicroseconds(period - pulse_width);
  digitalWrite(pin, HIGH);
  delayMicroseconds(pulse_width);
}

