/*
   Blink 3 LED không dùng delay tận dụng tài nguyên
 */

// KHAI BÁO CÁC THƯ VIỆN LIÊN QUAN
#include <energia.h>
// KHAI BÁO CÁC ĐỊNH NGHĨA
#define LED1 RED_LED
#define LED2 BLUE_LED
#define LED3 GREEN_LED
// TẠO KIỂU DỮ LIỆU CLASS MỚI: tên một lớp các đối tượng là Blink
class Blink {
// Khai báo các biến trong class là private mặc định
// (private/public/protected)
private:
unsigned long previousMillis;
unsigned int intervalTime;
boolean LedState;
uint8_t LedPin;
// Khai báo các hàm khởi tạo và hàm con của kiểu Blink
public:
Blink(uint8_t LED, unsigned long intervalLED);
void Update(void);
};
// TRIỂN KHAI CÁC HÀM CỦA KIỂU DỮ LIỆU Blink
Blink::Blink(uint8_t LED, unsigned long intervalLED) {
        LedPin = LED;
        LedState = false;
        intervalTime = intervalLED;
        pinMode(LedPin, OUTPUT);
}
void Blink::Update(void){
        // lưu timestamp bắt đầu
        unsigned long currentMillis = millis();
        // chuyển trạng thái của LED
        if ((unsigned long)(currentMillis - previousMillis) >= intervalTime) {
                LedState = !LedState;
                digitalWrite(LedPin, LedState);
                // lưu timestamp khi thời gian đạt giá trị intervalLED
                previousMillis= currentMillis;
        }
}
// Khởi tạo các đối tượng kiểu dữ liệu Blink
Blink led_red(LED1, 300);
Blink led_blue(LED2, 600);
Blink led_green(LED3, 1200);
//  Các hàm khởi tạo
void setup() {
}
// Hàm lặp vô hạn
void loop() {
        led_red.Update();
        led_blue.Update();
        led_green.Update();
}
