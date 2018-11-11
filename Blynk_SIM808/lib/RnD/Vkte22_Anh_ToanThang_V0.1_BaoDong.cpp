/*
   Lập trình sản phẩm báo động khi phát hiện chuyển động con người
   Chạy được chế độ offline và online
   Date: 22/07/2018
   Version: V0
   Author: Do Trung Kien
 */

// Khai báo các định nghĩa
// #define BLYNK_PRINT Serial  // Nếu debug bỏ comment
#define TINY_GSM_MODEM_SIM800

#define TRUE          1
#define FALSE         0

// Chân chức năng trên arduino nano
#define RELAY_3 6 // out STOP_PIN
#define RELAY_2 7 // out UP_PIN - Đèn
#define RELAY_1 8 // out DOWN_PIN - Chuông
#define LED_BLINK  13 // out LED trên NANO

#define A_CHANNEL 2 // in RF D3 hay PIND.2 trên Nano
#define B_CHANNEL 3 // in RF D2 hay PIND.3 trên Nano
#define C_CHANNEL 4 // in RF D1
#define D_CHANNEL 5 // in RF D0

#define SENSORS PIND&B00001100  // Cảm biến tại chân 2 và chân 3 tức PIND.2 và PIND.3
#define LIGHT RELAY_2 // Đèn tại RELAY_2
#define BELL RELAY_3  // Chuông tại RELAY_3
// #define RUNTIME_OFFLINE 120000L  // Thời gian chạy chuông khi offline milliseconds
#define RUNTIME_CheckConnect 10000L  // Thời gian kiểm tra kết nối sau milliseconds
#define RUNTIME_RunNotice 150000L  // Thời gian chạy báo notify cách nhau ít nhất 2 cuộc gọi
// #define DOOR1_MESSAGE "Phát hiện chuyển động cửa 1!"  // Thông báo phát hiện tại Sensor 1
// #define DOOR2_MESSAGE "Phát hiện chuyển động cửa 2!"  // Thông báo phát hiện tại Sensor 2
// #define DOOR12_MESSAGE "Phát hiện chuyển động cả 2 cửa!"  // Thông báo phát hiện tại Sensor 2
#define RESET_MESSAGE "RESET thành công!"  // Thông báo đã tắt gọi cảnh báo
#define PHONE_NUMBER_1 "0916755689"  // Số điện thoại gọi 1
#define PHONE_NUMBER_2 "01688885507"  // Số điện thoại gọi 2
#define EEPROM_ADD 0



// Khai báo các thư viện sử dụng
// #include <avr/wdt.h>  // Hàm chạy Watchdog
#include <TinyGsmClient.h>
#include <BlynkSimpleSIM800.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

// Khai báo các hàm con
void software_Reset(); // Khởi động lại chương trình từ đầu
void CheckConnection(); // Kiểm tra kết nối và khởi động lại chương trình
void RunAlarm(); // Chương trình chính chạy báo động kiểm tra Sensor
void RunAlarm_Offline(); // Chương trình chính chạy báo động khi mất kết nối
void DelayMilFunc(float delayTime, void (func)()); // Hàm chạy khoảng thời gian cho Func
void TurnOnAll(); // Bật tất cả Light và Bell
void TurnAllOff(); // Tắt tất cả Light và Bell
void SyncButton(); // Đồng bộ nút bấm Light và Bell
void RunNotice(); // Chạy thông báo trên điện thoại
void Call_Alarm(); // Chạy chương trình gọi báo động

//Khai báo các biến toàn cục
char auth[] = "ed0c9164887d431fb862d38b2346b9ba"; // Anh vàng Toàn Thắng vkte22
// char server[]          = "blynk-cloud.com";
// unsigned int port      = 8442;

// char apn[]  = "v-internet"; // Thông tin APN của Viettel
char apn[]  = "internet"; // Thông tin APN của Vietnam mobile
char user[] = "";
char pass[] = "";

