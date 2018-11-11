
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
char auth[] = "c718e21b1f5f4057b196b5e0ef485d53"; //vkte14 - 6 kênh - ông đào
char server[]          = "blynk-cloud.com";
unsigned int port      = 8442;
// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "v-internet";
char user[] = "";
char pass[] = "";
// Khai báo các biến toàn cục
bool Bit_lock = FALSE;
bool Bit_Timer = FALSE;
bool Bit_Timer_2 = FALSE;
char currentTime[9];
char currentDate[11];
// long startTimeInSecs;
// long stopTimeInSecs;
long nowseconds;
long startseconds;
long stopseconds;
// long nowseconds_2;
long startseconds_2;
long stopseconds_2;
// Hardware Serial on Mega, Leonardo, Micro
// #define SerialAT Serial1
// Chân chức năng trên arduino
#define STOP_PIN 6      // Relay số 3
#define UP_PIN 7        // Relay số 2
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
        Blynk.notify("Đồng bộ hóa");
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
                Blynk.notify("Tắt sục 2!");
        } else {
                // Blynk.notify("Tắt thiết bị 3!");
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
                Blynk.notify("Bật sục 1!");
        } else {
                // Blynk.notify("Tắt thiết bị 2!");
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
                Blynk.notify("Tắt sục 1!");
        } else {
                // Blynk.notify("Tắt thiết bị 1!");
        }
}
BLYNK_WRITE(V28) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        digitalWrite(RELAY_4, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        if (pinValue == TRUE) {
                Blynk.notify("Bật sục 2!");
        } else {
                // Blynk.notify("Tắt thiết bị 4!");
        }
}
BLYNK_WRITE(V29) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        digitalWrite(RELAY_5, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        if (pinValue == TRUE) {
                Blynk.notify("Bật thiết bị 5!");
        } else {
                Blynk.notify("Tắt thiết bị 5!");
        }
}
BLYNK_WRITE(V30) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        digitalWrite(RELAY_6, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        if (pinValue == TRUE) {
                Blynk.notify("Bật thiết bị 6!");
        } else {
                Blynk.notify("Tắt thiết bị 6!");
        }
}
BLYNK_WRITE(V4) //Bit_Timer 1
{
        bool pinValue = param.asInt(); // assigning incoming value from pin V4 to a variable
        Bit_Timer = pinValue;         // đổi trạng thái
}
BLYNK_WRITE(V14) //Bit_Timer_2
{
        bool pinValue = param.asInt(); // assigning incoming value from pin V14 to a variable
        Bit_Timer_2 = pinValue;         // đổi trạng thái
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

// Hiển thị đồng hồ thời gian thực
BLYNK_WRITE(V11) {   // Scheduler #1 Time Input widget
        TimeInputParam t(param);
        startseconds =long (t.getStartHour() * 3600L) +long (t.getStartMinute() * 60L);
        stopseconds =long (t.getStopHour() * 3600L) +long (t.getStopMinute() * 60L);
        // Blynk.virtualWrite(V14, startseconds," - ", stopseconds);
        Serial.println("Thực hiện Time Input 1 ");
}
// Scheduler #2 Time Input widget
BLYNK_WRITE(V21) {   // Scheduler #2 Time Input widget
        TimeInputParam t2(param);
        startseconds_2 = long(t2.getStartHour() * 3600L) + long(t2.getStartMinute() * 60L);
        stopseconds_2 = long(t2.getStopHour() * 3600L) + long(t2.getStopMinute() * 60L);
        // Blynk.virtualWrite(V14, startseconds," - ", stopseconds);
        Serial.println("Thực hiện Time Input 2");
}
// Lập trình cho Timer 1
void activetoday(){         // check if schedule #1 or #2 should run today
//        if(year() != 1970) {
//                Blynk.syncVirtual(V0); // sync scheduler #1
//                //Blynk.syncVirtual(V2); // sync scheduler #2
//        }
        nowseconds = long(hour() * 3600L) + long(minute() * 60L) + long(second());
        //Serial.print("nowseconds: ");
        Serial.println(nowseconds);
        //Serial.print("startseconds: ");
        Serial.println(startseconds);
        //Serial.print("stopseconds: ");
        Serial.println(stopseconds);
        // //Serial.print("nowseconds_2: ");
        // Serial.println(nowseconds_2);
        //Serial.print("startseconds_2: ");
        Serial.println(startseconds_2);
        //Serial.print("stopseconds_2: ");
        Serial.println(stopseconds_2);
        // Điều kiện cho Timer 1
        if(nowseconds >= startseconds - 30L && nowseconds <= startseconds + 30L && Bit_Timer == TRUE) {                   // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(UP_PIN, 255);         // turn on virtual LED
                // digitalWrite(LED_BLINK, 255);
                delay(300);
                digitalWrite(UP_PIN, 0);         // turn on virtual LED
                // digitalWrite(LED_BLINK, 0);
                Blynk.notify("Bật sục 1!");
                Serial.println("Schedule 1 started");
                //digitalWrite(15, 1);
        }
        if(nowseconds >= stopseconds - 30L && nowseconds <= stopseconds + 30L && Bit_Timer == TRUE ) {         // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(DOWN_PIN, 255);         // turn OFF virtual LED
                delay(300);
                digitalWrite(DOWN_PIN, 0);
                Blynk.notify("Tắt sục 1!");
                Serial.println("Schedule 1 finished");
                //digitalWrite(15, 0);
        }
        //Điều kiện cho Timer 2
        if(nowseconds >= startseconds_2 - 30L && nowseconds <= startseconds_2 + 30L && Bit_Timer_2 == TRUE) {           // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(RELAY_4, 255); // turn on virtual LED
                delay(300);
                digitalWrite(RELAY_4, 0);
                Blynk.notify("Bật sục 2!");
                Serial.println("Schedule 2 started");
                //digitalWrite(15, 1);
        }
        if(nowseconds >= stopseconds_2 - 30L && nowseconds <= stopseconds_2 + 30L && Bit_Timer_2 == TRUE ) { // 62s on 60s timer ensures 1 trigger command is sent
                digitalWrite(STOP_PIN, 255); // turn OFF virtual LED
                delay(300);
                digitalWrite(STOP_PIN, 0);
                Blynk.notify("Tắt sục 2!");
                Serial.println("Schedule 2 finished");
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
