/*
   Lập trình sản phẩm báo động khi phát hiện chuyển động con người
   Chạy được chế độ offline và online
   Date: 22/07/2018
   Version: V0
   Author: Do Trung Kien
 */

// Khai báo các định nghĩa
#define BLYNK_PRINT Serial
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
#define RUNTIME_OFFLINE 120000  // Thời gian chạy chuông khi offline milliseconds
#define RUNTIME_CheckConnect 180000  // Thời gian kiểm tra kết nối sau milliseconds
#define DOOR1_MESSAGE "Phát hiện chuyển động cửa 1!"  // Thông báo phát hiện tại Sensor 1
#define DOOR2_MESSAGE "Phát hiện chuyển động cửa 2!"  // Thông báo phát hiện tại Sensor 2
#define DOOR12_MESSAGE "Phát hiện chuyển động cả 2 cửa!"  // Thông báo phát hiện tại Sensor 2


// Khai báo các thư viện sử dụng
#include <TinyGsmClient.h>
#include <BlynkSimpleSIM800.h>
#include <SoftwareSerial.h>

// Khai báo các hàm con
void software_Reset(); // Khởi động lại chương trình từ đầu
void CheckConnection(); // Kiểm tra kết nối và khởi động lại chương trình
void RunAlarm(); // Chương trình chính chạy báo động
void RunAlarm_Offline(); // Chương trình chính chạy báo động khi mất kết nối
void DelayMilFunc(float delayTime, void (func)()); // Hàm chạy khoảng thời gian cho Func
void TurnOnAll(); // Bật tất cả Light và Bell

//Khai báo các biến toàn cục
char auth[] = "ed0c9164887d431fb862d38b2346b9ba"; // Anh vàng Toàn Thắng vkte22
// char server[]          = "blynk-cloud.com";
// unsigned int port      = 8442;

char apn[]  = "v-internet"; // Thông tin APN của Viettel
char user[] = "";
char pass[] = "";

bool bitEnable = TRUE; // Cho phép chạy phát hiện không
bool bitReset = FALSE;  // Reset các relay
// Khai báo các kiểu dữ liệu kế thừa
BlynkTimer timer;  //Timer chạy funtion theo chu kỳ thời gian
SoftwareSerial SerialAT(9, 10); // RX, TX chân PWM
TinyGsm modem(SerialAT);

BLYNK_CONNECTED() {
        Blynk.syncAll();
}
// Hàm đọc giá trị Virtual PIN V0 viết giá trị lên LED_BLINK
BLYNK_WRITE(V1) //Start/Stop
{
        bool pinValue = param.asInt(); //
        bitEnable = pinValue;
        bitReset = FALSE;
}
BLYNK_WRITE(V2) //Bật tắt đèn độc lập
{
        bool pinValue = param.asInt(); //
        digitalWrite(LIGHT, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        bitReset = FALSE;
}
BLYNK_WRITE(V3) //Bật tắt chuông độc lập
{
        bool pinValue = param.asInt(); //
        digitalWrite(BELL, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        bitReset = FALSE;
}
BLYNK_WRITE(V4) //Reset tắt các chuông
{
        bitReset = FALSE;
        Blynk.virtualWrite(V2, 0); // Bật V2 đèn
        Blynk.virtualWrite(V3, 0); // Bật V3 chuông
        Blynk.syncVirtual(V2,V3); // Đồng bộ 2 chân ảo
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
        Serial.println("Khoi dong modem...");

        modem.restart();

        // Unlock your SIM card with a PIN
        //modem.simUnlock("1234");

        //Blynk.begin(auth, modem, apn, user, pass); // Blynk.begin tự động kết nối đến khi nào được do đó phải để trong CheckConnection
        timer.setInterval(RUNTIME_CheckConnect, CheckConnection);
        Serial.println("OUT SETUP...");
}

void loop()
{
        if (Blynk.connected()) {
                Blynk.run();
                RunAlarm();
        }
        else
        {
                Serial.println("OUT LOOO.....P!!!!!");
                // Code chạy độc lập tại đây
                RunAlarm_Offline();
        }
        timer.run();
        Serial.println("!!!!! INNNNNNNNNNN LOOP");
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
                case B00000000:  // Trạng thái đợi
                        break;
                case B00000100:  // Phát hiện chuyển động tại Sensor 1
                        Blynk.virtualWrite(V2, 1); // Bật V2 đèn
                        Blynk.virtualWrite(V3, 1); // Bật V3 chuông
                        Blynk.syncVirtual(V2,V3); // Đồng bộ 2 chân ảo
                        bitReset = TRUE;  // Bật cờ Reset rồi mới chạy tiếp
                        Blynk.notify(DOOR1_MESSAGE);
                        break;
                case B00001000:  // Phát hiện chuyển động tại Sensor 2
                        Blynk.virtualWrite(V2, 1); // Bật V2 đèn
                        Blynk.virtualWrite(V3, 1); // Bật V3 chuông
                        Blynk.syncVirtual(V2,V3); // Đồng bộ 2 chân ảo
                        bitReset = TRUE;  // Bật cờ Reset rồi mới chạy tiếp
                        Blynk.notify(DOOR2_MESSAGE);
                        break;
                case B00001100:  // Phát hiện chuyển động tại cả hai cảm biến
                        Blynk.virtualWrite(V2, 1); // Bật V2 đèn
                        Blynk.virtualWrite(V3, 1); // Bật V3 chuông
                        Blynk.syncVirtual(V2,V3); // Đồng bộ 2 chân ảo
                        bitReset = TRUE;  // Bật cờ Reset rồi mới chạy tiếp
                        Blynk.notify(DOOR12_MESSAGE);
                        break;
                default:  // Các trạng thái khác không làm gì cả
                        break;
                }
        }

}

void RunAlarm_Offline(){
        // if (bitEnable == TRUE && bitReset == FALSE) {
        if (bitReset == FALSE) {
                switch (SENSORS) {
                case B00000000:  // Trạng thái đợi
                        break;
                case B00000100:  // Phát hiện chuyển động tại Sensor 1
                        DelayMilFunc(RUNTIME_OFFLINE, TurnOnAll);
                        break;
                case B00001000:  // Phát hiện chuyển động tại Sensor 2
                        DelayMilFunc(RUNTIME_OFFLINE, TurnOnAll);
                        break;
                case B00001100:  // Phát hiện chuyển động tại cả hai cảm biến
                        DelayMilFunc(RUNTIME_OFFLINE, TurnOnAll);
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
