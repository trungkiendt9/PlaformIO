#include <Arduino.h>
#include "QuanTrac.h"
//1. Hàm setup chạy 1 lần
void Start(){
// Khởi động
        power_on();
        pinMode(POWER_KEY, OUTPUT);
}

// 2. Hàm khới động mạch truyền thông và định vị, chú ý ngắt nên nguồn ngoài khi nạp chương trình
void power_on(){
        uint8_t answer=0;

        // Kiểm tra xem module đã khởi động chưa?
        answer = sendATcommand("AT", "OK", RESPONSE_TIMEOUT_MILLISECONDS);
        if (answer == 0)
        {
                // Bật xung 3s
                digitalWrite(POWER_KEY,HIGH);
                delay(3000);
                digitalWrite(POWER_KEY,LOW);

                // Đợi phản hồi từ module
                while(answer == 0) { // Gửi lệnh AT sau mỗi RESPONSE_TIMEOUT_MILLISECONDS
                        answer = sendATcommand("AT", "OK", RESPONSE_TIMEOUT_MILLISECONDS);
                }
        }
}
