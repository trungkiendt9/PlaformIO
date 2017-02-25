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
