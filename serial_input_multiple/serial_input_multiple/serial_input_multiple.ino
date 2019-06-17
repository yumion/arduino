int m_x = 0;
int m_y = 0;
int m_z = 0;

void setup(){
  Serial.begin(9600);
}

void loop() {
  char str[20];
    if(Serial.available()){
      int i = 0;
      while(1){
        if(Serial.available()){
          str[i] = Serial.read();
          if(str[i]=='=' || i == 18){
            str[i+1] = '\0';
            break;
          }else{
            i++;
          }
        }
      }
      sscanf(str, "%d, %d, %d=", &m_x, &m_y, &m_z);
      Serial.print("recv:");
      Serial.print(m_x, DEC);
      Serial.print(", ");
      Serial.print(m_y, DEC);
      Serial.print(", ");
      Serial.print(m_z, DEC);
      Serial.print("::"); 
    }
} 
