
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
#define CheckConnection_Period 30000   // In milliseconds

#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#FF5733"
#define BLYNK_DARK_BLUE "#5F7CD8"

char auth[] = "f7af72bda29d494fbf24371d833089b7"; // doanctmi2
BlynkTimer timer;
WidgetRTC rtc;
// Khai báo các biến toàn cục
// Khai báo các biến toàn cục
bool FirstConnect = TRUE;
bool FirstTrigger_1 = FALSE;
bool FirstTrigger_2 = FALSE;
bool FirstTrigger_3 = FALSE;
bool FirstTrigger_4 = FALSE;

bool Bit_lock = FALSE;
bool Bit_Timer = FALSE;
bool Bit_Timer_2 = FALSE;
bool Bit_Timer_3 = FALSE;
bool Bit_Timer_4 = FALSE;

char currentTime[9];
char currentDate[11];
// long startTimeInSecs;
// long stopTimeInSecs;
unsigned long nowseconds;

unsigned long startseconds;
unsigned long stopseconds;
unsigned long delta_time_1;

unsigned long startseconds_2;
unsigned long stopseconds_2;

unsigned long startseconds_3;
unsigned long stopseconds_3;

unsigned long startseconds_4;
unsigned long stopseconds_4;

#define STOP_PIN D0
#define UP_PIN D1
#define DOWN_PIN  D2
#define LED_BLINK  D4
#define A_CHANNEL D5
#define B_CHANNEL D6
#define C_CHANNEL D7
#define D_CHANNEL D8

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
        rtc.begin();
        FirstConnect = FALSE;
}
void activetoday();
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
        rtc.begin();
        timer.setInterval(CheckConnection_Period, CheckConnection);
        timer.setInterval(10000L, activetoday);
        delay(1000);

}
// Hàm đọc giá trị Virtual PIN V0 viết giá trị lên LED_BLINK
BLYNK_WRITE(V1) //Send data from app to hardware
{
        Blynk.setProperty(V1, "color", BLYNK_RED);
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        digitalWrite(DOWN_PIN, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        if (pinValue == TRUE && FirstTrigger_1 == FALSE) {
                Blynk.notify("Bật kênh 1!");
                FirstTrigger_1 = TRUE;
        } else if (pinValue == FALSE) {
                Blynk.notify("Tắt kênh 1!");
                FirstTrigger_1 = FALSE;
        }
}
BLYNK_WRITE(V2) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        digitalWrite(UP_PIN, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        if (pinValue == TRUE) {
                Blynk.notify("Bật kênh 2!");
        } else {
                Blynk.notify("Tắt kênh 2!");
        }
        // process received value

}
BLYNK_WRITE(V3) //Send data from app to hardware, hàm chỉ được gọi khi ta bấm nút V0 mà thôi
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);

        digitalWrite(STOP_PIN, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        if (pinValue == TRUE) {
                Blynk.notify("Bật kênh 3!");
        } else {
                Blynk.notify("Tắt kênh 3!");
        }


        // process received value
}

BLYNK_WRITE(V7) //Bit_Timer
{
        Blynk.setProperty(V7, "label", "BẬT/TẮT HẸN GIỜ KÊNH 1");
        Blynk.setProperty(V7, "color", BLYNK_RED);
        bool pinValue = param.asInt(); // assigning incoming value from pin V4 to a variable
        Bit_Timer = pinValue;         // đổi trạng thái
}
BLYNK_WRITE(V8) //Bit_Timer_2
{
        Blynk.setProperty(V8, "label", "BẬT/TẮT HẸN GIỜ KÊNH 2");
        bool pinValue = param.asInt(); // assigning incoming value from pin V14 to a variable
        Bit_Timer_2 = pinValue;         // đổi trạng thái
}
BLYNK_WRITE(V9) //Bit_Timer_3
{
        Blynk.setProperty(V9, "label", "BẬT/TẮT HẸN GIỜ KÊNH 3");
        bool pinValue = param.asInt(); // assigning incoming value from pin V14 to a variable
        Bit_Timer_3 = pinValue;         // đổi trạng thái
}
BLYNK_WRITE(V10) //Bit_Timer_4
{
        Blynk.setProperty(V10, "label", "BẬT/TẮT HẸN GIỜ KÊNH 4");
        bool pinValue = param.asInt(); // assigning incoming value from pin V14 to a variable
        Bit_Timer_4 = pinValue;         // đổi trạng thái
}

