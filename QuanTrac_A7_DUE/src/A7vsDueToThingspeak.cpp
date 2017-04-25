/*
   Khoa điện - điện tử
   vktech.edu.vn
   ee.vktech@gmail.com
   trungkiendt9@gmail.com
   Kết nối DUE vs A7 và các cảm biến, gửi dữ liệu lên Thingspeak IoT.
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

// KHAI BÁO CÁC THƯ VIỆN SỬ DỤNG
#include <Arduino.h>
#include <DHT.h>
// KHAI BÁO CÁC ĐỊNH NGHĨA
#define DHTPIN 2     // Chân DATA nối với chân 2 của Arduino DUE
#define DHTTYPE DHT22   // DHt22  (AM2302), AM2321
const char pin[]="";
const char apn[]="v-internet"; // VPN của Viettel
const char user_name[]=""; // để trống
const char password[]=""; // để trống
char user_number[]="0988004457";
char thong_bao[]="Thiet bi bat dau hoat dong.";
const char IP_SERVER[] = "184.106.153.149"; // IP của thingspeak.com
const char URL_current_temp[] = "/update?key=QZLN6Q1ZPQOOG93R"; // https://thingspeak.com/channels/51851
const unsigned long interval=118000; // Thời gian delay lấy mẫu
const unsigned int cmd_max_len = 150;

// KHAI BÁO CÁC BIẾN TOÀN CỤC
int onModulePin = 2; // Chân kích khởi động 5V cho module A7, thời gian kích mở là 2 giây
char cmd_buff[cmd_max_len]; // Chuỗi ký tự lưu câu lệnh
int8_t cmd_length = 0; // Độ dài của câu lệnh tại
unsigned long previousMillis=0; // millis() trả về số unsigned long.

// KHAI BÁO CÁC HÀM SỬ DỤNG
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout);
int8_t sendATcommand2(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned int timeout);
void power_on();
boolean sms_send(char* phone_number, char* sms_message);
boolean connect_GPRS();
boolean connect_Server();
boolean get_data(float* number1, float* number2);
// CÁC HÀM KHỞI TẠO
DHT dht(DHTPIN, DHTTYPE);
void setup() {
        // Khởi động A7
        //power_on();
        Serial.println("Starting...");
        // Đợi cho A7 khởi động hoàn toàn
        delay(10000);
        // bắt đầu đọc dữ liệu cảm biến DHT22
        dht.begin();
        // khởi tạo thành chuỗi cmd_buff
        memset(cmd_buff, '\0', cmd_max_len);
        // port mặc định giữa máy tính với RX1 và TX1 của DUE
        Serial.begin(115200);
        // port RX1 và TX1 tương ứng PB0 và PB1; Serial1 giữa ESP8266 và TIVA C
        Serial1.begin(115200);
        // Cấu hình cho A7
        // kiểm tra kết nối
        sendATcommand("AT", "OK", 5000);
        // reset
        sendATcommand("AT+RST", "OK", 10000);
        // cấu hình các chân vào ra
        pinMode(onModulePin, OUTPUT);
        //sets the PIN code
        snprintf(cmd_buff, sizeof(cmd_buff), "AT+CPIN=%s", pin);
        sendATcommand(cmd_buff, "OK", 5000);
        Serial.println("Connecting to the network...");
        // Kiểm tra xem thẻ SIM đã được đăng ký vào nhà mạng hay chưa?
        while (sendATcommand2("AT+CREG?", "+CREG: 1,1", "+CREG: 0,3", 5000)== 0) ;
        // Thiết lập SMS mode UCS2
        sendATcommand("AT+CSCS=\"UCS2\"", "OK", 5000);
        // Thiết lập APN nhà mạng
        sendATcommand("AT+CSTT=\"v-internet\",\"\",\"\"", "OK", 5000);
        // Gửi thông báo hoạt động
        sms_send(user_number, thong_bao);
}
// VÒNG LẶP VÔ HẠN
void loop() {

}

//***************TRIỂN KHAI CÁC HÀM SỬ DỤNG*************************************
// 1. Hàm gửi lệnh AT
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout) {

        uint8_t x = 0,  answer = 0;
        char response[300];
        unsigned long previous;

        memset(response, '\0', 300); // xóa buffer

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
        char response[300];
        unsigned long previous;

        memset(response, '\0', 300); // Khởi tạo lại chuỗi về 0

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
// 3. Hàm gửi tin nhắn sms_send gửi bảng ký tự ASCII 8 bit
boolean sms_send(char* phone_number, char* sms_message){
        memset(cmd_buff, '\0', cmd_max_len);
        Serial.print("Setting SMS mode...");
        sendATcommand("AT+CMGF=1", "OK", 1000); // Thiết lập SMS mode
        Serial.println("Sending SMS");
        sprintf(cmd_buff,"AT+CMGS=\"%s\"", phone_number);
        if(sendATcommand(cmd_buff, ">", 5000)) {// gửi sms message
                snprintf(cmd_buff, sizeof(cmd_buff), "%s%c", sms_message, 0x1A); // Kết thúc tin nhắn bằng cách gửi mã ASCII \0x1a  SUB  substitute  ctrl-Z
                //Serial1.println(char(26));
                if (sendATcommand(cmd_buff, "OK", 5000))
                {
                        Serial.println("Sent OK");
                }
                else
                {
                        Serial.println("ERROR");
                }
                return 1;
        }
        else
        {
                Serial.println("FAILED");
                return 0;
        }
}
// 4. Hàm kết nối với server dữ liệu thingspeak.com
boolean connect_Server(){
        memset(cmd_buff, '\0', cmd_max_len); //Đặt lại giá trị của cmd_buff
        // Kết nối vào mạng dữ liệu
        sendATcommand("AT+CGATT=1", "OK", 5000);
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
boolean get_data(float number1, float number2) {
        memset(cmd_buff, '\0', 150);
        snprintf(cmd_buff, sizeof(cmd_buff), "GET %s%s%.2f%s%0.2f", URL_current_temp, "&field1=", number1,"&field2=", number2); //Gộp dữ liệu vào cmd_buff
        cmd_length = strlen(cmd_buff) + 2;                // Tính chiều dài dữ liệu, +2 cho \r\n
        snprintf(cmd_buff, sizeof(cmd_buff), "AT+CIPSEND=4,%d", cmd_length); //Gửi kết nối và chờ >
        if (sendATcommand2(cmd_buff, ">", "ERROR", 5000) == 1) {
                snprintf(cmd_buff, sizeof(cmd_buff), "GET %s%s%.2f%s%0.2f", URL_current_temp, "&field1=", number1,"&field2=", number2);
                if (sendATcommand2(cmd_buff, "SEND OK", "ERROR", 5000)==1) {
                        return 1;
                }
        }
        else {
                Serial.println("Get failed"); //nếu không gửi được báo failed
                return 0;
        }
        //Đóng tất cả các TCP kết nối trước khi bắt đầu một kết nối khác
        // for (int8_t i = 0; i < 100; i++) {
        //         if (sendATcommand("AT+CIPCLOSE=5", "OK", 6000)==1) {
        //                 Serial.println("CloseIP done");
        //                 break;
        //         }
        // }
}
