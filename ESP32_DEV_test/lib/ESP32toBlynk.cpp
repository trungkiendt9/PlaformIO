/**************************************************************
* Blynk is a platform with iOS and Android apps to control
* Arduino, Raspberry Pi and the likes over the Internet.
* You can easily build graphic interfaces for all your
* projects by simply dragging and dropping widgets.
*
*   Downloads, docs, tutorials: http://www.blynk.cc
*   Blynk community:            http://community.blynk.cc
*   Social networks:            http://www.fb.com/blynkapp
*                               http://twitter.com/blynk_app
*
* Blynk library is licensed under MIT license
* This example code is in public domain.
*
**************************************************************
* This example runs directly on ESP32 chip.
*
* Note: This requires ESP32 support package:
*   https://github.com/espressif/arduino-esp32
*
* Please be sure to select the right ESP32 module
* in the Tools -> Board menu!
*
* Change WiFi ssid, pass, and Blynk auth token to run :)
*
**************************************************************/

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "3629ddead6634ac59acdcc05e91b780b";

// Your WiFi credentials.
// Set password to "" for open networks.
const char* my_ssid     = "netis";
const char* my_password = "password";
uint8_t my_bssid[6] = { 0x04, 0x8d, 0x38, 0x24, 0xba, 0x3f};
//begin(char* ssid, char *passphrase = NULL, int32_t channel = 0, const uint8_t* bssid = NULL, bool connect = true);
void setup()
{
        Serial.begin(115200);
        WiFi.begin(my_ssid,my_password,16,my_bssid, true);

        while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
        }

        Serial.println("");
        Serial.println("WiFi connected.");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        Blynk.begin(auth, my_ssid, my_password);
}

void loop()
{
        Blynk.run();
}
