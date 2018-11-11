// ThuNghiemTimer_GPRS(2) -- Thử rút gọn clean chương trình - OK
// Thu nghiem thêm Watchdog timer chống nhiễu làm đơ chip
// Cần chạy boot loader của UNO. tức là coi NANO như UNO mới nạp đc IC
// Khi đó mới chạy được Watchdog của NANO
// Timer chạy liên tục trong khoảng thời gian cài đặt.
// Bỏ được hàm CheckConnection()
// Thử nghiệm lại RTC của Blynk làm sai timer
// Đã sửa thành công lỗi tràn bộ đếm nowseconds bằng biến long
// Chạy song song cả software_Reset và hardware reset với WDT

// #define BLYNK_PRINT Serial
// Select your modem:
#define TINY_GSM_MODEM_SIM800
//#define TINY_GSM_MODEM_SIM900
//#define TINY_GSM_MODEM_M590
//#define TINY_GSM_MODEM_A6

//#define BLYNK_HEARTBEAT 30
#include <avr/wdt.h>
#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <BlynkSimpleSIM800.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

#define TRUE          1
#define FALSE         0
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
// char auth[] = "5dace783f8664ac9b28b3b715d76afad"; // doanctmi
char auth[] = "078f8264bffe422ca9b59a3bfe4df70e"; // sangtaobg01
// char server[]          = "blynk-cloud.com";
// unsigned int port      = 8442;
// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "v-internet";
char user[] = "";
char pass[] = "";
// Khai báo các biến toàn cục #9A12B3 #F660AB
// #define BLYNK_GREEN     "#23C48E"
// #define BLYNK_BLUE      "#04C0F8"
// #define BLYNK_YELLOW    "#ED9D00"
// #define BLYNK_RED       "#FF5733"
// #define BLYNK_DARK_BLUE "#5F7CD8"
// #define BLYNK_CYAN      "#9A12B3"
// #define BLYNK_PINK      "#F660AB"



BlynkTimer timer;
WidgetRTC rtc;
// Khai báo các biến toàn cục
bool FirstConnect = TRUE;
bool FirstTrigger_1 = FALSE;
bool FirstTrigger_2 = FALSE;
bool FirstTrigger_3 = FALSE;
bool FirstTrigger_4 = FALSE;
bool FirstTrigger_5 = FALSE;
bool FirstTrigger_6 = FALSE;

bool Bit_lock = FALSE;
bool Bit_Timer = FALSE;
bool Bit_Timer_2 = FALSE;
bool Bit_Timer_3 = FALSE;
bool Bit_Timer_4 = FALSE;

// char currentTime[9];
// char currentDate[11];
// long startTimeInSecs;
// long stopTimeInSecs;
long nowseconds;

long startseconds;
long stopseconds;
long delta_time_1;

long startseconds_2;
long stopseconds_2;
long delta_time_2;

long startseconds_3;
long stopseconds_3;
long delta_time_3;

long startseconds_4;
long stopseconds_4;
long delta_time_4;

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

// String timeString();
// String dateString();

SoftwareSerial SerialAT(9, 10); // RX, TX chân PWM

