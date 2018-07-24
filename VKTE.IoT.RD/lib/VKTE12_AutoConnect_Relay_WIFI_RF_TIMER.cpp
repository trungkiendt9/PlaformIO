
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



char auth[] = "4131321677764d759f3dd3db29f53378"; // VKTE12 2c3c19789f1c444b85f133459007d227
BlynkTimer timer;

WidgetRTC rtc;


// Khai báo các biến toàn cục
bool Bit_lock = FALSE;
bool Bit_Timer = FALSE;
char currentTime[9];
char currentDate[11];
long startTimeInSecs;
long stopTimeInSecs;
unsigned int nowseconds;
unsigned int startseconds;
unsigned int stopseconds;
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
void clockDisplay()
{
        // You can call hour(), minute(), ... at any time
        // Please see Time library examples for details

        //  String currentTime = String(hour()) + ":" + minute() + ":" + second();
        //  String currentDate = String(day()) + " " + month() + " " + year();
        sprintf(currentTime, "%02d:%02d:%02d", hour(), minute(), second());
        sprintf(currentDate, "%02d/%02d/%04d", month(), day(), year());
        Serial.print("Current time: ");
        Serial.print(currentTime);
        Serial.print(" ");
        Serial.print(currentDate);
        Serial.println();

        // Send time to the App
        Blynk.virtualWrite(V12, currentTime);
        // Send date to the App
        Blynk.virtualWrite(V13, currentDate);


}
// Hiển thị đồng hồ thời gian thực
BLYNK_WRITE(V11) {   // Scheduler #1 Time Input widget
        TimeInputParam t(param);
        nowseconds = ((hour() * 3600) + (minute() * 60) + second());
        startseconds = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
        stopseconds = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
        Blynk.virtualWrite(V14, startseconds," - ", stopseconds);
        Serial.println("Thực hiện Time Input");


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
        if(nowseconds >= startseconds - 31 && nowseconds <= startseconds + 31 && Bit_Timer == TRUE) {                   // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(STOP_PIN, 255);         // turn on virtual LED
                digitalWrite(LED_BLINK, 255);
                delay(300);
                digitalWrite(LED_BLINK, 1);
                Blynk.notify("Timer bật RELAY 1!");
                Serial.println("Schedule 1 started");
                //digitalWrite(15, 1);
        }
        if(nowseconds >= stopseconds - 31 && nowseconds <= stopseconds + 31 && Bit_Timer == TRUE ) {         // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(STOP_PIN, 0);         // turn OFF virtual LED
                delay(300);
                digitalWrite(LED_BLINK, 0);
                Blynk.notify("Timer tắt RELAY 1!");
                Serial.println("Schedule 1 finished");
                //digitalWrite(15, 0);
        }


}
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
        Blynk.syncAll();
}
// Hàm đọc giá trị Virtual PIN V0 viết giá trị lên LED_BLINK
BLYNK_WRITE(V0) //Send data from app to hardware, hàm chỉ được gọi khi ta bấm nút V0 mà thôi
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);

        digitalWrite(STOP_PIN, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        if (pinValue == TRUE) {
                Blynk.notify("Bật thiết bị 1!");
        } else {
                Blynk.notify("Tắt thiết bị 1!");
        }


        // process received value
}
BLYNK_WRITE(V1) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        digitalWrite(UP_PIN, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        if (pinValue == TRUE) {
                Blynk.notify("Bật thiết bị 2!");
        } else {
                Blynk.notify("Tắt thiết bị 2!");
        }
        // process received value

}
BLYNK_WRITE(V2) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        digitalWrite(DOWN_PIN, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        if (pinValue == TRUE) {
                Blynk.notify("Bật thiết bị 3!");
        } else {
                Blynk.notify("Tắt thiết bị 3!");
        }


        // process received value
}
// BLYNK_WRITE(V3) //Send data from app to hardware
// {
//         bool pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
//         //Serial.println(pinValue);
//         if (pinValue && Bit_lock==TRUE) {
//                 Bit_lock = FALSE; // đổi trạng thái
//                 Blynk.virtualWrite(V10, "Không khóa");
//                 //digitalWrite(STOP_PIN, 0);
//                 //Serial.print("Bit lock:");
//                 //Serial.println(Bit_lock);
//         } else if (pinValue && Bit_lock==FALSE) {
//                 Bit_lock = TRUE;
//                 Blynk.virtualWrite(V10, "Khóa");
//                 digitalWrite(LED_BLINK, 0);
//                 //digitalWrite(STOP_PIN, 1);
//                 //Serial.print("Bit lock:");
//                 //Serial.println(Bit_lock);
//         }
// Bit timer bật hoặc tắt
BLYNK_WRITE(V15) //Bit_Timer
{
        bool pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        Bit_Timer = pinValue;         // đổi trạng thái
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

        // pinMode(A_CHANNEL, INPUT);
        // pinMode(B_CHANNEL, INPUT);
        // pinMode(C_CHANNEL, INPUT);
        // pinMode(D_CHANNEL, INPUT);

        //Blynk.begin(auth, ssid, pass);
        //rtc.begin();
        timer.setInterval(10000L, CheckConnection);
        timer.setInterval(1000L, clockDisplay);
        timer.setInterval(30000L, activetoday);
        delay(1000);
        rtc.begin();
}

