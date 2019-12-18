#include <Servo.h>
Servo GRASP;  // 手
Servo HORIZONTAL;  // 腕の縦振り
Servo VERTICAL;  // 腕の横スライド

#define AIN1 3  // 右輪
#define AIN2 11
#define BIN1 5  // 左輪
#define BIN2 6

#define R_MOTOR_bit 0
#define L_MOTOR_bit 32
#define GRASP_bit 64
#define VERTICAL_bit 96
#define HORIZONTAL_bit 128
#define TERMINATE_bit 160


/* sample
モーターの種類(3bit)入力する値(5bit)
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


short r_value = 0;
short l_value = 0;

void loop() {
  /* get parameter from python */
    short input = Serial.read();
    if (input != -1) Serial.println((input & B00011111), DEC);
//    delay(1000);

    switch(input & B11100000) {
      
      case R_MOTOR_bit:
        r_value = (input & B00011111) * 5;
        break;

      case L_MOTOR_bit:
        l_value = (input & B00011111) * 5;
        break;
        
      case TERMINATE_bit:
        dc_motor_forward(r_value, l_value);
        break;
        
      case GRASP_bit:
        GRASP.write(map((input & B00011111) * 100, 0, 100, 0, 50));
        break;
        
      case HORIZONTAL_bit:
        HORIZONTAL.write(map((input & B00011111) * 10, 0, 100, 80, 83));
        break;
        
      case VERTICAL_bit:
        VERTICAL.write((input & B00011111) * 100);
        
      default:
        dc_motor_forward(0, 0);
    }
}