BLYNK_WRITE(V11) {   // Scheduler #1 Time Input widget
        Blynk.setProperty(V11, "label", "GIỜ BẬT-TẮT KÊNH 1");
        Blynk.setProperty(V11, "color", BLYNK_RED);
        TimeInputParam t(param);
        startseconds = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
        stopseconds = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
        // Tính thời gian chạy Timer delta_time_1
        if (stopseconds > startseconds) {
                delta_time_1 = stopseconds - startseconds;
        } else if (stopseconds <= startseconds) {
                delta_time_1 = startseconds + 86400 - stopseconds;
        }
        // Blynk.virtualWrite(V14, startseconds," - ", stopseconds);
        Serial.println("Thực hiện Time Input 1 ");
}
// // Scheduler #2 Time Input widget
BLYNK_WRITE(V12) {   // Scheduler #2 Time Input widget
        Blynk.setProperty(V12, "label", "GIỜ BẬT-TẮT KÊNH 2");
        TimeInputParam t2(param);
        // nowseconds_2 = ((hour() * 3600) + (minute() * 60) + second());
        startseconds_2 = (t2.getStartHour() * 3600) + (t2.getStartMinute() * 60);
        stopseconds_2 = (t2.getStopHour() * 3600) + (t2.getStopMinute() * 60);
        // Blynk.virtualWrite(V14, startseconds," - ", stopseconds);
        Serial.println("Thực hiện Time Input 2");
}
// // Scheduler #3 Time Input widget
BLYNK_WRITE(V13) {   // Scheduler #2 Time Input widget
        Blynk.setProperty(V13, "label", "GIỜ BẬT-TẮT KÊNH 3");
        TimeInputParam t3(param);
        // nowseconds_2 = ((hour() * 3600) + (minute() * 60) + second());
        startseconds_3 = (t3.getStartHour() * 3600) + (t3.getStartMinute() * 60);
        stopseconds_3 = (t3.getStopHour() * 3600) + (t3.getStopMinute() * 60);
        // Blynk.virtualWrite(V14, startseconds," - ", stopseconds);
        Serial.println("Thực hiện Time Input 3");
}
// // Scheduler #4 Time Input widget
BLYNK_WRITE(V14) {   // Scheduler #2 Time Input widget
        Blynk.setProperty(V14, "label", "GIỜ BẬT-TẮT KÊNH 4");
        TimeInputParam t4(param);
        // nowseconds_2 = ((hour() * 3600) + (minute() * 60) + second());
        startseconds_4 = (t4.getStartHour() * 3600) + (t4.getStartMinute() * 60);
        stopseconds_4 = (t4.getStopHour() * 3600) + (t4.getStopMinute() * 60);
        // Blynk.virtualWrite(V14, startseconds," - ", stopseconds);
        Serial.println("Thực hiện Time Input 4");
}

// Lập trình cho Timer 1
void activetoday(){         // check if schedule #1 or #2 should run today
//        if(year() != 1970) {
//                Blynk.syncVirtual(V0); // sync scheduler #1
//                //Blynk.syncVirtual(V2); // sync scheduler #2
//        }
        nowseconds = ((hour() * 3600) + (minute() * 60) + second());
        Serial.print("nowseconds: ");
        Serial.println(nowseconds);
        Serial.print("startseconds: ");
        Serial.println(startseconds);
        Serial.print("stopseconds: ");
        Serial.println(stopseconds);
        Serial.println();
        // //Serial.print("nowseconds_2: ");
        // Serial.println(nowseconds_2);
        //Serial.print("startseconds_2: ");
        // Serial.println(startseconds_2);
        //Serial.print("stopseconds_2: ");
        // Serial.println(stopseconds_2);
        // Điều kiện cho Timer 1
        if(nowseconds >= startseconds  && nowseconds <= startseconds + delta_time_1  && Bit_Timer == TRUE) {                   // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(DOWN_PIN, 1);         // turn on virtual LED
                digitalWrite(LED_BLINK, 1);
                Blynk.virtualWrite(V1, 1);
                Blynk.syncVirtual(V1);
                Serial.println("Schedule 1 ..................started");
        }
        if(nowseconds >= stopseconds && nowseconds <= stopseconds + 20 && Bit_Timer == TRUE ) {         // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(DOWN_PIN, 0);         // turn OFF virtual LED
                digitalWrite(LED_BLINK, 0);
                Blynk.virtualWrite(V1, 0);
                FirstTrigger_1 = FALSE;
                Blynk.notify("Tắt kênh 1!");
                Serial.println("Schedule 1 finished######################");
        }
        // //Điều kiện cho Timer 2
        if(nowseconds >= startseconds_2 - 30 && nowseconds <= startseconds_2 + 30 && Bit_Timer_2 == TRUE) {           // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(UP_PIN, 1); // turn on virtual LED
                digitalWrite(LED_BLINK, 1);
                Blynk.notify("Bật kênh 2!");
                Serial.println("Schedule 2 started");
        }
        if(nowseconds >= stopseconds_2 - 30 && nowseconds <= stopseconds_2 + 30 && Bit_Timer_2 == TRUE ) { // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(UP_PIN, 0); // turn OFF virtual LED
                digitalWrite(LED_BLINK, 0);
                Blynk.notify("Tắt kênh 2!");
                Serial.println("Schedule 2 finished");
        }
        // //Điều kiện cho Timer 3
        if(nowseconds >= startseconds_3 - 30 && nowseconds <= startseconds_3 + 30 && Bit_Timer_3 == TRUE) {           // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(STOP_PIN, 1); // turn on virtual LED
                digitalWrite(LED_BLINK, 1);
                Blynk.notify("Bật kênh 3!");
                Serial.println("Schedule 3 started");
        }
        if(nowseconds >= stopseconds_3 - 30 && nowseconds <= stopseconds_3 + 30 && Bit_Timer_3 == TRUE ) { // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(STOP_PIN, 0); // turn OFF virtual LED
                digitalWrite(LED_BLINK, 0);
                Blynk.notify("Tắt kênh 3!");
                Serial.println("Schedule 3 finished");
        }
}
void loop() {
        if (Blynk.connected()) {
                Blynk.run();
        }
        timer.run();
}
