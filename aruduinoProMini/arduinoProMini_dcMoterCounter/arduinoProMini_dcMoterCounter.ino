// Specify pin# where Rotary encoder connected to
const uint8_t speeds = 30;

// グローバル変数の宣言
char input[1];  // 文字列格納用
int i = 0;      // 文字数のカウンタ
int val = 0;    // 受信した数値
int val0;  // counter
int rot=0;   // 回転指示用

void setup() {
  Serial.begin(9600);
  Serial.println("CarMovement: 53.=break, 49.=forward, 50.=back, 51.=cw, 52.=ccw");
//  val0 = analogRead(0);
  Serial.println(val0);
}


// シリアル通信で受信したデータを数値に変換
int serialNum(){
  // データ受信した場合の処理
  if (Serial.available()) {
    input[i] = Serial.read();
     // 文字数が3以上 or 末尾文字がある場合の処理
    if (i > 0 || input[i] == "\0") {
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
  Serial.println(rot);
  rot = serialNum();
//  rot = analogRead(1);
//  val0 = analogRead(0);
  
  // put your main code here, to run repeatedly:
  // 正転(回転)
  if(rot == 49){
//     Serial.println("正転");
     analogWrite(10,speeds);
     analogWrite(11,0);
     analogWrite(5,speeds);
     analogWrite(6,0);
//     Serial.println(val0);
     if (val0 / 256 == 1) {
           Serial.println("stop");
           analogWrite(10,0);
           analogWrite(11,0);
           analogWrite(5,0);
           analogWrite(6,0);
           delay(5000);
      }

  }
  else{
     // 逆転(逆回転)
     if(rot > 0){
        //Serial.println("逆転");
        analogWrite(10,0);
        analogWrite(11,speeds);
        analogWrite(5,0);
        analogWrite(6,speeds);
//        delay(5000);
     }
     else{
        if(rot == 51){
           //Serial.println("右周り");
           analogWrite(10,0);
           analogWrite(11,speeds);
           analogWrite(5,speeds);
           analogWrite(6,0);      
        }
        else{
          if(rot == 52){
            //Serial.println("左周り");
           analogWrite(10,speeds);
           analogWrite(11,0);
           analogWrite(5,0);
           analogWrite(6,speeds);                 
          }
          else{
           // ブレーキ
           //Serial.println("ブレーキ");
           analogWrite(10,0);
           analogWrite(11,0);
           analogWrite(5,0);
           analogWrite(6,0);
           //delay(1000);
          }
        }
     }
  }
}
