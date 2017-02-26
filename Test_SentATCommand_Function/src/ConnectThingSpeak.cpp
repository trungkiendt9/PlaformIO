/*
  Khoa điện - điện tử
  vktech.edu.vn
  ee.vktech@gmail.com

  Kiểm tra lệnh AT command với ESP8266 sử dụng TIVA C
  Thông tin kết nối:
*  ESP8266   |  TIVA  | Nguồn ngoài (khuyên dùng) hoặc 3.3V của TIVA C
*  VCC       |  x     | 3.3v
*  GND       |  GND   | GND
*  CH_PD     |  x     | 3.3v
*  RX        |  PB1   | x
*  TX        |  PB0   | x
AT commands have to be capitalized, and must terminate with "/r/n".
\r = CR (Carriage Return) // Used as a new line character in Mac OS before X
\n = LF (Line Feed) // Used as a new line character in Unix/Mac OS X
\r\n = CR + LF // Used as a new line character in Windows
Tham khảo AT command tại https://room-15.github.io/blog/2015/03/26/esp8266-at-command-reference/
https://github.com/espressif/esp8266_at/wiki
*/
// cài đặt thư viện cho platform titiva, framework = energia
// xem pinmap tại https://github.com/yuvadm/tiva-c/blob/master/driverlib/pin_map.h
#include <energia.h>
//#include <DHT.h>

// KHAI BÁO CÁC ĐỊNH NGHĨA
const char SSID[] = "netis";
const char SSID_KEY[] = "password";
const char BSSID[] = "04:8d:38:24:ba:3f";
const char URL_current_temp[] = "/update?key=QZLN6Q1ZPQOOG93R&field1="; // https://thingspeak.com/channels/51851
// KHAI BÁO CÁC BIẾN TOÀN CỤC
float NhietDo = 0.00;
char cmd_buff[100];
// KHAI BÁO CÁC HÀM SỬ DỤNG
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout);
int8_t sendATcommand2(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned int timeout);
boolean connect_WIFI();
// CÁC HÀM KHỞI TẠO
void setup() {
  // khởi tạo thành chuỗi cmd_buff
  memset(cmd_buff, '\0', 100);
  // port mặc định giữa máy tính với RX0 và TX0
  Serial.begin(9600);
  // port RX1 và TX1 tương ứng PB0 và PB1; Serial1 giữa ESP8266 và TIVA C
  Serial1.begin(9600);
  // Cấu hình cho ESP8266
  // kiểm tra kết nối
  sendATcommand("AT", "OK", 5000);
  // reset
  sendATcommand("AT+RST", "ready", 5000);
  // cấu hình chế độ station và AP, trạm thu phát
  sendATcommand("AT+CWMODE=3", "OK", 5000);
  // list wifi hiện có
  sendATcommand("AT+CWLAP", "OK", 5000); // bị tràn nếu response[100]
  sendATcommand("AT+CWLAP", "OK", 5000);
  sendATcommand("AT+CWLAP", "OK", 5000);
  // kết nối wifi
  for (int8_t i = 0; i < 10; i++) {
    if (connect_WIFI()==1) {
      break;
    }
  }
  // Bật chế độ đa kết nối
  sendATcommand("AT+CIPMUX=1", "OK", 5000);
  // Hiển thị ip
  sendATcommand("AT+CIFSR", "OK", 5000);
  // Hiển thị SSID wifi nếu kết nối thành công
  sendATcommand("AT+CWJAP?", "OK", 5000);

}

// VÒNG LẶP VÔ HẠN
void loop() {
  /* code */
}

// TRIỂN KHAI CÁC HÀM SỬ DỤNG
// 1. Hàm gửi lệnh AT
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
  char response[1000];
  unsigned long previous;

  memset(response, '\0', 1000);    // xóa buffer

  delay(100);

  while ( Serial1.available() > 0) Serial1.read();   // đọc input

  Serial1.println(ATcommand);    // Gửi lệnh AT

  x = 0;
  previous = millis();

  // Chờ phản hồi
  do {
    if (Serial1.available() != 0) {
      // Nếu có dữ liệu trong buffer UART, đọc và kiểm tra nó với expected_answer
      response[x] = Serial1.read();
      x++;
      // Nếu đúng thì trả kết quả answer = 1, thoát hàm
      if (strstr(response, expected_answer) != NULL)
      {
        answer = 1;
      }

    }
  } while ((answer == 0) && ((millis() - previous) < timeout)); // Nếu sai thì tiếp tục thử lại cho tới hết thời gian timeout
  Serial.println(response);   // In giá trị nhận được để debug
  return answer;
}
// 2. Hàm gửi lệnh AT 2 để gửi dữ liệu
int8_t sendATcommand2(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Khởi tạo lại chuỗi về 0

  delay(100);

  while ( Serial1.available() > 0) Serial1.read();   // Xóa buffer

  Serial1.println(ATcommand);    // Gửi lệnh AT

  x = 0;
  previous = millis();

  // Chờ phản hồi
  do {
    // Nếu có dữ liệu từ UART thì đọc và kiểm tra
    if (Serial1.available() != 0) {
      response[x] = Serial1.read();
      x++;
      // Trả về giá trị 1 nếu nhận được expected_answer1
      if (strstr(response, expected_answer1) != NULL)
      {
        answer = 1;
      }
      // Trả về giá trị 2 nếu nhận được expected_answer2
      else if (strstr(response, expected_answer2) != NULL)
      {
        answer = 2;
      }
    }
  }
  // Đợi time out
  while ((answer == 0) && ((millis() - previous) < timeout));
  Serial.println(response);   // In giá trị nhận được để debug
  return answer;
}
// 3. Hàm kết nối wifi
boolean connect_WIFI(){
  memset(cmd_buff, '\0', 100);
  snprintf(cmd_buff, sizeof(cmd_buff), "AT+CWJAP=\"%s\",\"%s\",\"%s\"",SSID, SSID_KEY, BSSID);
  if (sendATcommand(cmd_buff, "OK", 10000)==1) {
    return 1;
  }
  else {
    return 0;
  };
}
