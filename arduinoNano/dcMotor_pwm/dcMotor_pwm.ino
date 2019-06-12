// Specify pin# where Rotary encoder connected to
const uint8_t speeds = 50;

// グローバル変数の宣言
char input[1];  // 文字列格納用
int i = 0;      // 文字数のカウンタ
int val = 0;    // 受信した数値
int rot=0;   // 回転指示用

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT);  // AIN1
  pinMode(6, OUTPUT);  // AIN2
  pinMode(10, OUTPUT);  // BIN1
  pinMode(11, OUTPUT);  // BIN2
  Serial.println("CarMovement: 53.=break, 49.=forward, 50.=back, 51.=cw, 52.=ccw");
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


/*
// シリアル通信で受信したデータを数値に変換
int serialNum(){
  // データ受信した場合の処理
  if (Serial.available()) {
    for(i=0;i<=1;i++){
        input[i] = Serial.read();}
    val = atoi(input);    // 文字列を数値に変換
    Serial.write(input); // 文字列を送信
    Serial.write("\n");
    i = 0;      // カウンタの初期化
  }
  return val;
}
*/

void loop() {
  rot = serialNum();
  //rot = Serial.read();
  //Serial.write(rot);
  //Serial.write("\n");

  if(rot ==  49){
     //Serial.println("正転");
     analogWrite(5, speeds);  // AIN1:1
     analogWrite(6, 0);  // AIN2:0
     analogWrite(10, speeds);  // BIN1:1
     analogWrite(11, 0);  // BIN2:0
     //delay(5000);
     }
  else if(rot == 50){
        //Serial.println("逆転");
     analogWrite(5, 0);  // AIN1:0
     analogWrite(6, speeds);  // AIN2:1
     analogWrite(10, 0);  // BIN1:0
     analogWrite(11, speeds);  // BIN2:1
        //delay(5000);
     }
  else if(rot == 51){
       //Serial.println("右周り");
     analogWrite(5, speeds);  // AIN1:1
     analogWrite(6, 0);  // AIN2:0
     analogWrite(10, 0);  // BIN1:0
     analogWrite(11, speeds);  // BIN2:1  
    }
  else if(rot == 52){
      //Serial.println("左周り");
     analogWrite(5, 0);  // AIN1:0
     analogWrite(6, speeds);  // AIN2:1
     analogWrite(10, speeds);  // BIN1:1
     analogWrite(11, 0);  // BIN2:0               
    }
  else{
   //Serial.println("空転");
     analogWrite(5, 0);  // AIN1:0
     analogWrite(6, 0);  // AIN2:0
     analogWrite(10, 0);  // BIN1:0
     analogWrite(11, 0);  // BIN2:0
   //delay(1000);
  }
}

