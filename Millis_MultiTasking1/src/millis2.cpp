/*
   Blink 3 LED không dùng delay tận dụng tài nguyên
   vktech.edu.vn
   Khoa điện - điện tử
 */

// KHAI BÁO CÁC THƯ VIỆN LIÊN QUAN
#include <energia.h>
// KHAI BÁO CÁC ĐỊNH NGHĨA
#define LED1 RED_LED
#define LED2 BLUE_LED
#define LED3 GREEN_LED
//xem pins_energia.h về định nghĩa các chân tại
//C:\Users\TRUNGKIEN\.platformio\packages\framework-energiativa\variants\EK-TM4C123GXL
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
// Hàm khởi tạo gán giá trị cho các biến của Blink
Blink::Blink(uint8_t LED, unsigned long intervalLED) {
        LedPin = LED;
        LedState = false;
        intervalTime = intervalLED;
        pinMode(LedPin, OUTPUT);
}
// Chương trình chuyển trạng thái LED
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
Blink led_blue(LED2, 590);
Blink led_green(LED3, 1190);
//  Các hàm khởi tạo
void setup() {
}
// Hàm lặp vô hạn
void loop() {
        led_red.Update();
        led_blue.Update();
        led_green.Update();
}