bool bitEnable = TRUE; // Cho phép chạy phát hiện không
bool bitReset = FALSE;  // Reset các relay
// bool bitAns = TRUE; // Kiểm tra xem có nhấc máy không
byte bitMotion = B00000000; // Phát hiện chuyển động hay không
// Khai báo các kiểu dữ liệu kế thừa
BlynkTimer timer;  //Timer chạy funtion theo chu kỳ thời gian
SoftwareSerial SerialAT(9, 10); // RX, TX chân PWM
TinyGsm modem(SerialAT);

BLYNK_CONNECTED() {
        Blynk.syncVirtual(V1,V2,V3);
        // Blynk.notify(ONLINE_MESSAGE);
}
// Hàm đọc giá trị Virtual PIN V0 viết giá trị lên LED_BLINK
BLYNK_WRITE(V1) //Start/Stop
{
        bool pinValue = param.asInt(); //
        bitEnable = pinValue;
}
BLYNK_WRITE(V2) //Bật tắt đèn độc lập
{
        bool pinValue = param.asInt(); //
        digitalWrite(LIGHT, pinValue);
        digitalWrite(LED_BLINK, pinValue);
}
BLYNK_WRITE(V3) //Bật tắt chuông độc lập
{
        bool pinValue = param.asInt(); //
        digitalWrite(BELL, pinValue);
        digitalWrite(LED_BLINK, pinValue);
}
BLYNK_WRITE(V4) //Reset tắt các chuông
{
        bitReset = FALSE;
        EEPROM.write(EEPROM_ADD, B00000000); // Tắt báo chuông điện thoại
        Blynk.notify(RESET_MESSAGE);
        // Blynk.virtualWrite(V2, 0); // Tắt V2 đèn
        // Blynk.virtualWrite(V3, 0); // Tắt V3 chuông
        // Blynk.syncVirtual(V2,V3); // Đồng bộ 2 chân ảo
        digitalWrite(LED_BLINK, FALSE);
}

void setup()
{
        // Debug console
        // pinMode(RELAY_1, OUTPUT);
        pinMode(RELAY_2, OUTPUT);
        pinMode(RELAY_3, OUTPUT);
        pinMode(LED_BLINK, OUTPUT);

        pinMode(A_CHANNEL, INPUT); // Chân cảm biến 1
        pinMode(B_CHANNEL, INPUT); // Chân cảm biến 2
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

        modem.restart();

        // Unlock your SIM card with a PIN
        //modem.simUnlock("1234");

        //Blynk.begin(auth, modem, apn, user, pass);
        // Blynk.begin tự động kết nối đến khi nào được do đó phải để trong CheckConnection
        timer.setInterval(RUNTIME_CheckConnect, CheckConnection);
        timer.setInterval(RUNTIME_RunNotice, RunNotice);
        //Serial.println("OUT SETUP...");
}

void loop()
{
        if (Blynk.connected()) {
                Blynk.run();
                // Code chạy khi online tại đây
                RunAlarm();
                // wdt_reset();
        }
        else
        {
                //Serial.println("OUT LOOO.....P!!!!!");
                // Code chạy độc lập tại đây khi offline
                // RunAlarm_Offline();
                // wdt_reset();
        }
        timer.run();
        //Serial.println("!!!!! INNNNNNNNNNN LOOP");
        // Code chạy theo vòng loop tại đây
        // RunAlarm();
}

/////////////////////// Thực hiện các hàm con//////////////////////////////////

void software_Reset()
{
        asm volatile ("jmp 0");
}

void CheckConnection() {
        if (!Blynk.connected()) {
                Blynk.begin(auth, modem, apn, user, pass);
        }
}

