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
// #include <avr/wdt.h>
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
char auth[] = "9cbc9e573c81490eb886039174187ab2"; // vkteanhquynh
// char server[]          = "blynk-cloud.com";
// unsigned int port      = 8442;
// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "v-internet";
char user[] = "";
char pass[] = "";




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
//
// bool Bit_lock = FALSE;
bool Bit_Timer = FALSE;
bool Bit_Timer_2 = FALSE;
bool Bit_Timer_3 = FALSE;
bool Bit_Timer_4 = FALSE;

bool Bit_E_STOP = TRUE;
bool Bit_DC1 = FALSE; //UP
bool Bit_DC2 = FALSE; //Cấy mạ
bool Bit_DC3 = FALSE; // Quay trái/phải


// chân PWM là chân 3,9,10,11 có tần số là 490Hz, xung trên chân 5,6 có tần số 980Hz.
// Hardware Serial on Mega, Leonardo, Micro
// #define SerialAT Serial1
// Chân chức năng trên arduino
// #define STOP_PIN 6      // Relay số 3 -
// #define UP_PIN 7        // Relay số 2 -
// #define DOWN_PIN  8     // Relay số 1
// #define RELAY_4  3     // Relay số 4
// #define RELAY_5  A5     // Relay số 5
// #define RELAY_6  A4     // Relay số 6
#define PWM_UP_PIN 3 // PWM UP - dây trắng
#define PWM_DOWN_PIN 6 // PWM Cấy mạ - dây đỏ
#define PWM_TURN_PIN 11 // PWM Rẽ trái/phải - dây đen
#define DIR_TURN_PIN 7 // Chân điều khiển rẽ trái/phải

//#define LED_BLINK  13
// #define A_CHANNEL 3
// #define B_CHANNEL 5
// #define C_CHANNEL 2
// #define D_CHANNEL 4



SoftwareSerial SerialAT(9, 10); // RX, TX chân PWM

TinyGsm modem(SerialAT);

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
        asm volatile ("jmp 0");
}

///////////////Đồng bộ hóa/////////////////////////////////////
BLYNK_CONNECTED() {
        if(FirstConnect == TRUE)
//                Blynk.notify("Đồng bộ hóa");
                Blynk.syncAll();
        // rtc.begin();
        FirstConnect = FALSE;
}
// Hàm cho các nút bấm
/// Code điều khiển động cơ máy cấy
BLYNK_WRITE(V0) //Nút dừng khẩn cấp
{

        bool pinValue = param.asInt();
        Bit_E_STOP = pinValue;
        if(Bit_E_STOP == FALSE)
        {
                Blynk.syncVirtual(V2,V4,V6);
                Blynk.virtualWrite(V1, 0);
                Blynk.virtualWrite(V3, 0);
                Blynk.virtualWrite(V5, 0);
                Blynk.virtualWrite(V7, 0);
        }


}

// Code lập trình chạy thẳng tiến
BLYNK_WRITE(V1) // Nút chạy tiến UP
{
        bool pinValue = param.asInt();
        Bit_DC1 = pinValue;
        Blynk.syncVirtual(V2);
}
BLYNK_WRITE(V2)
{
        int pinValue = param.asInt();
        if(Bit_DC1 == TRUE && Bit_E_STOP == TRUE)
        {
                analogWrite(PWM_UP_PIN, pinValue*255/100);
        }
        else
        {
                analogWrite(PWM_UP_PIN, 0);
        }
}
// Code lập trình chạy cấy mạ
BLYNK_WRITE(V3) // Nút chạy cấy mạ
{
        bool pinValue = param.asInt();
        Bit_DC2 = pinValue;
        Blynk.syncVirtual(V4);
}
BLYNK_WRITE(V4)
{
        int pinValue = param.asInt();
        if(Bit_DC2 == TRUE && Bit_E_STOP == TRUE)
        {
                analogWrite(PWM_DOWN_PIN, pinValue*255/100);
        }
        else
        {
                analogWrite(PWM_DOWN_PIN, 0);
        }
}
// Code lập trình chạy rẽ phải trái
BLYNK_WRITE(V5) // Nút chạy rẽ phải
{
        bool pinValue = param.asInt();
        digitalWrite(DIR_TURN_PIN, FALSE);
        Bit_DC3 = pinValue;
        Blynk.syncVirtual(V6);
}
BLYNK_WRITE(V7) // Nút chạy rẽ trái
{
        bool pinValue = param.asInt();
        digitalWrite(DIR_TURN_PIN, TRUE);
        Bit_DC3 = pinValue;
        Blynk.syncVirtual(V6);
}
BLYNK_WRITE(V6)
{
        int pinValue = param.asInt();
        if(Bit_DC3 == TRUE && Bit_E_STOP == TRUE)
        {
                analogWrite(PWM_TURN_PIN, pinValue*255/100);
        }
        else
        {
                analogWrite(PWM_TURN_PIN, 0);
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
                //wdt_reset();
        }
}
void setup()
{
        //wdt_disable();
      //  delay(1000);
        // Debug console
        // pinMode(STOP_PIN, OUTPUT);
        // pinMode(UP_PIN, OUTPUT);
        // pinMode(DOWN_PIN, OUTPUT);
        // pinMode(RELAY_4, OUTPUT);
        // pinMode(RELAY_5, OUTPUT);
        // pinMode(RELAY_6, OUTPUT);


        pinMode(PWM_UP_PIN, OUTPUT);  // Chân PWM 5
        pinMode(PWM_DOWN_PIN, OUTPUT);  // Chân PWM 6
        pinMode(PWM_TURN_PIN, OUTPUT);  // Chân PWM 11
        pinMode(DIR_TURN_PIN, OUTPUT);
        //pinMode(LED_BLINK, OUTPUT);  // Chân 13

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
        //wdt_enable(WDTO_8S);  // Chạy Watchdog 8s tối đa
        //delay(2000);
        //wdt_reset();
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
        // timer.setInterval(10000L, activetoday);
        timer.setInterval(20000L, CheckConnection);
        //wdt_reset();
        // Serial.println("                      ......Done setup!");
}



void loop()
{
        if (Blynk.connected()) {
                Blynk.run();
                //wdt_reset(); // thực hiện reset bộ đếm về 0 sau 8s.
        }
        timer.run();
        // //wdt_reset();
}
