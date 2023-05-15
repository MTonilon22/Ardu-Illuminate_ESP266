#include <Arduino.h>
#include <ESP8266WiFi.h> //import for wifi functionality
#include <WebSocketsServer.h> //import for websocket


#define ledpin D2 //defining the OUTPUT pin for LED (D4)
#define RELAY HIGH

int brightness = 0;
const char *ssid =  "Ardu-Illuminate";   //Wifi SSID (Name)   
const char *pass =  "123456789"; //wifi password


    String newSsid = "";
    String newPassword = "";
WebSocketsServer webSocket = WebSocketsServer(81); //websocket init with port 81

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
//webscket event method   
    String cmd = "";
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("Websocket is disconnected");
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
                digitalWrite(ledpin, !RELAY);   //make ledpin output to HIGH  
            }else if(cmd == "poweroff"){
                digitalWrite(ledpin, RELAY);    //make ledpin output to LOW on 'pweroff' command.
            }
            else if(cmd.indexOf("brightness") >=0)){
               brightness = cmd.substring(10).toInt();
              String sliderValue = "0";
              int dutyCycle = map(sliderValue.toInt(),0,100,0,1023);
              sliderValue = cmd.substring(10);
              
               if (brightness == 0 || brightness == 100) {
                     // If the brightness is either 0 or 100, turn the relay on or off
                   digitalWrite(ledpin, brightness == 100 ? RELAY : !RELAY);
               }else {
    // Otherwise, simulate dimming by turning the relay on and off with a delay
                  digitalWrite(ledpin, RELAY);
                  delay(dutyCycle);
                  digitalWrite(ledpin, !RELAY);
                  delay(255 - dutyCycle);
                }
              }
                else if(cmd.indexOf("ssid") >=0) {
                  newSsid = cmd.substring(4);
                  newPassword = cmd.substring(8);
  
                 Serial.print("New SSID: ");
                 Serial.println(newSsid);
                 Serial.print("New Password: ");
                 Serial.println(newPassword);
                 
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

void setup() {
   pinMode(ledpin, OUTPUT); //set ledpin (D2) as OUTPUT pin
   Serial.begin(9600); //serial start

   Serial.println("Connecting to wifi");
   Serial.println(newSsid);
   Serial.println(newPassword);
   
   IPAddress apIP(192, 168, 0, 1);   //Static IP for wifi gateway
   WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); //set Static IP gateway on NodeMCU
   WiFi.softAP(ssid, pass); //turn on WIFI

   webSocket.begin(); //websocket Begin
   webSocket.onEvent(webSocketEvent); //set Event for websocket
   Serial.println("Websocket is started");
}

void loop() {
   webSocket.loop(); //keep this line on loop method
}
