
#include <PCA9685.h>            //PCA9685用ヘッダーファイル（秋月電子通商作成）

PCA9685 pwm = PCA9685(0x40);    //PCA9685のアドレス指定（アドレスジャンパ未接続時）
#define SERVOMIN 150            //最小パルス幅 (標準的なサーボパルスに設定)
#define SERVOMAX 600            //最大パルス幅 (標準的なサーボパルスに設定)

void setup() {
  Serial.begin(9600);
  pwm.begin();                   //初期設定 (アドレス0x40用)
  pwm.setPWMFreq(60);            //PWM周期を60Hzに設定 (アドレス0x40用)
}

int l = 0;
int n = 0;
int orn = 1;
void loop() {
  servo_write(3, n);
  rack_pinion(2, l);
  rc_servo(0, orn);
  rc_servo(1, -orn+1);
  l=l+1;
  n=n+10;
  if(l==15) l=0;
  if(n==180) n=0;
  delay(200);
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

