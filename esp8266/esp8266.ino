
#include <Arduino.h>
#include <ESP8266WiFi.h> //import for wifi functionality
#include <WebSocketsServer.h> //import for websocket


#define ledpin D2 //defining the OUTPUT pin for LED (D4)

const char *ssid =  "Legazy Z_GLOBE SHIT";   //Wifi SSID (Name)   
const char *pass =  "M@tr1xt3ch112"; //wifi password

WebSocketsServer webSocket = WebSocketsServer(81); //websocket init with port 81
WiFiClient client;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
//webscket event method
    String cmd = "";
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("We bsocket is disconnected");
            //case when Websocket is disconnected
            break;
        case WStype_CONNECTED:{
            //wcase when websocket is connected
            Serial.println("Websocket is connected");
            Serial.println(webSocket.remoteIP(num).toString());
            webSocket.sendTXT(num, "connected");}
            break;
        case WStype_TEXT:
            cmd = "";
            for(int i = 0; i < length; i++) {
                cmd = cmd + (char) payload[i]; 
            } //merging payload to single string
            Serial.println(cmd);

            if(cmd == "poweron"){ //when command from app is "poweron"
                digitalWrite(ledpin, HIGH);   //make ledpin output to HIGH  
            }else if(cmd == "poweroff"){
                digitalWrite(ledpin, LOW);    //make ledpin output to LOW on 'pweroff' command.
            }
               else if(cmd.indexOf("brightness" >=0)){
              String sliderValue = "0";
              int dutyCycle;           
              sliderValue = cmd.substring(10);
              dutyCycle = map(sliderValue.toInt(),0,100,0,1023);
              analogWrite(ledpin,dutyCycle);
            }

             webSocket.sendTXT(num, cmd + ":success");
             //send response to mobile, if command is "poweron" then response will be "poweron:success"
             //this response can be used to track down the success of command in mobile app.
            break;
        case WStype_FRAGMENT_TEXT_START:
            break;
        case WStype_FRAGMENT_BIN_START:
            break;
        case WStype_BIN:
            hexdump(payload, length);
            break;
        default:
            break;
    }
}

void connectToWiFi(){
  
   Serial.println("Connecting to wifi");
   WiFi.begin(ssid,pass);
   int retries = 0;
   while ((WiFi.status() != WL_CONNECTED)&& (retries < 15)) {
      retries++;
      delay(500);
      
    }

    if(retries > 14 ) {
        Serial.println(F("WiFi connection FAILED"));
      }

     if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected!"));
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
  }

void setup() {
   pinMode(ledpin, OUTPUT); //set ledpin (D2) as OUTPUT pin
   Serial.begin(9600); //serial start

  
   
//   IPAddress apIP(192, 168, 0, 1);   //Static IP for wifi gateway
//   WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); //set Static IP gateway on NodeMCU
//   WiFi.softAP(ssid, pass); //turn on WIFI

    connectToWiFi();
   webSocket.begin(); //websocket Begin
   webSocket.onEvent(webSocketEvent); //set Event for websocket
   Serial.println("Websocket is started");
}

void loop() {
   webSocket.loop(); //keep this line on loop method
}
