#define AIN1 3
#define AIN2 11
#define BIN1 5
#define BIN2 6

int angle;
int i = 0;
char buf[30];
int channel[3];

/* sample (default)
 * right wheel, left wheel 
 * 100,100e
 */

void setup() {
  Serial.begin(9600);
  TCCR2B &= B11111000;  // 3,11番ピンの周波数を980Hzに変更
  TCCR2B |= B00000011;  // r=32の場合
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
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
  /* dc motor */
  dc_motor(channel[0], channel[1]);
   
  delay(50);
}


void dc_motor(int r_speeds, int l_speeds) {
  analogWrite(AIN1, l_speeds);
  analogWrite(AIN2, 0);
  analogWrite(BIN1, r_speeds);
  analogWrite(BIN2, 0);
}
