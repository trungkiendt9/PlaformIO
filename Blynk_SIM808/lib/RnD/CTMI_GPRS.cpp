
#define BLYNK_PRINT Serial

// Select your modem:
#define TINY_GSM_MODEM_SIM800
//#define TINY_GSM_MODEM_SIM900
//#define TINY_GSM_MODEM_M590
//#define TINY_GSM_MODEM_A6
void software_Reset();


//#define BLYNK_HEARTBEAT 30

#include <TinyGsmClient.h>
#include <BlynkSimpleSIM800.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

#define TRUE          1
#define FALSE         0

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "5dace783f8664ac9b28b3b715d76afad"; //CTMI
// char server[]          = "blynk-cloud.com";
// unsigned int port      = 8442;
// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "v-internet";
char user[] = "";
char pass[] = "";
// Khai báo các biến toàn cục
bool Bit_lock = FALSE;
bool Bit_Timer = FALSE;
bool Bit_Timer_2 = FALSE;
bool Bit_Timer_3 = FALSE;
bool Bit_Timer_4 = FALSE;

char currentTime[9];
char currentDate[11];
// long startTimeInSecs;
// long stopTimeInSecs;
unsigned int nowseconds;
unsigned int startseconds;
unsigned int stopseconds;

unsigned int startseconds_2;
unsigned int stopseconds_2;

unsigned int startseconds_3;
unsigned int stopseconds_3;

unsigned int startseconds_4;
unsigned int stopseconds_4;

// Hardware Serial on Mega, Leonardo, Micro
// #define SerialAT Serial1
// Chân chức năng trên arduino
#define STOP_PIN 6      // Relay số 3 -
#define UP_PIN 7        // Relay số 2 - V
#define DOWN_PIN  8     // Relay số 1
#define RELAY_4  3     // Relay số 4
#define RELAY_5  A5     // Relay số 5
#define RELAY_6  A4     // Relay số 6
#define LED_BLINK  13
// #define A_CHANNEL 3
// #define B_CHANNEL 5
// #define C_CHANNEL 2
// #define D_CHANNEL 4


WidgetRTC rtc;
BlynkTimer timer;

// or Software Serial on Uno, Nano
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(9, 10); // RX, TX chân PWM

