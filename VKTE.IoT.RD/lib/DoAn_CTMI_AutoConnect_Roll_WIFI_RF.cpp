
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>


#if defined(ESP8266)
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
#define Wifi_AP_Timeout 100    // In seconds
#define CheckConnection_Period 120000   // In milliseconds



char auth[] = "f7af72bda29d494fbf24371d833089b7"; // doanctmi2
BlynkTimer timer;

WidgetRTC rtc;


// Khai báo các biến toàn cục
bool Bit_lock = FALSE;
bool FirstConnect = TRUE;
// char currentTime[9];
// char currentDate[11];
// long startTimeInSecs;
// long stopTimeInSecs;
// unsigned int nowseconds;
// unsigned int startseconds;
// unsigned int stopseconds;
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
// Lấy ngày và giờ


///////////////Đồng bộ hóa/////////////////////////////////////

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
///////////////Đồng bộ hóa/////////////////////////////////////
BLYNK_CONNECTED() {
        if(FirstConnect == TRUE)
                Blynk.notify("Đồng bộ hóa");
        Blynk.syncAll();
        FirstConnect = FALSE;
}
// Hàm đọc giá trị Virtual PIN V0 viết giá trị lên LED_BLINK
BLYNK_WRITE(V0) //Send data from app to hardware, hàm chỉ được gọi khi ta bấm nút V0 mà thôi
{
        Blynk.setProperty(V0, "label", "Kênh 1");
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        if (Bit_lock == 0) {
                digitalWrite(STOP_PIN, pinValue);

                delay(300);
                // Blynk.notify("STOP");
                if (pinValue == TRUE) {
                        digitalWrite(LED_BLINK, pinValue);
                        Blynk.notify("Bật V0");
                } else {
                        digitalWrite(LED_BLINK, pinValue);
                        Blynk.notify("Tắt V0");
                }
        }

        // process received value
}

BLYNK_WRITE(V1) //Send data from app to hardware
{
        Blynk.setProperty(V1, "label", "Kênh 2");
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        if (Bit_lock == 0) {
                // digitalWrite(UP_PIN, pinValue);
                // digitalWrite(LED_BLINK, pinValue);
                Blynk.virtualWrite(V0, pinValue);
                Blynk.syncVirtual(V0);
                delay(300);
                // Blynk.notify("UP");
        }

        // process received value

}
BLYNK_WRITE(V2) //Send data from app to hardware
{
        Blynk.setProperty(V2, "label", "Kênh 3");
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        if (Bit_lock == 0) {
                digitalWrite(DOWN_PIN, pinValue);
                digitalWrite(LED_BLINK, pinValue);
                delay(300);
                // Blynk.notify("DOWN");
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
                digitalWrite(LED_BLINK, 1);
                //digitalWrite(STOP_PIN, 1);
                //Serial.print("Bit lock:");
                //Serial.println(Bit_lock);
        }
}


// digitalWrite(LOCK_PIN, pinValue);
// process received value
// }
void setup() {
        // put your setup code here, to run once:
        Serial.begin(9600);

        //WiFiManager
        //Local intialization. Once its business is done, there is no need to keep it around
        WiFiManager wifiManager;
        wifiManager.setTimeout(Wifi_AP_Timeout);
        //reset saved settings

        //wifiManager.resetSettings();

        //set custom ip for portal
        //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

        //fetches ssid and pass from eeprom and tries to connect
        //if it does not connect it starts an access point with the specified name
        //here  "AutoConnectAP"
        //and goes into a blocking loop awaiting configuration
        wifiManager.autoConnect("AutoConnectAP");

        //wifiManager.setConnectTimeout(20);
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

        pinMode(A0, INPUT);
        pinMode(A_CHANNEL, INPUT);
        pinMode(B_CHANNEL, INPUT);
        pinMode(C_CHANNEL, INPUT);
        // pinMode(D_CHANNEL, INPUT);

        //Blynk.begin(auth, ssid, pass);
        //rtc.begin();
        timer.setInterval(CheckConnection_Period, CheckConnection);

        delay(1000);

}



void loop() {
        //Reset mạng wifi đã lưu tên
        int reset_level = analogRead(A0);
        //Serial.print("reset_level: ");
        //Serial.println(reset_level);

        if (reset_level >500) {
                WiFi.disconnect(true);
                delay(2000);
                ESP.reset();
        }
        if (Blynk.connected()) {
                Blynk.run();
                // Code for A_CHANNEL
                int a_rf = digitalRead(A_CHANNEL);
                if (Bit_lock == 0) {
                        digitalWrite(STOP_PIN, a_rf);
                        digitalWrite(LED_BLINK, a_rf);
                        delay(20);
                }
                else {
                        digitalWrite(STOP_PIN, FALSE);
                }
                // Code for B_CHANNEL
                int b_rf = digitalRead(B_CHANNEL);
                if (Bit_lock == 0) {
                        digitalWrite(UP_PIN, b_rf);
                        digitalWrite(LED_BLINK, b_rf);
                        delay(20);
                }
                else {
                        digitalWrite(UP_PIN, FALSE);
                }
                // Code for C_CHANNEL
                int c_rf = digitalRead(C_CHANNEL);
                if (Bit_lock == 0) {
                        digitalWrite(DOWN_PIN, c_rf);
                        digitalWrite(LED_BLINK, c_rf);
                        delay(20);
                }
                else {
                        digitalWrite(DOWN_PIN, FALSE);
                }
        }
        else {
                // Code for A_CHANNEL
                int a_rf = digitalRead(A_CHANNEL);
                if (Bit_lock == 0) {
                        digitalWrite(STOP_PIN, a_rf);
                        digitalWrite(LED_BLINK, a_rf);
                        delay(20);
                }
                else {
                        digitalWrite(STOP_PIN, FALSE);
                }
                // Code for B_CHANNEL
                int b_rf = digitalRead(B_CHANNEL);
                if (Bit_lock == 0) {
                        digitalWrite(UP_PIN, b_rf);
                        digitalWrite(LED_BLINK, b_rf);
                        delay(20);
                }
                else {
                        digitalWrite(UP_PIN, FALSE);
                }
                // Code for C_CHANNEL
                int c_rf = digitalRead(C_CHANNEL);
                if (Bit_lock == 0) {
                        digitalWrite(DOWN_PIN, c_rf);
                        digitalWrite(LED_BLINK, c_rf);
                        delay(20);
                }
                else {
                        digitalWrite(DOWN_PIN, FALSE);
                }

        }


        timer.run();


}


// void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
// {
//         asm volatile ("jmp 0");
// }