void RunAlarm(){
        if (bitEnable == TRUE && bitReset == FALSE) {
                switch (SENSORS) {
                case B00000100:  // Phát hiện chuyển động tại Sensor 1
                        SyncButton();
                        TurnOnAll();
                        EEPROM.write(EEPROM_ADD, B00000100);
                        bitReset = TRUE;  // Bật cờ Reset rồi mới chạy tiếp
                        if (bitReset) {
                                bitMotion = B00000100;
                        }
                        Call_Alarm();
                        // Blynk.notify(DOOR1_MESSAGE);
                        // Serial.println(DOOR1_MESSAGE);
                        break;
                case B00001000:  // Phát hiện chuyển động tại Sensor 2
                        SyncButton();
                        TurnOnAll();
                        EEPROM.write(EEPROM_ADD, B00001000);
                        bitReset = TRUE;  // Bật cờ Reset rồi mới chạy tiếp
                        if (bitReset) {
                                bitMotion = B00001000;
                        }
                        Call_Alarm();
                        // Blynk.notify(DOOR2_MESSAGE);
                        // Serial.println(DOOR2_MESSAGE);
                        break;
                case B00001100:  // Phát hiện chuyển động tại cả hai cảm biến
                        SyncButton();
                        TurnOnAll();
                        EEPROM.write(EEPROM_ADD, B00001100);
                        bitReset = TRUE;  // Bật cờ Reset rồi mới chạy tiếp
                        if (bitReset) {
                                bitMotion = B00001100;
                        }
                        Call_Alarm();
                        // Blynk.notify(DOOR12_MESSAGE);
                        // Serial.println(DOOR12_MESSAGE);
                        break;
                default:  // Các trạng thái khác không làm gì cả
                        break;
                }
        }

}

void DelayMilFunc(float delayTime, void (func)()){
        unsigned long endTime = millis() + delayTime;
        while(millis() < endTime)
        {
                func();
                while(millis() < endTime) {}; //Xóa dòng này nếu muốn lặp hàm func, giữ nếu muốn chạy hàm func 1 lần.
        }
}

void TurnOnAll() {
        digitalWrite(LIGHT, TRUE); // Bật V2 đèn
        digitalWrite(BELL, TRUE); // Bật V3 chuông
        digitalWrite(LED_BLINK, TRUE); // Bật LED
}

void TurnAllOff() {
        digitalWrite(LIGHT, FALSE); // Tắt V2 đèn
        digitalWrite(BELL, FALSE); // Tắt V3 chuông
        digitalWrite(LED_BLINK, FALSE); // Tắt LED
}

void SyncButton() {
        Blynk.virtualWrite(V2, 1); // Bật V2 đèn
        Blynk.virtualWrite(V3, 1); // Bật V3 chuông
        Blynk.syncVirtual(V2,V3); // Đồng bộ 2 chân ảo
}
//
void RunNotice() {
        bitMotion = EEPROM.read(EEPROM_ADD);
        if (bitEnable == TRUE) {
                switch (bitMotion) {
                case B00000100: // Phát hiện chuyển động tại Sensor 1
                        bitReset = TRUE; // Bật cờ Reset rồi mới chạy tiếp
                        //Blynk.notify(DOOR1_MESSAGE);
                        Call_Alarm();
                        //Serial.println(DOOR1_MESSAGE);
                        break;
                case B00001000: // Phát hiện chuyển động tại Sensor 2
                        bitReset = TRUE; // Bật cờ Reset rồi mới chạy tiếp
                        //Blynk.notify(DOOR2_MESSAGE);
                        Call_Alarm();
                        //Serial.println(DOOR2_MESSAGE);
                        break;
                case B00001100: // Phát hiện chuyển động tại cả hai cảm biến
                        bitReset = TRUE; // Bật cờ Reset rồi mới chạy tiếp
                        Call_Alarm();
                        //Blynk.notify(DOOR12_MESSAGE);
                        //Serial.println(DOOR12_MESSAGE);
                        break;
                default: // Các trạng thái khác không làm gì cả
                        break;
                }
        }
}

void Call_Alarm() {
        Blynk.disconnect();
        modem.callNumber(PHONE_NUMBER_1);
        DelayMilFunc(45000L,TurnOnAll);
        modem.callHangup();
        modem.callNumber(PHONE_NUMBER_2);
        DelayMilFunc(30000L,TurnOnAll);
        modem.callHangup();
        Blynk.connect(3000000L);
}
