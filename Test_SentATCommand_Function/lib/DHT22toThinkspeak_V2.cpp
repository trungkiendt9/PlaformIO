/*
   Khoa điện - điện tử
   vktech.edu.vn
   ee.vktech@gmail.com
   trungkiendt9@gmail.com
   //////////////////////////////////////////////////////////
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
   // Kết nối với DHT22
   // DHT       | TIVA C
   //----------------
   // VCC(1)    |  3.3V
   // DATA(2)   |  PD0
   // NC(3)     |  x
   // GND(4)    |  GND

   // Kết nối chân 1 của DHT với 3.3V
   // Chân 2 kết nối với PD0 của TIVA C
   // Chân 4 nối với GND
   // Nối trở 10k giữa chân 1 và chân 2
 */
// cài đặt thư viện cho platform titiva, framework = energia
// KHAI BÁO CÁC THƯ VIỆN SỬ DỤNG
#include <energia.h>
#include "DHT.h"

// KHAI BÁO CÁC ĐỊNH NGHĨA
#define DHTPIN PD_0     // Chân DATA nối với PD0
#define DHTTYPE DHT22   // DHt22  (AM2302), AM2321
const char SSID[] = "netis";
const char SSID_KEY[] = "password";
const char BSSID[] = "04:8d:38:24:ba:3f";
const char IP_SERVER[] = "184.106.153.149"; // IP của thingspeak.com
const char URL_current_temp[] = "/update?key=QZLN6Q1ZPQOOG93R"; // https://thingspeak.com/channels/51851
// Gửi nhiều &field1=58&field2=23&field3=98&field4=12&field5=25&field6=892&field7=33&field8=0
// KHAI BÁO CÁC BIẾN TOÀN CỤC
char cmd_buff[150];
int8_t cmd_length = 0;
unsigned long interval=58000; // Thời gian delay lấy mẫu
unsigned long previousMillis=0; // millis() trả về số unsigned long.
// KHAI BÁO CÁC HÀM SỬ DỤNG
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout);
int8_t sendATcommand2(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned int timeout);
boolean connect_WIFI();
boolean connect_Server();
void get_data(float number1, float number2);
// CÁC HÀM KHỞI TẠO
DHT dht(DHTPIN, DHTTYPE);
void setup() {
        // bắt đầu đọc dữ liệu cảm biến DHT22
        dht.begin();
        // khởi tạo thành chuỗi cmd_buff
        memset(cmd_buff, '\0', 100);
        // port mặc định giữa máy tính với RX0 và TX0 của TIVA
        Serial.begin(9600);
        // port RX1 và TX1 tương ứng PB0 và PB1; Serial1 giữa ESP8266 và TIVA C
        Serial1.begin(9600);
        // Cấu hình cho ESP8266
        // kiểm tra kết nối
        sendATcommand("AT", "OK", 5000);
        // reset
        sendATcommand("AT+RST", "ready", 5000);
        // cấu hình chế độ Router or Terminal
        sendATcommand("AT+CWMODE=3", "OK", 5000);
        // list wifi hiện có thử tối đa 10 lần
        for (int8_t i = 0; i < 10; i++) {
                if (sendATcommand("AT+CWLAP", "OK", 5000)==1) { // bị tràn nếu response[100]
                        break;
                }
        }
        // kết nối wifi thử tối đa 10 lần
        for (int8_t i = 0; i < 10; i++) {
                if (connect_WIFI()==1) {
                        break;
                }
        }
        // Hiển thị ip
        sendATcommand("AT+CIFSR", "OK", 5000);
        // Hiển thị SSID wifi nếu kết nối thành công
        sendATcommand("AT+CWJAP?", "OK", 5000);
        // Chạy thử kết nối server
        connect_Server();
        // chạy thử gửi data lên server
        //get_data(25.00);
}

// VÒNG LẶP VÔ HẠN

