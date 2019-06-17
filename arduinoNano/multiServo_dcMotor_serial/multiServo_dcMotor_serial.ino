#include <Servo.h>
Servo sv1;
Servo sv2;
Servo sv3;
Servo sv4;

#define AIN1 3
#define AIN2 11
#define BIN1 5
#define BIN2 6

int angle;
int i = 0;
char buf[30];
int channel[6];


void setup() {
  Serial.begin(9600);
  TCCR2B &= B11111000;  // 3,11番ピンの周波数を980Hzに変更
  TCCR2B |= B00000011;  // r=32の場合
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  sv1.attach(2);  // pwm
  sv2.attach(4);  // pwm
  sv3.attach(7);  // pwm
  sv4.attach(8);  // pwm
}


void dc_motor(int orn, int speeds=100) {

  if(orn ==  0){
     //Serial.println("正転");
     analogWrite(AIN1, speeds);  // AIN1:1
     analogWrite(AIN2, 0);  // AIN2:0
     analogWrite(BIN1, speeds);  // BIN1:1
     analogWrite(BIN2, 0);  // BIN2:0
     }
  else if(orn == 1){
        //Serial.println("逆転");
     analogWrite(AIN1, 0);  // AIN1:0
     analogWrite(AIN2, speeds);  // AIN2:1
     analogWrite(BIN1, 0);  // BIN1:0
     analogWrite(BIN2, speeds);  // BIN2:1
     }
  else if(orn == 2){
       //Serial.println("右周り");
     analogWrite(AIN1, speeds);  // AIN1:1
     analogWrite(AIN2, 0);  // AIN2:0
     analogWrite(BIN1, 0);  // BIN1:0
     analogWrite(BIN2, speeds);  // BIN2:1  
    }
  else if(orn == 3){
      //Serial.println("左周り");
     analogWrite(AIN1, 0);  // AIN1:0
     analogWrite(AIN2, speeds);  // AIN2:1
     analogWrite(BIN1, speeds);  // BIN1:1
     analogWrite(BIN2, 0);  // BIN2:0               
    }
  else{
   //Serial.println("空転");
     analogWrite(AIN1, 0);  // AIN1:0
     analogWrite(AIN2, 0);  // AIN2:0
     analogWrite(BIN1, 0);  // BIN1:0
     analogWrite(BIN2, 0);  // BIN2:0
  }
}


void loop() {
  /* serialで受け取った文字列をモータのパラメータに変換 */
   if (Serial.available()) {
    buf[i] = Serial.read();
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
  /* servo */
  sv1.write(channel[0]);
  sv2.write(channel[1]);
  sv3.write(channel[2]);
  sv4.write(channel[3]);
  /* dc motor */
  dc_motor(channel[4], channel[5]);
   
  delay(50);
}


