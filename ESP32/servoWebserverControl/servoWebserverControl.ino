#include <WiFiClient.h>
#include <ESP32WebServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>

/* あなたのWIFIに接続するSSIDとパスワードに書き替えください。 */
const char* ssid = "onoderaLab-2.4GHz";
const char* password = "onodera009F";

ESP32WebServer server(80);

const int servoPin = 12;
int oldAngle = 0; 
int PWM_WIDTH = 20000;

/* 
HPのデータを格納するデータ配列
*/
char res[2500]=
"<!DOCTYPE html>\
<html>\
<head>\
<meta charset='utf-8'>\
<link href='https://stackpath.bootstrapcdn.com/bootstrap/4.1.0/css/bootstrap.min.css' rel='stylesheet' integrity='sha384-9gVQ4dYFwwWSjIDZnLEWnxCjeSWFphJiwGPXr1jddIhOegiu1FwO5qRGvFXOdJZ4' crossorigin='anonymous'>\
<script src='https://stackpath.bootstrapcdn.com/bootstrap/4.1.0/js/bootstrap.min.js' integrity='sha384-uefMccjFJAIv6A+rW+L4AHf99KvxDjWSu1z9VI8SKNVmz4sk7buKt/6v9KI65qnm' crossorigin='anonymous'></script>\
<script src='https://code.jquery.com/jquery-3.3.1.min.js' integrity='sha256-FgpCb/KJQlLNfOu91ta32o/NMZxltwRo8QtmkMRdAu8=' crossorigin='anonymous'></script>\
<script src='https://code.jquery.com/ui/1.12.1/jquery-ui.min.js' integrity='sha256-VazP97ZCwtekAsvgPBSUwPFKdrwD3unUfSGVYrahUqU=' crossorigin='anonymous'></script>\
<script>\
$(function() {\
$('#sliVal').html('角度: 0 度');\
$('#slider').slider({\
    orientation:'horisontal',value:0,min: 0,max: 180,step: 1\
});\
$('#slider').slider().bind({\
slidestop: function(e,ui){\
    $('#res').removeClass('p-3 mb-2 bg-success text-white').addClass('p-3 mb-2 bg-danger text-white');\
    $('#sliVal').html('角度: '+ui.value+' 度');\
    $.get('/ang?val=' + ui.value, function(d, s){\
        $('#res').removeClass('p-3 mb-2 bg-danger text-white').addClass('p-3 mb-2 bg-success text-white');\
        $('#res').html(s);\
    }); \
}\
});\
});\
</script>\
</head>\
  <body>\
    <div class='container'>\
      <br>\
      <div class='row'>\
        <div class='col-lg-12 shadow-lg p-3 mb-5 bg-white rounded d-block'>\
          <h1>株式会社虹賢舎</h1>\
          <hr>\
          <p>ESP32 Web経由サーボ制御</p>\
        </div>\
      </div>\
      <div class='row'>\
        <div class='col-lg-12 shadow-lg p-3 mb-5 bg-white rounded d-block'>\
            <div class='col-lg-12' id='slider'></div></br>\
            <div class='col-lg-12' id='sliVal'></div></br>\
            <div class='col-lg-12' id='res'></div></br>\
        </div>\
      </div>\
    </div>\
  </body>\
</html>";


///////////////////
void handleRoot() {
  server.send(200, "text/html", res);
}
/* 角度から　pulse に変換する */
int servoPulse(int angleDegrees)
{
  int pulseWidth = map(angleDegrees, 0,180,600,2400);
  return pulseWidth;
}
/* 
角度を確認して、pulseを動作させる
*/
void servoGo(int oldAngle, int newAngle)
{
  int pulseWidth;
  if(oldAngle == newAngle){
    return;
  }else if(oldAngle < newAngle){
    /* 時計回り処理 */
    for (int i=oldAngle; i<=newAngle; i++){
      /* convert angle to pulse width us*/
      pulseWidth = servoPulse(i);
      /* HIGH pulse */  
      digitalWrite(servoPin, HIGH);
      /* use delayMicroseconds to delay for pulseWidth */
      delayMicroseconds(pulseWidth); 
      /* LOW pulse */                          
      digitalWrite(servoPin, LOW);
      /* 
      DELAY
　　　　*/ 
      delayMicroseconds(PWM_WIDTH - pulseWidth);   
    }
  }else if(oldAngle > newAngle){
    /* 逆時計回り処理 */
    for (int i=oldAngle; i>=newAngle; i--){
      pulseWidth = servoPulse(i);
      digitalWrite(servoPin, HIGH);
      delayMicroseconds(pulseWidth);                           
      digitalWrite(servoPin, LOW);
      delayMicroseconds(PWM_WIDTH - pulseWidth);
    }
  }
}
/* サーボを動かす */
void handleServo() {
  //Serial.println(server.argName(0));
  int newAngle = server.arg(0).toInt();
  servoGo(oldAngle, newAngle);
  oldAngle = newAngle;
  server.send(200, "text/html", "ok");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  server.send(404, "text/plain", message);
}

void setup(void){
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  server.on("/ang", handleServo);

  server.onNotFound(handleNotFound);

  server.begin();
  pinMode(servoPin, OUTPUT);
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
