/*************************************************************
   Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

   Blynk is a platform with iOS and Android apps to control
   Arduino, Raspberry Pi and the likes over the Internet.
   You can easily build graphic interfaces for all your
   projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Social networks:            http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

   Blynk library is licensed under MIT license
   This example code is in public domain.

*************************************************************
   This example runs directly on NodeMCU.

   Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

   Please be sure to select the right NodeMCU module
   in the Tools -> Board menu!

   For advanced settings please follow ESP examples :
   - ESP8266_Standalone_Manual_IP.ino
   - ESP8266_Standalone_SmartConfig.ino
   - ESP8266_Standalone_SSL.ino

   Change WiFi ssid, pass, and Blynk auth token to run :)
   Feel free to apply it to any other example. It's simple!
*************************************************************/

/* Comment this out to disable prints and save space */
//#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp8266.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#else
#include <WiFi.h>          //https://github.com/esp8266/Arduino
#endif

//needed for library
#include <DNSServer.h>
#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

// Khai báo các định nghĩa
//BlynkTimer timer;
void CheckConnection();
#define LED_BLINK 4
#define RELAY_1 7
#define RELAY_2 6
#define RELAY_3 5
#define RELAY_4 2
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "619b8b83aa1c44ef99a107dedece4a3d"; //VKTE.IoT08

// Your WiFi credentials.
// Set password to "" for open networks.
// char ssid[] = "netis";
// char pass[] = "password";
// WidgetTerminal terminal(V1);

void setup()
{
        // Debug console
        Serial.begin(9600);
        pinMode(LED_BLINK, OUTPUT);
        pinMode(RELAY_1, OUTPUT);
        pinMode(RELAY_2, OUTPUT);
        pinMode(RELAY_3, OUTPUT);
        pinMode(RELAY_4, OUTPUT);
        WiFiManager wifiManager;
//wifiManager.resetSettings(); //Comment nếu không muốn reset pass wifi đã ghi nhớ
        wifiManager.autoConnect("AutoConnectAP");
        Blynk.config(auth);
        Blynk.connect();
        // You can also specify server:
        //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
        //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);
        //timer.setInterval(10000L, CheckConnection);
}

BLYNK_CONNECTED() {
        Blynk.syncAll();
        // terminal.println("Đã đồng bộ");
        // terminal.flush();
}

BLYNK_WRITE(V2) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);

        digitalWrite(RELAY_1, pinValue);
        digitalWrite(LED_BLINK, pinValue);



        // process received value
}
BLYNK_WRITE(V3) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);

        digitalWrite(RELAY_2, pinValue);
        digitalWrite(LED_BLINK, pinValue);



        // process received value
}
BLYNK_WRITE(V4) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);

        digitalWrite(RELAY_3, pinValue);
        digitalWrite(LED_BLINK, pinValue);



        // process received value
}
BLYNK_WRITE(V5) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);

        digitalWrite(RELAY_4, pinValue);
        digitalWrite(LED_BLINK, pinValue);



        // process received value
}
// void CheckConnection() {   // check every 11s if connected to Blynk server
//         if (!Blynk.connected()) {
//                 // terminal.println("Mất kết nối");
//                 // terminal.flush();
//                 Blynk.connect(); // try to connect to server with default timeout
//         }
//         else {
//                 // terminal.println("Đang kết nối");
//                 // terminal.flush();
//         }
// }

void loop() {
        if (Blynk.connected()) {
                Blynk.run();
                // terminal.println("Đã kết nối");
        }
        //timer.run();
}
