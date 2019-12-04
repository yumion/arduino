#include <Servo.h>
Servo GRASP;  // 手
Servo HORIZONTAL;  // 腕の縦振り
Servo VERTICAL;  // 腕の横スライド

#define AIN1 3  // 右輪
#define AIN2 11
#define BIN1 5  // 左輪
#define BIN2 6

int channel[5];

/* sample
 * right wheel, left wheel,4本指,縦振り,横スライド
 * DEFAULT: 0,0,80,60,90e
 * MIN: 0,0,,0,0e
 * MAX: 100,100,,180,180e
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
  GRASP.attach(4);
  HORIZONTAL.attach(7);
  VERTICAL.attach(8);
}


void dc_motor_digital(int r_speeds, int l_speeds, int period=100) {
  /* control speed by digitalWrite like PWM */
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
//  Serial.println("speeds");
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
  /* serialで受け取った文字列をモータのパラメータに変換 */
  int i = 0;
  while (Serial.available()) {
    buffer[i] = Serial.read();
    if (buffer[i] == 'e') {
      buffer[i] = '\0';
      //Serial.println(buf);
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
    }
    else {
      i++;
    }
  }
}


void loop() {
  /* get parameter from python */
  char buf[21];
  getInputValue(buf);
  /* driver dc motor */
  dc_motor_digital(channel[0], channel[1]);
  /* drive servo */
  GRASP.write(channel[2]);
  HORIZONTAL.write(channel[3]);
  VERTICAL.write(channel[4]);
}


