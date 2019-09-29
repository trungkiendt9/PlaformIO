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

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
// char auth[] = "5dace783f8664ac9b28b3b715d76afad"; // doanctmi
char auth[] = "rHWAJx2b6ur3OnZHVT5tnmk37hCFzNQH"; // VALVE tong
// char server[]          = "blynk-cloud.com";
// unsigned int port      = 8442;
// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "v-internet";
char user[] = "";
char pass[] = "";
// Khai báo các biến toàn cục
// Timer for blynking
BlynkTimer timer;

// Khai báo các biến toàn cục
static bool FirstConnect = HIGH;

// Hardware Serial on Mega, Leonardo, Micro
// Chân chức năng trên arduino
// #define STOP_PIN 6      // Relay số 3 -
// #define UP_PIN 7        // Relay số 2 -
#define VALVE_PIN  8     // Relay số 1
#define LED_BLINK  13   //LED



SoftwareSerial SerialAT(9, 10); // RX, TX chân PWM

WidgetLED led_valve(V5);


TinyGsm modem(SerialAT);

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
        asm volatile ("jmp 0");
}
///////////////Đồng bộ hóa/////////////////////////////////////
BLYNK_CONNECTED() {
        if(FirstConnect == HIGH)
                Blynk.notify("Đồng bộ 2");
        Blynk.syncAll();
        FirstConnect = LOW;
}
BLYNK_WRITE(V5) // Chỉ thị VALVE
{
        bool pinValue = param.asInt();
}
void CheckConnection() {   // check every 11s if connected to Blynk server
        if (!Blynk.connected()) {
                software_Reset();
        }
}

void setup()
{
        delay(1000);
        // Debug console
        pinMode(VALVE_PIN, OUTPUT);

        delay(100);

        // Set GSM module baud rate
        SerialAT.begin(4800); // Rate tối đa của SoftwareSerial là 9600
        delay(3000);
        // Restart takes quite some time
        // To skip it, call init() instead of restart()
        modem.restart();
        //modem.simUnlock("1234");
        // Serial.println("Khởi động.........");
        Blynk.begin(auth, modem, apn, user, pass);
        delay(1000);
        // Chay chu ky de kiem tra ket noi
        timer.setInterval(59898L, CheckConnection);

}
void loop()
{
        if (Blynk.connected()) {
                Blynk.run();
        }
        timer.run();
        // wdt_reset();
}
