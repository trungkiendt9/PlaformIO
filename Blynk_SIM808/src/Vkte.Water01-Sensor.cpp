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
char auth[] = "s4Q5Wg9vfa4NpBvws5abc3_TmbWYVL8Z"; // MAY BOM
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
#define AC_SENSOR 2     // Cảm biến máy bơm chạy


SoftwareSerial SerialAT(9, 10); // RX, TX chân PWM
// Bridge widget on virtual pin 1
WidgetBridge bridge1(V1);
WidgetLED led_bom(V0);


TinyGsm modem(SerialAT);

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
        asm volatile ("jmp 0");
}
///////////////Đồng bộ hóa/////////////////////////////////////
BLYNK_CONNECTED() {
        bridge1.setAuthToken("rHWAJx2b6ur3OnZHVT5tnmk37hCFzNQH"); // VAN TONG
        if(FirstConnect == HIGH)
                Blynk.notify("Đồng bộ 1");
        Blynk.syncAll();
        FirstConnect = LOW;
}
BLYNK_WRITE(V0) // Chỉ thị bơm
{
        bool pinValue = param.asInt();
}
void CheckConnection() {   // check every 11s if connected to Blynk server
        if (!Blynk.connected()) {
                software_Reset();
        }
}
static bool ac_sensor = LOW;
static bool previus_ac_sensor = LOW;
void blynkAnotherDevice() // Here we will send HIGH or LOW once per second
{
  // Send value to another device
  if (ac_sensor == HIGH && previus_ac_sensor == LOW) {
    bridge1.digitalWrite(VALVE_PIN, 1023);  // Digital Pin 6 on the second board will be set HIGH
    bridge1.virtualWrite(V5, 1023); // Sends 1 value to BLYNK_WRITE(V5) handler on receiving side.
    Blynk.virtualWrite(V0, 1023);
    led_bom.on();
    previus_ac_sensor = HIGH;
    /////////////////////////////////////////////////////////////////////////////////////////
    // Keep in mind that when performing virtualWrite with Bridge,
    // second board will need to process the incoming command.
    // It can be done by using this handler on the second board:
    //
    //    BLYNK_WRITE(V5){
    //    int pinData = param.asInt(); // pinData variable will store value that came via Bridge
    //    }
    //
    /////////////////////////////////////////////////////////////////////////////////////////
  } else if (ac_sensor == LOW && previus_ac_sensor == HIGH) {
    bridge1.digitalWrite(VALVE_PIN, LOW); // Digital Pin 9 on the second board will be set LOW
    bridge1.virtualWrite(V5, LOW); // Sends 0 value to BLYNK_WRITE(V5) handler on receiving side.
    Blynk.virtualWrite(V0, LOW);
    led_bom.off();
    previus_ac_sensor = LOW;
  }

}
void setup()
{
        delay(1000);
        // Debug console
        pinMode(VALVE_PIN, OUTPUT);
        pinMode(AC_SENSOR, INPUT);

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
        timer.setInterval(1000L, blynkAnotherDevice);
}
void loop()
{
        if (Blynk.connected()) {
                Blynk.run();
        }
        timer.run();
        // wdt_reset();
        ac_sensor = digitalRead(AC_SENSOR);
}