void loop() {
        // Gửi dữ liệu
        unsigned long currentMillis = millis(); // grab current time
        if ((unsigned long)(currentMillis - previousMillis) >= interval) {
                // Lấy thông số cảm biến về độ ẩm
                float h = dht.readHumidity();
                // Đọc giá trị nhiệt độ C (mặc định)
                float t = dht.readTemperature();
                // Đọc giá trị nhiệt độ F(isFahrenheit = true)
                float f = dht.readTemperature(true);

                // Kiểm tra quá trình đọc thành công hay không
                if (isnan(h) || isnan(t) || isnan(f)) {
                        Serial.println("Failed to read from DHT sensor!");
                        return;
                }
                if (connect_Server()) {
                        get_data(t,h);
                }
                // Kiểm tra
                if (h>=30&&h<=60) {
                        Serial.println("GOOD!. Độ ẩm khá thoải mái......");
                } else if (h>60) {
                        Serial.println("WET!. Độ ẩm cao!!!!!!!!!!!!!");
                } else {
                        Serial.println("DRY DRY!. Độ ẩm thấp");
                }
                previousMillis = millis();
        }

}

// TRIỂN KHAI CÁC HÀM SỬ DỤNG
// 1. Hàm gửi lệnh AT
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout) {

        uint8_t x = 0,  answer = 0;
        char response[1000];
        unsigned long previous;

        memset(response, '\0', 1000); // xóa buffer

        delay(100);

        while ( Serial1.available() > 0) Serial1.read(); // đọc input

        Serial1.println(ATcommand); // Gửi lệnh AT

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
        Serial.println(response); // In giá trị nhận được để debug
        return answer;
}
// 2. Hàm gửi lệnh AT 2 expect answer để gửi dữ liệu
int8_t sendATcommand2(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned int timeout) {

        uint8_t x = 0,  answer = 0;
        char response[100];
        unsigned long previous;

        memset(response, '\0', 100); // Khởi tạo lại chuỗi về 0

        delay(100);

        while ( Serial1.available() > 0) Serial1.read(); // Xóa buffer

        Serial1.println(ATcommand); // Gửi lệnh AT

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
        Serial.println(response); // In giá trị nhận được để debug
        return answer;
}
// 3. Hàm kết nối wifi
boolean connect_WIFI(){
        memset(cmd_buff, '\0', 150);
        snprintf(cmd_buff, sizeof(cmd_buff), "AT+CWJAP=\"%s\",\"%s\",\"%s\"",SSID, SSID_KEY, BSSID);
        if (sendATcommand(cmd_buff, "OK", 10000)==1) {
                Serial.println("OK Connected WIFI");
                return 1;
        }
        else {
                return 0;
        }
}
// 4. Hàm kết nối với server dữ liệu thingspeak.com
boolean connect_Server(){
        memset(cmd_buff, '\0', 150); //Đặt lại giá trị của aux_str
        // Bật chế độ đa kết nối
        sendATcommand("AT+CIPMUX=1", "OK", 5000);
        snprintf(cmd_buff, sizeof(cmd_buff), "AT+CIPSTART=4,\"TCP\",\"%s\",80", IP_SERVER); //Kết nối server thingspeak
        if (sendATcommand2(cmd_buff, "OK",  "ERROR", 5000) == 1) {
                Serial.println("OK Connected Thingspeak");
                return 1;
        }
        else {
                Serial.println("FAILED Connecting to Thingspeak");
                return 0;
        }
}
// 5. Hàm gửi data lên server dữ liệu thingspeak.com
// Hoạt động như 1 TCP Client
void get_data(float number1, float number2) {
        memset(cmd_buff, '\0', 150);
        snprintf(cmd_buff, sizeof(cmd_buff), "GET %s%s%.2f%s%0.2f", URL_current_temp, "&field1=", number1,"&field2=", number2); //Gộp dữ liệu vào cmd_buff
        cmd_length = strlen(cmd_buff) + 2;                // Tính chiều dài dữ liệu, +2 cho \r\n
        snprintf(cmd_buff, sizeof(cmd_buff), "AT+CIPSEND=4,%d", cmd_length); //Gửi kết nối và chờ >
        if (sendATcommand2(cmd_buff, ">", "ERROR", 5000) == 1) {
                snprintf(cmd_buff, sizeof(cmd_buff), "GET %s%s%.2f%s%0.2f", URL_current_temp, "&field1=", number1,"&field2=", number2);
                sendATcommand2(cmd_buff, "SEND OK", "ERROR", 5000);
        }
        else {
                Serial.println("Get failed"); //nếu không gửi được báo failed
        }
        //Đóng tất cả các TCP kết nối trước khi bắt đầu một kết nối khác
        for (int8_t i = 0; i < 10; i++) {
                if (sendATcommand("AT+CIPCLOSE=5", "OK", 5000)==1) {
                        Serial.println("CloseIP done");
                        break;
                }
        }
}
