/*
   Kiểm tra lệnh AT command với ESP8266 sử dụng TIVA C
   Thông tin kết nối:
 *  ESP8266   |  TIVA  | Nguồn ngoài (khuyên dùng) hoặc 3.3V của TIVA C
 *  VCC       |  x     | 3.3v
 *  GND       |  GND   | GND
 *  CH_PD     |  x     | 3.3v
 *  RX        |  PB1   | x
 *  TX        |  PB0   | x

   Tham khảo AT command tại https://room-15.github.io/blog/2015/03/26/esp8266-at-command-reference/
 */
// cài đặt thư viện cho platform titiva, framework = energia
#include <energia.h>

void setup() {
        // khởi tạo cho các cổng truyển thông nối tiếp:
        Serial.begin(9600);
        Serial1.begin(9600);
}

void loop() {
        // đọc từ cổng 1, viết lên cổng 0:
        if (Serial1.available()) {
                // đọc bytes đến khi gặp ký tự \n
                String inByte = Serial1.readStringUntil('\n');
                Serial.println(inByte);
        }
        // đọc từ cổng 0, viết lên cổng 1:
        if (Serial.available()) {
                // đọc bytes đến khi gặp ký tự \n
                String inByte2 = Serial.readStringUntil('\n');
                Serial1.println(inByte2);
                //Serial.println(inByte2);
        }

}
