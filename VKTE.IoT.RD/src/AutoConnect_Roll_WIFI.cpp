
//#define BLYNK_PRINT Serial

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
#define TRUE          1
#define FALSE         0

#include <BlynkSimpleEsp8266.h>

char auth[] = "2c3c19789f1c444b85f133459007d227"; // VKTE09 2c3c19789f1c444b85f133459007d227
BlynkTimer timer;
// Khai báo các biến toàn cục
bool Bit_lock = FALSE;
// Hardware Serial on Mega, Leonardo, Micro
// #define SerialAT Serial1
// Chân chức năng trên arduino
#define STOP_PIN D0
#define UP_PIN D1
#define DOWN_PIN  D2
#define LED_BLINK  D4
#define A_CHANNEL D5
#define B_CHANNEL D6
#define C_CHANNEL D7
#define D_CHANNEL D8


void software_Reset();

void CheckConnection() {   // check every 11s if connected to Blynk server
        if (!Blynk.connected()) {
                // terminal.println("Mất kết nối");
                // terminal.flush();
                Blynk.config(auth);
                Blynk.connect(); // try to connect to server with default timeout
        }
        else{
                //  Serial.println("connected to blynk");
        }
}
// Hàm đọc giá trị Virtual PIN V0 viết giá trị lên LED_BLINK
BLYNK_WRITE(V0) //Send data from app to hardware, hàm chỉ được gọi khi ta bấm nút V0 mà thôi
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        if (Bit_lock == 0) {
                digitalWrite(STOP_PIN, pinValue);
                digitalWrite(LED_BLINK, pinValue);
                delay(300);
        }

        // process received value
}
BLYNK_WRITE(V1) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        if (Bit_lock == 0) {
                digitalWrite(UP_PIN, pinValue);
                digitalWrite(LED_BLINK, pinValue);
                delay(300);
        }
        // process received value

}
BLYNK_WRITE(V2) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        if (Bit_lock == 0) {
                digitalWrite(DOWN_PIN, pinValue);
                digitalWrite(LED_BLINK, pinValue);
                delay(300);
        }

        // process received value
}
BLYNK_WRITE(V3) //Send data from app to hardware
{
        bool pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        if (pinValue && Bit_lock==TRUE) {
                Bit_lock = FALSE; // đổi trạng thái
                Blynk.virtualWrite(V10, "Không khóa");
                //digitalWrite(STOP_PIN, 0);
                //Serial.print("Bit lock:");
                //Serial.println(Bit_lock);
        } else if (pinValue && Bit_lock==FALSE) {
                Bit_lock = TRUE;
                Blynk.virtualWrite(V10, "Khóa");
                digitalWrite(LED_BLINK, 0);
                //digitalWrite(STOP_PIN, 1);
                //Serial.print("Bit lock:");
                //Serial.println(Bit_lock);
        }



        // digitalWrite(LOCK_PIN, pinValue);
        // process received value
}
void setup() {
        // put your setup code here, to run once:
        //Serial.begin(9600);

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
        //Serial.println("connected...yeey :)");
        // delay(60000);
        Blynk.config(auth);
        Blynk.connect();

        pinMode(STOP_PIN, OUTPUT);
        pinMode(UP_PIN, OUTPUT);
        pinMode(DOWN_PIN, OUTPUT);
        pinMode(LED_BLINK, OUTPUT);

        pinMode(A_CHANNEL, INPUT);
        pinMode(B_CHANNEL, INPUT);
        pinMode(C_CHANNEL, INPUT);
        pinMode(D_CHANNEL, INPUT);

        //Blynk.begin(auth, ssid, pass);
        timer.setInterval(10000L, CheckConnection);
}

void loop() {
        if (Blynk.connected()) {
                Blynk.run();
        }
        else{

                // Code for A_CHANNEL
                int a_rf = digitalRead(A_CHANNEL);
                if (Bit_lock == 0) {
                        digitalWrite(STOP_PIN, a_rf);
                        digitalWrite(LED_BLINK, a_rf);
                        delay(120);
                }
                else {
                        digitalWrite(STOP_PIN, FALSE);
                }
                // Code for B_CHANNEL
                int b_rf = digitalRead(B_CHANNEL);
                if (Bit_lock == 0) {
                        digitalWrite(UP_PIN, b_rf);
                        digitalWrite(LED_BLINK, b_rf);
                        delay(120);
                }
                else {
                        digitalWrite(UP_PIN, FALSE);
                }
                // Code for C_CHANNEL
                int c_rf = digitalRead(C_CHANNEL);
                if (Bit_lock == 0) {
                        digitalWrite(DOWN_PIN, c_rf);
                        digitalWrite(LED_BLINK, c_rf);
                        delay(120);
                }
                else {
                        digitalWrite(DOWN_PIN, FALSE);
                }
        }

        timer.run();
        // Code for A_CHANNEL
        int a_rf = digitalRead(A_CHANNEL);
        if (Bit_lock == 0) {
                digitalWrite(STOP_PIN, a_rf);
                digitalWrite(LED_BLINK, a_rf);
                delay(120);
        }
        else {
                digitalWrite(STOP_PIN, FALSE);
        }
        // Code for B_CHANNEL
        int b_rf = digitalRead(B_CHANNEL);
        if (Bit_lock == 0) {
                digitalWrite(UP_PIN, b_rf);
                digitalWrite(LED_BLINK, b_rf);
                delay(120);
        }
        else {
                digitalWrite(UP_PIN, FALSE);
        }
        // Code for C_CHANNEL
        int c_rf = digitalRead(C_CHANNEL);
        if (Bit_lock == 0) {
                digitalWrite(DOWN_PIN, c_rf);
                digitalWrite(LED_BLINK, c_rf);
                delay(120);
        }
        else {
                digitalWrite(DOWN_PIN, FALSE);
        }
}

// void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
// {
//         asm volatile ("jmp 0");
// }
