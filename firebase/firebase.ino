/*Before Building The Project You Can Simply Test 
The Firebase Database Functionality Using This Code*/
  
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>


//Provide the RTDB payload printing info and other helper functions.

float randNumber1;

#define FIREBASE_HOST "ardu-illuminate-default-rtdb.asia-southeast1.firebasedatabase.app"  
#define FIREBASE_AUTH "9IB8xzPkUzNSSbMXGUzhj2ytjBOSLsVfFpUE9Pz1"  

String ssid, password;
//Define Firebase Data object


void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));

  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

   ssid = Firebase.getString("/network/ssid");
   password = Firebase.getString("/network/password");

    Serial.println(ssid);
    Serial.print(password);
  if(ssid == "Ardu-Illuminate" && password == "123456789"){
    
   IPAddress apIP(192, 168, 0, 1);   //Static IP for wifi gateway
   IPAddress subnet(255,255,255,0);
   WiFi.softAPConfig(apIP, apIP, subnet); //set Static IP gateway on NodeMCU
   WiFi.softAP(ssid, password); //turn on WIFI

   Serial.print(ssid + password);
  
  }else{
      WiFi.begin(ssid,password);
      Serial.print(ssid + password);
      Serial.print(WiFi.localIP());
      Serial.print("Connecting");
      while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
         delay(1000);
        }
    }
  
}

void loop() {
  

  
randNumber1 = random(10);

  Firebase.setFloat ("Smoke",randNumber1);
  delay(2000);

 if(Firebase.failed())
{
  Serial.println("Firebase log sending failed");
  Serial.println(Firebase.error());
  return;
}

}
