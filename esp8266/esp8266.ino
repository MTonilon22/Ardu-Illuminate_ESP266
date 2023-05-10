
#include <Arduino.h>
#include <ESP8266WiFi.h> //import for wifi functionality
#include <WebSocketsServer.h> //import for websocket


#define RELAY_PIN D1 //defining the OUTPUT pin for LED (D4)

#define RELAY_ON LOW
const int relay = 5;



const char *ssid =  "Ardu-Illuminate";   //Wifi SSID (Name)   
const char *pass =  "123456789"; //wifi password

WebSocketsServer webSocket = WebSocketsServer(81); //websocket init with port 81
WiFiClient client;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
//webscket event method
    String cmd = "";
    String newSsid = "";
    String newPassword = "";
  //  pinMode(relay,OUTPUT);
    
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("We bsocket is disconnMected");
            //case when Websocket is disconnected
            break;
        case WStype_CONNECTED:{
            //wcase when websocket is connected
            Serial.println("Websocket is connected");
            Serial.println(webSocket.remoteIP(num).toString());
            webSocket.sendTXT(num, "connected");}
            break;
        case WStype_TEXT:
        pinMode(relay,OUTPUT);
            cmd = "";
            for(int i = 0; i < length; i++) { 
                cmd = cmd + (char) payload[i]; 
            } //merging payload to single string
            Serial.println(cmd);

            if(cmd == "poweron"){ 
              
                digitalWrite(RELAY_PIN, RELAY_ON);
            }else if(cmd == "poweroff"){
               
                digitalWrite(RELAY_PIN, !RELAY_ON);

            }else if(cmd.indexOf("brightness") >= 0) {
                String sliderValue = cmd.substring(10);
                int dutyCycle = map(sliderValue.toInt(), 0, 100, 0, 1023);
                if(dutyCycle > 0) {
                    digitalWrite(RELAY_PIN, HIGH); // turn on the relay if duty cycle is greater than 0
                    delay(dutyCycle * 10); // wait for a time proportional to the duty cycle
                    digitalWrite(RELAY_PIN, LOW); // turn off the relay
                    delay((100 - dutyCycle) * 10); // wait for the remaining time
                } else {
                    digitalWrite(RELAY_PIN, LOW); // turn off the relay if duty cycle is 0
                }
            }else if(cmd.indexOf("ssid") >=0 && cmd.indexOf("password") > =0) {
              newSsid = cmd.substring(4);
              newPassword = cmd.substring(8);

               Serial.print("New SSID: ");
               Serial.println(newSsid);
               Serial.print("New Password: ");
               Serial.println(newPassword);
               WiFi.disconnect(); // Disconnect from current network
               delay(1000);
               WiFi.mode(WIFI_STA); // Set Wi-Fi mode to station mode
               WiFi.begin(newSsid.c_str(), newPassword.c_str()); // Connect to new network
            }
            // else if(cmd.indexOf("password") >=0){
            //   newPassword = cmd.substring(8);
            //   Serial.print("New Password: ");
            //   Serial.println(newPassword);
            //   WiFi.disconnect();
            //   delay(1000);
            //   WiFi.mode(WIFI_STA);
            //   WiFi.begin(newSsid.c_str(), newPassword.c_str());

            // }

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
   pinMode(RELAY_PIN, OUTPUT); //set ledpin (D2) as OUTPUT pin
   Serial.begin(9600); //serial start

  
  
  IPAddress apIP(192, 168, 0, 1);   //Static IP for wifi gateway
  IPAddress subnet(255,255,255,0);
  WiFi.softAPConfig(apIP, apIP, subnet); //set Static IP gateway on NodeMCU
  WiFi.softAP(ssid, pass); //turn on WIFI

   connectToWiFi();
   webSocket.begin(); //websocket Begin
   webSocket.onEvent(webSocketEvent); //set Event for websocket
   Serial.println("Websocket is started");
}

void loop() {
   webSocket.loop(); //keep this line on loop method
}
