/*
   Khoa điện - điện tử
   vktech.edu.vn
   ee.vktech@gmail.com
   trungkiendt9@gmail.com
   Kiểm tra lệnh AT GSM/GPRS/GPS... của SIM808 sử dụng DUE
   Thông tin kết nối:
 *  SIM808UNO                                        |  DUE  | Nguồn ngoài 9V (khuyên dùng)
 *  5V                                               |  5V    | 5V
 *  GND                                             |  GND   | GND
 *  TX (J101A in schematic)                         |  RX3   | x
 *  RX (J101C in schematic)                         |  TX3   | x
 */
// cài đặt thư viện cho platform atmelsam, framework =arduino
// Chuyển mã PDU cho các ký tự UCS2 - 16 bit : http://www.multisilicon.com/blog/a22201774~/pdu.htm
// http://m2msupport.net/m2msupport/tutorial-for-simcom-m2m-modules/#SMS
#include <Arduino.h>

void setup() {
        // khởi tạo cho các cổng truyển thông nối tiếp:
        Serial.begin(9600);
        Serial3.begin(9600);
}

void loop() {
        // đọc từ cổng 1, viết lên cổng 0:
        if (Serial3.available()) {
                // đọc bytes đến khi gặp ký tự \n
                String inByte = Serial3.readStringUntil('\n');
                Serial.println(inByte);
        }
        // đọc từ cổng 0, viết lên cổng 1:
        if (Serial.available()) {
                // đọc bytes đến khi gặp ký tự \n
                String inByte2 = Serial.readStringUntil('\n');
                Serial3.println(inByte2);
        }

}