TinyGsm modem(SerialAT);
///////////////Đồng bộ hóa/////////////////////////////////////
BLYNK_CONNECTED() {
        Blynk.syncAll();
        //Blynk.notify("Đồng bộ hóa");
}
// Hàm đọc giá trị Virtual PIN V0 viết giá trị lên LED_BLINK
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
BLYNK_WRITE(V1) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        digitalWrite(DOWN_PIN, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        if (pinValue == TRUE) {
                Blynk.notify("Bật kênh 1!");
        } else {
                Blynk.notify("Tắt kênh 1!");
        }
}
BLYNK_WRITE(V4) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        digitalWrite(RELAY_4, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        if (pinValue == TRUE) {
                Blynk.notify("Bật kênh 4!");
        } else {
                Blynk.notify("Tắt kênh 4!");
        }
}
BLYNK_WRITE(V5) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        digitalWrite(RELAY_5, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        if (pinValue == TRUE) {
                Blynk.notify("Bật kênh 5!");
        } else {
                Blynk.notify("Tắt kênh 5!");
        }
}
BLYNK_WRITE(V6) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        digitalWrite(RELAY_6, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        if (pinValue == TRUE) {
                Blynk.notify("Bật kênh 6!");
        } else {
                Blynk.notify("Tắt kênh 6!");
        }
}
BLYNK_WRITE(V7) //Bit_Timer
{
        bool pinValue = param.asInt(); // assigning incoming value from pin V4 to a variable
        Bit_Timer = pinValue;         // đổi trạng thái
}
BLYNK_WRITE(V8) //Bit_Timer_2
{
        bool pinValue = param.asInt(); // assigning incoming value from pin V14 to a variable
        Bit_Timer_2 = pinValue;         // đổi trạng thái
}
BLYNK_WRITE(V9) //Bit_Timer_3
{
        bool pinValue = param.asInt(); // assigning incoming value from pin V14 to a variable
        Bit_Timer_3 = pinValue;         // đổi trạng thái
}
BLYNK_WRITE(V10) //Bit_Timer_4
{
        bool pinValue = param.asInt(); // assigning incoming value from pin V14 to a variable
        Bit_Timer_4 = pinValue;         // đổi trạng thái
}
void CheckConnection() {   // check every 11s if connected to Blynk server
        if (!Blynk.connected()) {
                //Serial.println("Khong ket noi toi Blynk server");
                //Blynk.connectNetwork(apn, user, pass); // try to connect to server with default timeout
                //Blynk.connect(10000);
                software_Reset();
                //Blynk.begin(auth, modem, apn, user, pass);
        }
        else {
                //Serial.println("Da ket noi Blynk server");
        }
}
BLYNK_WRITE(V11) {   // Scheduler #1 Time Input widget
        TimeInputParam t(param);
        nowseconds = ((hour() * 3600) + (minute() * 60) + second());
        startseconds = (t.getStartHour() * 3600) + (t.getStartMinute() * 60);
        stopseconds = (t.getStopHour() * 3600) + (t.getStopMinute() * 60);
        // Blynk.virtualWrite(V14, startseconds," - ", stopseconds);
        Serial.println("Thực hiện Time Input 1 ");
}
// // Scheduler #2 Time Input widget
BLYNK_WRITE(V12) {   // Scheduler #2 Time Input widget
        TimeInputParam t2(param);
        // nowseconds_2 = ((hour() * 3600) + (minute() * 60) + second());
        startseconds_2 = (t2.getStartHour() * 3600) + (t2.getStartMinute() * 60);
        stopseconds_2 = (t2.getStopHour() * 3600) + (t2.getStopMinute() * 60);
        // Blynk.virtualWrite(V14, startseconds," - ", stopseconds);
        Serial.println("Thực hiện Time Input 2");
}
// // Scheduler #3 Time Input widget
BLYNK_WRITE(V13) {   // Scheduler #2 Time Input widget
        TimeInputParam t3(param);
        // nowseconds_2 = ((hour() * 3600) + (minute() * 60) + second());
        startseconds_3 = (t3.getStartHour() * 3600) + (t3.getStartMinute() * 60);
        stopseconds_3 = (t3.getStopHour() * 3600) + (t3.getStopMinute() * 60);
        // Blynk.virtualWrite(V14, startseconds," - ", stopseconds);
        Serial.println("Thực hiện Time Input 3");
}
// // Scheduler #4 Time Input widget
BLYNK_WRITE(V14) {   // Scheduler #2 Time Input widget
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
        //Serial.print("nowseconds: ");
        Serial.println(nowseconds);
        //Serial.print("startseconds: ");
        Serial.println(startseconds);
        //Serial.print("stopseconds: ");
        Serial.println(stopseconds);
        // //Serial.print("nowseconds_2: ");
        // Serial.println(nowseconds_2);
        //Serial.print("startseconds_2: ");
        // Serial.println(startseconds_2);
        //Serial.print("stopseconds_2: ");
        // Serial.println(stopseconds_2);
        // Điều kiện cho Timer 1
        if(nowseconds >= startseconds - 30 && nowseconds <= startseconds + 30 && Bit_Timer == TRUE) {                   // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(DOWN_PIN, 255);         // turn on virtual LED
                digitalWrite(LED_BLINK, 255);
                delay(300);
                Blynk.notify("Bật kênh 1!");
                Serial.println("Schedule 1 started");
                //digitalWrite(15, 1);
        }
        if(nowseconds >= stopseconds - 30 && nowseconds <= stopseconds + 30 && Bit_Timer == TRUE ) {         // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(DOWN_PIN, 0);         // turn OFF virtual LED
                delay(300);
                digitalWrite(LED_BLINK, 0);
                Blynk.notify("Tắt kênh 1!");
                Serial.println("Schedule 1 finished");
                //digitalWrite(15, 0);
        }
        // //Điều kiện cho Timer 2
        if(nowseconds >= startseconds_2 - 30 && nowseconds <= startseconds_2 + 30 && Bit_Timer_2 == TRUE) {           // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(UP_PIN, 255); // turn on virtual LED
                digitalWrite(LED_BLINK, 255);
                delay(300);
                Blynk.notify("Bật kênh 2!");
                Serial.println("Schedule 2 started");
                //digitalWrite(15, 1);
        }
        if(nowseconds >= stopseconds_2 - 30 && nowseconds <= stopseconds_2 + 30 && Bit_Timer_2 == TRUE ) { // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(UP_PIN, 0); // turn OFF virtual LED
                delay(300);
                digitalWrite(LED_BLINK, 0);
                Blynk.notify("Tắt kênh 2!");
                Serial.println("Schedule 2 finished");
                //digitalWrite(15, 0);
        }
        // //Điều kiện cho Timer 3
        if(nowseconds >= startseconds_3 - 30 && nowseconds <= startseconds_3 + 30 && Bit_Timer_3 == TRUE) {           // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(STOP_PIN, 255); // turn on virtual LED
                digitalWrite(LED_BLINK, 255);
                delay(300);
                Blynk.notify("Bật kênh 3!");
                Serial.println("Schedule 3 started");
                //digitalWrite(15, 1);
        }
        if(nowseconds >= stopseconds_3 - 30 && nowseconds <= stopseconds_3 + 30 && Bit_Timer_3 == TRUE ) { // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(STOP_PIN, 0); // turn OFF virtual LED
                delay(300);
                digitalWrite(LED_BLINK, 0);
                Blynk.notify("Tắt kênh 3!");
                Serial.println("Schedule 3 finished");
                //digitalWrite(15, 0);
        }
        // //Điều kiện cho Timer 4
        if(nowseconds >= startseconds_4 - 30 && nowseconds <= startseconds_4 + 30 && Bit_Timer_4 == TRUE) {           // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(RELAY_4, 255); // turn on virtual LED
                digitalWrite(LED_BLINK, 255);
                delay(300);
                Blynk.notify("Bật kênh 4!");
                Serial.println("Schedule 4 started");
                //digitalWrite(15, 1);
        }
        if(nowseconds >= stopseconds_4 - 30 && nowseconds <= stopseconds_4 + 30 && Bit_Timer_4 == TRUE ) { // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(RELAY_4, 0); // turn OFF virtual LED
                delay(300);
                digitalWrite(LED_BLINK, 0);
                Blynk.notify("Tắt kênh 4!");
                Serial.println("Schedule 4 finished");
                //digitalWrite(15, 0);
        }

}
//////////////////////////////////////////////////
void setup()
{
        // Debug console
        pinMode(STOP_PIN, OUTPUT);
        pinMode(UP_PIN, OUTPUT);
        pinMode(DOWN_PIN, OUTPUT);
        pinMode(RELAY_4, OUTPUT);
        pinMode(RELAY_5, OUTPUT);
        pinMode(RELAY_6, OUTPUT);

        pinMode(LED_BLINK, OUTPUT);

        // pinMode(A_CHANNEL, INPUT);
        // pinMode(B_CHANNEL, INPUT);
        // pinMode(C_CHANNEL, INPUT);
        // pinMode(D_CHANNEL, INPUT);

        Serial.begin(4800);

        delay(10);

        // Set GSM module baud rate
        SerialAT.begin(4800); // Rate tối đa của SoftwareSerial là 9600
        delay(3000);

        // Restart takes quite some time
        // To skip it, call init() instead of restart()
        //Serial.println("Khoi dong modem...");
        //delay(3000);
        //software_Reset();
        modem.restart();

        // Unlock your SIM card with a PIN
        //modem.simUnlock("1234");

        // Blynk.config(modem, auth, server, port);
        // Blynk.connectNetwork(apn, user, pass);
        // Blynk.connect();

        Blynk.begin(auth, modem, apn, user, pass);
        rtc.begin();
        timer.setInterval(5000L, CheckConnection);
        // timer.setInterval(3600000L, clockDisplay);
        timer.setInterval(30000L, activetoday);
        //Blynk.virtualWrite(V10, "Không khóa");
}



void loop()
{
        if (Blynk.connected()) {
                Blynk.run();
        }
        timer.run();


}

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
        asm volatile ("jmp 0");
}
