#include <Servo.h>
Servo GRASP;  // 手
Servo HORIZONTAL;  // 腕の縦振り
Servo VERTICAL;  // 腕の横スライド

#define AIN1 3  // 右輪
#define AIN2 11
#define BIN1 5  // 左輪
#define BIN2 6

int channel[6];

/* sample
 * right wheel, left wheel,4本指,縦振り,横スライド
 * DEFAULT: 0,0,0,0,0e
 * MIN: 0,0,0,0,-90e
 * MAX: 100,100,100,100,90e
 */

void setup() {
  Serial.begin(115200);
//  Serial.setTimeout(50);
  TCCR2B &= B11111000;  // 3,11番ピンの周波数を980Hzに変更
  TCCR2B |= B00000011;  // r=32の場合
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  GRASP.attach(4);
  HORIZONTAL.attach(7);
  VERTICAL.attach(8);
}


void dc_motor_forward(int r_speeds, int l_speeds, int period=100) {
  /* control speed by digitalWrite like PWM */
  // IN1
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
    if (l_speeds != 0) {
     digitalWrite(BIN1, HIGH); 
    }
    delay(l_speeds - r_speeds);
    if (r_speeds != 0) {
     digitalWrite(AIN1, HIGH); 
    }
    delay(r_speeds);
  }
  digitalWrite(AIN1, LOW);
  digitalWrite(BIN1, LOW);
  // IN2
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN2, LOW);
//  Serial.println(r_speeds, l_speeds);
}


void dc_motor_backward(int r_speeds, int l_speeds, int period=100) {
  /* BACKWARD */
  r_speeds = abs(r_speeds);
  l_speeds = abs(l_speeds);
  // IN2
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN2, LOW);
  if (r_speeds > l_speeds) {
    delay(period - r_speeds);
    digitalWrite(AIN2, HIGH); 
    delay(r_speeds - l_speeds);
    if (l_speeds != 0) {
     digitalWrite(BIN2, HIGH); 
    }
    delay(l_speeds);
  }
  else {
    delay(period - l_speeds);
    if (l_speeds != 0) {
     digitalWrite(BIN2, HIGH); 
    }
    delay(l_speeds - r_speeds);
    if (r_speeds != 0) {
     digitalWrite(AIN2, HIGH); 
    }
    delay(r_speeds);
  }
  // IN1
  digitalWrite(AIN1, LOW);
  digitalWrite(BIN1, LOW);
//  Serial.println("backward");
}


void getMonitorInput(char buffer[], uint8_t maxSize=20) {
  /* シリアルモニタから入力 */
  memset(buffer, 0, maxSize);  // bufferの要素を0で初期化
  while( Serial.available() == 0 ) {
    delay(1);
  }
  uint8_t count = 0;
  do
  {
    count += Serial.readBytes(buffer+count, maxSize);
    delay(2);
  } while( (count < maxSize) && !(Serial.available() == 0) );
}


void getInputValue(char buffer[]) {
//  memset(buffer, 0, sizeof(buffer) / sizeof(int));
  /* serialで受け取った文字列をモータのパラメータに変換 */
  int i = 0;
  while (Serial.available()) {
    buffer[i] = Serial.read();
//    Serial.println(buffer);
    if (buffer[i] == 'e') {
      buffer[i] = '\0';
      channel[0] = atoi(strtok(buffer, ","));
      for(int n=1; n<sizeof(channel)/sizeof(int); n++){
        channel[n] = atoi(strtok(NULL, ","));
      }
      for(int n=0; n<sizeof(channel)/sizeof(int); n++){
        Serial.print("ch");
        Serial.print(n);
        Serial.print(": ");
        Serial.println(channel[n]);
      }
      Serial.println("---");
    }
    else {
      i++;
    }
  }
}


void loop() {
  /* get parameter from python */
  char buf[30];
  getInputValue(buf);
  
//  if (channel[5] == 1) {
    /* driver dc motor */
    if (channel[0] >= 0) {
      dc_motor_forward(channel[0], channel[1]);
    }
    else {
      dc_motor_backward(channel[0], channel[1]);
    }
    /* drive servo */
    GRASP.write(map(channel[2], 0, 100, 0, 50));
    HORIZONTAL.write(map(channel[3], 0, 100, 58, 140));
    VERTICAL.write(map(channel[4], -90, 90, 0, 180));
//  }
//  else {
//    dc_motor_forward(0, 0);
//  }
}


