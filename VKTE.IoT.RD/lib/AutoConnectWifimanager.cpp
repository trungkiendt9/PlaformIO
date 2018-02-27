
#define BLYNK_PRINT Serial

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
////////////////////////


#include <BlynkSimpleEsp8266.h>

char auth[] = "2c3c19789f1c444b85f133459007d227"; // VKTE09 2c3c19789f1c444b85f133459007d227
BlynkTimer timer;

void CheckConnection() {   // check every 11s if connected to Blynk server
        if (!Blynk.connected()) {
                // terminal.println("Mất kết nối");
                // terminal.flush();
                Blynk.config(auth);
                Blynk.connect(); // try to connect to server with default timeout
        }
        else{
                Serial.println("connected to blynk");
        }
}

void setup() {
        // put your setup code here, to run once:
        Serial.begin(9600);

        //WiFiManager
        //Local intialization. Once its business is done, there is no need to keep it around
        WiFiManager wifiManager;
        //reset saved settings
        //wifiManager.resetSettings();

        //set custom ip for portal
        //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

        //fetches ssid and pass from eeprom and tries to connect
        //if it does not connect it starts an access point with the specified name
        //here  "AutoConnectAP"
        //and goes into a blocking loop awaiting configuration
        wifiManager.autoConnect("AutoConnectAP");
        //or use this for auto generated name ESP + ChipID
        //wifiManager.autoConnect();


        //if you get here you have connected to the WiFi
        Serial.println("connected...yeey :)");
        // delay(60000);
        Blynk.config(auth);
        Blynk.connect();

        //Blynk.begin(auth, ssid, pass);
        timer.setInterval(10000L, CheckConnection);
}

void loop() {
        if (Blynk.connected()) {
                Blynk.run();
        }
}