TinyGsm modem(SerialAT);

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
        asm volatile ("jmp 0");
}
// Chuyển trạng thái Relay và đèn Led
void Set_Pin(uint8_t pinNumber, bool pinValue){
        digitalWrite(pinNumber, pinValue);
        digitalWrite(LED_BLINK, pinValue);
}
// Tính delta_time cho mỗi Timer
long DeltaTimer(long startseconds, long stopseconds) {
        if (stopseconds > startseconds) {
                long delta_time = stopseconds - startseconds;
                return delta_time;
        } else if (stopseconds <= startseconds) {
                long delta_time = startseconds + 86400L - stopseconds;
                return delta_time;
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
// Hàm cho các nút bấm
BLYNK_WRITE(V1) //Send data from app to hardware
{
        // Blynk.setProperty(V1, "color", BLYNK_RED);
        // Blynk.setProperty(V1, "label", "Kênh 1");
        bool pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        Set_Pin(DOWN_PIN, pinValue);
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
        // Blynk.setProperty(V2, "color", BLYNK_BLUE);
        // Blynk.setProperty(V2, "label", "Kênh 2");
        bool pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        Set_Pin(UP_PIN, pinValue);
        if (pinValue == TRUE && FirstTrigger_2 == FALSE) {
                Blynk.notify("Bật kênh 2!");
                FirstTrigger_2 = TRUE;
        } else if (pinValue == FALSE) {
                Blynk.notify("Tắt kênh 2!");
                FirstTrigger_2 = FALSE;
        }
}
BLYNK_WRITE(V3) //Send data from app to hardware
{
        // Blynk.setProperty(V3, "color", BLYNK_YELLOW);
        // Blynk.setProperty(V3, "label", "Kênh 3");
        bool pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        Set_Pin(STOP_PIN, pinValue);
        if (pinValue == TRUE && FirstTrigger_3 == FALSE) {
                Blynk.notify("Bật kênh 3!");
                FirstTrigger_3 = TRUE;
        } else if (pinValue == FALSE) {
                Blynk.notify("Tắt kênh 3!");
                FirstTrigger_3 = FALSE;
        }
}
BLYNK_WRITE(V4) //Send data from app to hardware
{
        // Blynk.setProperty(V4, "color", BLYNK_GREEN);
        // Blynk.setProperty(V4, "label", "Kênh 4");
        bool pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        Set_Pin(RELAY_4, pinValue);
        if (pinValue == TRUE && FirstTrigger_4 == FALSE) {
                Blynk.notify("Bật kênh 4!");
                FirstTrigger_4 = TRUE;
        } else if (pinValue == FALSE) {
                Blynk.notify("Tắt kênh 4!");
                FirstTrigger_4 = FALSE;
        }
}
BLYNK_WRITE(V5) //Send data from app to hardware
{
        // Blynk.setProperty(V5, "color", BLYNK_PINK);
        // Blynk.setProperty(V5, "label", "Kênh 5");
        bool pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        Set_Pin(RELAY_5, pinValue);
        if (pinValue == TRUE && FirstTrigger_5 == FALSE) {
                Blynk.notify("Bật kênh 5!");
                FirstTrigger_5 = TRUE;
        } else if (pinValue == FALSE) {
                Blynk.notify("Tắt kênh 5!");
                FirstTrigger_5 = FALSE;
        }
}
BLYNK_WRITE(V6) //Send data from app to hardware
{
        // Blynk.setProperty(V6, "color", BLYNK_CYAN);
        // Blynk.setProperty(V6, "label", "Kênh 6");
        bool pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        Set_Pin(RELAY_6, pinValue);
        if (pinValue == TRUE && FirstTrigger_6 == FALSE) {
                Blynk.notify("Bật kênh 6!");
                FirstTrigger_6 = TRUE;
        } else if (pinValue == FALSE) {
                Blynk.notify("Tắt kênh 6!");
                FirstTrigger_6 = FALSE;
        }
}
BLYNK_WRITE(V7) //Bit_Timer
{
        // Blynk.setProperty(V7, "label", "BẬT/TẮT HẸN GIỜ KÊNH 1");
        // Blynk.setProperty(V7, "color", BLYNK_RED);
        bool pinValue = param.asInt(); // assigning incoming value from pin V4 to a variable
        Bit_Timer = pinValue;         // đổi trạng thái
}
BLYNK_WRITE(V8) //Bit_Timer_2
{
        // Blynk.setProperty(V8, "label", "BẬT/TẮT HẸN GIỜ KÊNH 2");
        // Blynk.setProperty(V8, "color", BLYNK_BLUE);
        bool pinValue = param.asInt(); // assigning incoming value from pin V14 to a variable
        Bit_Timer_2 = pinValue;         // đổi trạng thái
}
BLYNK_WRITE(V9) //Bit_Timer_3
{
        // Blynk.setProperty(V9, "label", "BẬT/TẮT HẸN GIỜ KÊNH 3");
        // Blynk.setProperty(V9, "color", BLYNK_YELLOW);
        bool pinValue = param.asInt(); // assigning incoming value from pin V14 to a variable
        Bit_Timer_3 = pinValue;         // đổi trạng thái
}
BLYNK_WRITE(V10) //Bit_Timer_4
{
        // Blynk.setProperty(V10, "label", "BẬT/TẮT HẸN GIỜ KÊNH 4");
        // Blynk.setProperty(V10, "color", BLYNK_GREEN);
        bool pinValue = param.asInt(); // assigning incoming value from pin V14 to a variable
        Bit_Timer_4 = pinValue;         // đổi trạng thái
}




BLYNK_WRITE(V11) {   // Scheduler #1 Time Input widget
        // Blynk.setProperty(V11, "label", "GIỜ BẬT-TẮT KÊNH 1");
        // Blynk.setProperty(V11, "color", BLYNK_RED);
        TimeInputParam t(param);
        startseconds =long (t.getStartHour() * 3600L) +long (t.getStartMinute() * 60L);
        stopseconds =long (t.getStopHour() * 3600L) +long (t.getStopMinute() * 60L);
        // Tính thời gian chạy Timer delta_time_1
        delta_time_1 = DeltaTimer(startseconds, stopseconds);
        //Serial.println("Thực hiện Time Input 1 ");
}
// // Scheduler #2 Time Input widget
BLYNK_WRITE(V12) {   // Scheduler #2 Time Input widget
        // Blynk.setProperty(V12, "label", "GIỜ BẬT-TẮT KÊNH 2");
        // Blynk.setProperty(V12, "color", BLYNK_BLUE);
        TimeInputParam t2(param);
        startseconds_2 = long(t2.getStartHour() * 3600L) + long(t2.getStartMinute() * 60L);
        stopseconds_2 = long(t2.getStopHour() * 3600L) + long(t2.getStopMinute() * 60L);
        // Tính thời gian chạy Timer delta_time_2
        delta_time_2 = DeltaTimer(startseconds_2, stopseconds_2);
        //Serial.println("Thực hiện Time Input 2");
}
// // Scheduler #3 Time Input widget
BLYNK_WRITE(V13) {   // Scheduler #3 Time Input widget
        // Blynk.setProperty(V13, "label", "GIỜ BẬT-TẮT KÊNH 3");
        // Blynk.setProperty(V13, "color", BLYNK_YELLOW);
        TimeInputParam t3(param);
        startseconds_3 = long(t3.getStartHour() * 3600L) + long(t3.getStartMinute() * 60L);
        stopseconds_3 = long(t3.getStopHour() * 3600L) + long(t3.getStopMinute() * 60L);
        // Tính thời gian chạy Timer delta_time_3
        delta_time_3 = DeltaTimer(startseconds_3, stopseconds_3);
        //Serial.println("Thực hiện Time Input 3");
}
// // Scheduler #4 Time Input widget
BLYNK_WRITE(V14) {   // Scheduler #4 Time Input widget
        // Blynk.setProperty(V14, "label", "GIỜ BẬT-TẮT KÊNH 4");
        // Blynk.setProperty(V14, "color", BLYNK_GREEN);
        TimeInputParam t4(param);
        startseconds_4 = long(t4.getStartHour() * 3600L) + long(t4.getStartMinute() * 60L);
        stopseconds_4 = long(t4.getStopHour() * 3600L) + long(t4.getStopMinute() * 60L);
        // Tính thời gian chạy Timer delta_time_4
        delta_time_4 = DeltaTimer(startseconds_4, stopseconds_4);
        //Serial.println("Thực hiện Time Input 4");
}

// Lập trình cho Timer 1
void activetoday(){         // check if schedule #1 or #2 should run today
        // Serial.println(dateString()+" "+timeString()+" in setup 1. ");
        nowseconds = long(hour() * 3600L) + long(minute() * 60L) + long(second());
        // // Serial.print("nowseconds: ");
        // Serial.println(nowseconds);
        // // Serial.print("startseconds: ");
        // Serial.println(startseconds);
        // // Serial.print("stopseconds: ");
        // Serial.println(stopseconds);
        // // Serial.println();

        // Điều kiện cho Timer 1
        if(nowseconds >= startseconds  && nowseconds <= startseconds + delta_time_1  && Bit_Timer == TRUE) {                   // 62s on 60s timer ensures 1 trigger command is sent
                Set_Pin(DOWN_PIN, 1);
                Blynk.virtualWrite(V1, 1);
                Blynk.syncVirtual(V1);
                //Serial.println("Schedule 1 ..................started");
        }
        if(nowseconds >= stopseconds && nowseconds <= stopseconds + 20L && Bit_Timer == TRUE ) {         // 62s on 60s timer ensures 1 trigger command is sent
                Set_Pin(DOWN_PIN, 0);         // turn OFF virtual LED
                Blynk.virtualWrite(V1, 0);
                FirstTrigger_1 = FALSE;
                Blynk.notify("Tắt kênh 1!");
                //Serial.println("Schedule 1 finished######################");
        }
        // //Điều kiện cho Timer 2
        if(nowseconds >= startseconds_2  && nowseconds <= startseconds_2 + delta_time_2  && Bit_Timer_2 == TRUE) {                   // 62s on 60s timer ensures 1 trigger command is sent
                Set_Pin(UP_PIN, 1);         // turn on virtual LED
                Blynk.virtualWrite(V2, 1);
                Blynk.syncVirtual(V2);
                //Serial.println("Schedule 2 ..................started");
        }
        if(nowseconds >= stopseconds_2 && nowseconds <= stopseconds_2 + 20L && Bit_Timer_2 == TRUE ) {         // 62s on 60s timer ensures 1 trigger command is sent
                Set_Pin(UP_PIN, 0);         // turn OFF virtual LED
                Blynk.virtualWrite(V2, 0);
                FirstTrigger_2 = FALSE;
                Blynk.notify("Tắt kênh 2!");
                //Serial.println("Schedule 2 finished######################");
        }
        // //Điều kiện cho Timer 3
        if(nowseconds >= startseconds_3  && nowseconds <= startseconds_3 + delta_time_3  && Bit_Timer_3 == TRUE) {                   // 62s on 60s timer ensures 1 trigger command is sent
                Set_Pin(STOP_PIN, 1);         // turn on virtual LED
                Blynk.virtualWrite(V3, 1);
                Blynk.syncVirtual(V3);
                //Serial.println("Schedule 3 ..................started");
        }
        if(nowseconds >= stopseconds_3 && nowseconds <= stopseconds_3 + 20L && Bit_Timer_3 == TRUE ) {         // 62s on 60s timer ensures 1 trigger command is sent
                Set_Pin(STOP_PIN, 0);         // turn OFF virtual LED
                Blynk.virtualWrite(V3, 0);
                FirstTrigger_3 = FALSE;
                Blynk.notify("Tắt kênh 3!");
                //Serial.println("Schedule 3 finished######################");
        }
        // //Điều kiện cho Timer 4
        if(nowseconds >= startseconds_4  && nowseconds <= startseconds_4 + delta_time_4  && Bit_Timer_4 == TRUE) {                   // 62s on 60s timer ensures 1 trigger command is sent
                Set_Pin(RELAY_4, 1);         // turn on virtual LED
                Blynk.virtualWrite(V4, 1);
                Blynk.syncVirtual(V4);
                //Serial.println("Schedule 4 ..................started");
        }
        if(nowseconds >= stopseconds_4 && nowseconds <= stopseconds_4 + 20L && Bit_Timer_4 == TRUE ) {         // 62s on 60s timer ensures 1 trigger command is sent
                Set_Pin(RELAY_4, 0);         // turn OFF virtual LED
                Blynk.virtualWrite(V4, 0);
                FirstTrigger_4 = FALSE;
                Blynk.notify("Tắt kênh 4!");
                //Serial.println("Schedule 4 finished######################");
        }
}
void CheckConnection() {   // check every 11s if connected to Blynk server
        if (!Blynk.connected()) {
                //Serial.println("Khong ket noi toi Blynk server");
                //Blynk.connectNetwork(apn, user, pass); // try to connect to server with default timeout
                //Blynk.connect(10000);
                software_Reset();
                // wdt_reset();
        }
        else {
                //Serial.println("Da ket noi Blynk server");
                wdt_reset();
        }
}
void setup()
{
        wdt_disable();
        delay(1000);
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
        // Serial.println("Khởi động.........");
        Blynk.begin(auth, modem, apn, user, pass);
        delay(1000);
        // timer.setInterval(20000L, CheckConnection);
        wdt_enable(WDTO_8S);  // Chạy Watchdog 8s tối đa
        delay(2000);
        wdt_reset();
        while (Blynk.connect() == false) {};
        rtc.begin();          // give RTC a second to sync up
        delay(1000);
        // setSyncInterval(15*60);
        // Serial.println();
        // Serial.println(millis());
        Blynk.run();            // Try to initialize RTC
        Blynk.run();            // Try to initialize RTC
        Blynk.run();            // Try to initialize RTC
        Blynk.run();            // Try to initialize RTC
        Blynk.run();            // Try to initialize RTC
        // Serial.println(millis());
        // Serial.println(dateString()+" "+timeString()+" in setup 1. ");
        timer.setInterval(10000L, activetoday);
        timer.setInterval(20000L, CheckConnection);
        wdt_reset();
        // Serial.println("                      ......Done setup!");
}



void loop()
{
        if (Blynk.connected()) {
                Blynk.run();
                wdt_reset(); // thực hiện reset bộ đếm về 0 sau 8s.
        }
        timer.run();
        // wdt_reset();
}

// ///Debug
// String timeString()
// {
//         char c[20];  // long enough to hold complete string
//         sprintf(c, "%02dh%02d:%02d",hour(),minute(),second());
//         String currentTime= String(c);
//         return currentTime;
// }
//
// String dateString()
// {
//         char c[20];  // long enough to hold complete string
//         sprintf(c, "%02d/%02d/%4d",day(),month(),year());
//         String currentDate= String(c);
//         return currentDate;
// }
