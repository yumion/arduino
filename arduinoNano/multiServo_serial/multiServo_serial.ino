#include <Servo.h>
Servo myServo0; // create a servo object
Servo myServo1; // create a servo object
Servo myServo2; // create a servo object
Servo myServo3;
int angle;

int i = 0;
char buf[20];
int ch[5];


void setup() {
    myServo0.attach(2); // attaches the servo on pin 9 to the servo object
    myServo1.attach(3); // attaches the servo on pin 9 to the servo object
    myServo2.attach(4); // attaches the servo on pin 9 to the servo object
    myServo3.attach(5);
    Serial.begin(9600);
}

void loop() { 

 if (Serial.available()) {
    buf[i] = Serial.read();
    if (buf[i] == 'e') {
      buf[i] = '\0';
      //Serial.println(buf);

      ch[0] = atoi(strtok(buf, ","));
      ch[1] = atoi(strtok(NULL, ","));
      ch[2] = atoi(strtok(NULL, ","));
      ch[3] = atoi(strtok(NULL, ","));

      Serial.println(ch[0]);
      Serial.println(ch[1]);
      Serial.println(ch[2]);
      Serial.println(ch[3]);
      i = 0;
    }
    else {
      i++;
    }
  }    
  
     angle = ch[0];
     myServo0.write(ch[0]);
     myServo1.write(ch[1]);
     myServo2.write(ch[2]);
     myServo3.write(ch[3]);
     delay(15);
}

