/*
   Khoa điện - điện tử
   vktech.edu.vn
   ee.vktech@gmail.com
   trungkiendt9@gmail.com
   Kiểm tra lệnh AT GSM/GPRS/GPS... của A7 sử dụng DUE
   Thông tin kết nối:
 *  A7 Thinker                                        |  DUE  | Nguồn ngoài (khuyên dùng) hoặc 3.3V của DUE
 *  5V                                               |  5V    | 3.3v đến 5V
 *  GND                                             |  GND   | GND
 *  GPS_TXD(37) hoặc UTXD(30) (AT+GPSRD=N NEMA)     |  RX1   | x
 *  URXD(31)                                        |  TX1   | x
   Datasheet English version tại http://smart-prototyping.com/image/data/9_Modules/101756%20Wireless%20A6C/A6_A7_A6C_datasheet-EN.pdf
   Wiki https://www.elecrow.com/wiki/index.php?title=A7_GPRS%2BGSM%2BGPS_Shield
   AT command tại https://www.makerfabs.com/desfile/files/A6A7A6CA20_AT_Commends.pdf
   Module https://www.elecrow.com/wiki/index.php?title=32u4_with_A7_GPRS/GSM
   AT + GPS = 1 to turn on GPS
   AT + GPS = 0 off GPS
   AT + AGPS = 1 Open AGPS
   AT + AGPS = 0 off AGPS
   Open GPS / AGPS, the default NEMA information from the GPS_TXD pin to 9600 baud rate output, if you want to let NEMA
   From the AT serial output, you can use AT + GPSRD.
   AT + GPSRD = 0 OFF NEMA output from AT port
   AT + GPSRD = N (N là chu kỳ gửi tọa độ) NEMA information N seconds output from the AT port once, the actual use of N will be replaced by digital;
   Refer to the following AT documentation and examples of usage
   Đợi 10 giây để A7 khởi động hoàn toàn
 */
// cài đặt thư viện cho platform atmelsam, framework =arduino
// Chuyển mã PDU cho các ký tự UCS2 - 16 bit : http://www.multisilicon.com/blog/a22201774~/pdu.htm
// http://m2msupport.net/m2msupport/tutorial-for-simcom-m2m-modules/#SMS
#include <Arduino.h>

void setup() {
        // khởi tạo cho các cổng truyển thông nối tiếp:
        Serial.begin(115200);
        Serial1.begin(115200);
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
        }

}
