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
   Attention! Please check out TinyGSM guide:
    http://tiny.cc/tiny-gsm-readme

   WARNING: GSM modem support is for BETA testing.

   Change GPRS apm, user, pass, and Blynk auth token to run :)
   Feel free to apply it to any other example. It's simple!

*************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

// Select your modem:
#define TINY_GSM_MODEM_SIM800
//#define TINY_GSM_MODEM_SIM900
//#define TINY_GSM_MODEM_M590
//#define TINY_GSM_MODEM_A6

// Default heartbeat interval for GSM is 60
// If you want override this value, uncomment and set this option:
//#define BLYNK_HEARTBEAT 30

#include <TinyGsmClient.h>
#include <BlynkSimpleSIM800.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "26916e9f5716461fabcb4fc1c92def26";

// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "v-internet";
char user[] = "";
char pass[] = "";

// Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1
// Chân LED trên arduino
#define LED_BLINK 13
BlynkTimer timer;

// or Software Serial on Uno, Nano
//#include <SoftwareSerial.h>
//SoftwareSerial SerialAT(2, 3); // RX, TX

TinyGsm modem(SerialAT);

// Hàm đọc giá trị Virtual PIN V0 viết giá trị lên LED_BLINK
BLYNK_WRITE(V0) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        Serial.println(pinValue);
        digitalWrite(LED_BLINK, pinValue);

        // process received value
}
void CheckConnection() {   // check every 11s if connected to Blynk server
        if (!Blynk.connected()) {
                Serial.println("Not connected to Blynk server");
                Blynk.connect(); // try to connect to server with default timeout
        }
        else {
                Serial.println("Connected to Blynk server");
        }
}

void setup()
{
        // Debug console
        pinMode(LED_BLINK, OUTPUT);
        Serial.begin(9600);

        delay(10);

        // Set GSM module baud rate
        SerialAT.begin(115200);
        delay(3000);

        // Restart takes quite some time
        // To skip it, call init() instead of restart()
        Serial.println("Initializing modem...");
        modem.restart();

        // Unlock your SIM card with a PIN
        //modem.simUnlock("1234");

        Blynk.begin(auth, modem, apn, user, pass);
        timer.setInterval(5000L, CheckConnection);
}

void loop()
{
        if (Blynk.connected()) {
                Blynk.run();
        }
        timer.run();
}
