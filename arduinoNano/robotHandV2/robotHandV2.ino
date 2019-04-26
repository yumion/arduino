
#include <PCA9685.h>            //PCA9685用ヘッダーファイル（秋月電子通商作成）

PCA9685 pwm = PCA9685(0x40);    //PCA9685のアドレス指定（アドレスジャンパ未接続時）
#define SERVOMIN 150            //最小パルス幅 (標準的なサーボパルスに設定)
#define SERVOMAX 600            //最大パルス幅 (標準的なサーボパルスに設定)
// サーボのチャンネル
#define LEFT_WHEEL 0
#define RIGHT_WHEEL 1
#define HORIZON 2
#define VERTICAL 3
#define WRIST 4
#define GRASP 5

void setup() {
  Serial.begin(9600);
  pwm.begin();                   //初期設定 (アドレス0x40用)
  pwm.setPWMFreq(60);            //PWM周期を60Hzに設定 (アドレス0x40用)
}

int l = 0;
int n = 0;
int orn = 2;
void loop() {
  delay(2000);
  servo_write(GRASP, 80);
  servo_write(WRIST, 60);
  rack_pinion(HORIZON, 15);
  servo_write(VERTICAL, 140);
  rc_servo(LEFT_WHEEL, orn);
  rc_servo(RIGHT_WHEEL, -orn+1);
  delay(2000);
  servo_write(GRASP, 170);
  servo_write(VERTICAL, 60);
}

void servo_write(int ch, int ang){ //動かすサーボチャンネルと角度を指定
  ang = map(ang, 0, 180, SERVOMIN, SERVOMAX); //角度（0～180）をPWMのパルス幅（150～600）に変換
  pwm.setPWM(ch, 0, ang);
  //delay(1);
}

void rc_servo(int ch, int orn){
  int pulse_w = 387; // ブレーキ
  if(orn == 0){
    //右回転
    pulse_w = 100;
//    Serial.println("cw");
  } else if(orn == 1){
    //左回転
    pulse_w = 500;
//    Serial.println("ccw");
  }
  pwm.setPWM(ch, 0, pulse_w);
}

void rack_pinion(int ch, int distance){
  float ang = map(distance, 0, 15, 0, 65); //最大15mm(65度)移動．角度に変換
  servo_write(ch, ang); //角度指定で動かす
}

