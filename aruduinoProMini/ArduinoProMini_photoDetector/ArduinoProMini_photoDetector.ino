#include <Servo.h>

Servo servo1;
int photoDetector = 0;    // A0 pin to open 
int hand = 0;
// シリアルモニタから文字を読み取る用の変数
int val = 0;
int i = 0;
char input[1];

void setup() {
  Serial.begin(9600);
  servo1.attach(10);
  pinMode(10, OUTPUT);
  Serial.print("圧力:");
  Serial.println(photoDetector);
  Serial.print("指の角度:");
  Serial.println(hand);
}

// シリアル通信で受信したデータを数値に変換
int serialNum(){
  // データ受信した場合の処理
  if (Serial.available()) {
    input[i] = Serial.read();
     // 文字数が3以上 or 末尾文字がある場合の処理
    if (i > 0 || input[i] == "\0") {
//    if (i > 2) {
      //input[i+1] = '\0';      // 末尾に終端文字の挿入
      val = atoi(input);    // 文字列を数値に変換
      Serial.write(input); // 文字列を送信
      Serial.write("\n");
      i = 0;      // カウンタの初期化
    }
    else { i++; }
  }
  return val;
}

void loop() {
//    hand = serialNum();
    servo1.write(30);
    Serial.print("指の角度:");
    Serial.println(30);
    photoDetector = analogRead(0);
    Serial.print("圧力:");
    Serial.println(photoDetector);
    delay(5000);
    
    servo1.write(130);
    Serial.print("指の角度:");
    Serial.println(130);
    photoDetector = analogRead(0);
    Serial.print("圧力:");
    Serial.println(photoDetector);
    delay(5000);
}