void loop() {
        if (Blynk.connected()) {
                Blynk.run();
        }
        // else{
        //
        //         // Code for A_CHANNEL
        //         int a_rf = digitalRead(A_CHANNEL);
        //         if (Bit_lock == 0 && a_rf == 1) {
        //                 Bit_A = ~Bit_A;
        //                 digitalWrite(STOP_PIN, Bit_A);
        //                 digitalWrite(LED_BLINK, Bit_A);
        //                 delay(10);
        //         }
        //
        //         // Code for B_CHANNEL
        //         int b_rf = digitalRead(B_CHANNEL);
        //         if (Bit_lock == 0 && b_rf == 1) {
        //                 Bit_B=~Bit_B;
        //                 digitalWrite(UP_PIN, Bit_B);
        //                 digitalWrite(LED_BLINK, Bit_B);
        //                 delay(10);
        //         }
        //
        //         // Code for C_CHANNEL
        //         int c_rf = digitalRead(C_CHANNEL);
        //         if (Bit_lock == 0 && c_rf == 1) {
        //                 Bit_C=~Bit_C;
        //                 digitalWrite(DOWN_PIN, Bit_C);
        //                 digitalWrite(LED_BLINK, Bit_C);
        //                 delay(10);
        //         }
        //
        // }

        timer.run();
        // Code for A_CHANNEL
        // int a_rf = digitalRead(A_CHANNEL);
        // if (Bit_lock == 0 && a_rf == 1) {
        //         Bit_A = ~Bit_A;
        //         digitalWrite(STOP_PIN, Bit_A);
        //         digitalWrite(LED_BLINK, Bit_A);
        //         delay(10);
        // }
        //
        // // Code for B_CHANNEL
        // int b_rf = digitalRead(B_CHANNEL);
        // if (Bit_lock == 0 && b_rf == 1) {
        //         Bit_B=~Bit_B;
        //         digitalWrite(UP_PIN, Bit_B);
        //         digitalWrite(LED_BLINK, Bit_B);
        //         delay(10);
        // }
        //
        // // Code for C_CHANNEL
        // int c_rf = digitalRead(C_CHANNEL);
        // if (Bit_lock == 0 && c_rf == 1) {
        //         Bit_C=~Bit_C;
        //         digitalWrite(DOWN_PIN, Bit_C);
        //         digitalWrite(LED_BLINK, Bit_C);
        //         delay(10);
        // }

}

// void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
// {
//         asm volatile ("jmp 0");
// }
