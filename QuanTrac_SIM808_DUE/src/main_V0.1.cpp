/*
 *   Khoa điện - điện tử
 *   vktech.edu.vn
 *   ee.vktech@gmail.com
 *   trungkiendt9@gmail.com
 *   Date: 21/07/2017
 *
 *
   Thông tin kết nối:
 *  SIM808UNO                                        |  DUE  | Nguồn ngoài 9V (khuyên dùng)
 *  5V                                               |  5V    | 5V
 *  GND                                             |  GND   | GND
 *  TX (J101A in schematic)                         |  RX3   | x
 *  RX (J101C in schematic)                         |  TX3   | x
 *  Cài đặt thư viện cho platform atmelsam, framework =arduino
 *  Chuyển mã PDU cho các ký tự UCS2 - 16 bit : http://www.multisilicon.com/blog/a22201774~/pdu.htm
 */
#include <Arduino.h>
#include "QuanTrac.h"
#include "DHT.h"


// CÁC HÀM KHỞI TẠO
DHT dht(DHTPIN, DHTTYPE);
void setup() {
        // khởi tạo thành chuỗi cmd_buff
        memset(GLO_cmd_buff, '\0', AT_CMD_MAX_LENGHT);
        // port mặc định giữa máy tính với RX và TX của DUE
        Serial.begin(9600);
        // port RX3 và TX3 của DUE giáo tiếp với SIMCOM
        Serial3.begin(9600);
        // khởi động
        Start();
        // Kiểm tra xem thẻ SIM đã được đăng ký vào nhà mạng hay chưa?
        while (sendATcommand2("AT+CREG?", "+CREG: 0,1", "+CREG: 0,5", RESPONSE_TIMEOUT_MILLISECONDS)== 0) ;
        // Thiết lập SMS mode UCS2
        sendATcommand("AT+CSCS=\"GSM\"", "OK", RESPONSE_TIMEOUT_MILLISECONDS);
        // Thiết lập APN nhà mạng
        //snprintf(GLO_cmd_buff, sizeof(GLO_cmd_buff), "AT+CSTT=\"%s\"", APN);
        //sendATcommand(GLO_cmd_buff, "OK", RESPONSE_TIMEOUT_MILLISECONDS);
        // Gửi thông báo hoạt động
        //sms_send(USER_PHONE_NUMBER, REPORT_START);
        // Gọi điện tới nhà quản lý
        //float t = 29.11, h = 95.07;
        voice_call(USER_PHONE_NUMBER, AUTO_END_CALL_TIMEOUT);
        //get_data_single_mode(t, h);
}

// VÒNG LẶP VÔ HẠN
void loop() {
        delay(102000);  // trừ hao 18s
        float h = dht.readHumidity();
        float t = dht.readTemperature();
        get_data_single_mode(t, h);

}

// TRIỂN KHAI CÁC HÀM
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
                delay(5000); // Đợi cho module khởi động hoàn tất

                // Đợi phản hồi từ module
                while(answer == 0) { // Gửi lệnh AT sau mỗi RESPONSE_TIMEOUT_MILLISECONDS
                        answer = sendATcommand("AT", "OK", RESPONSE_TIMEOUT_MILLISECONDS);
                }
        }
}
// 3. Hàm gửi lệnh AT với 1 expected_answer
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout) {
        uint8_t x = 0,  answer = 0;

        char response[AT_RESPONSE_MAX_LENGHT];
        unsigned long previous;

        memset(response, '\0', AT_RESPONSE_MAX_LENGHT); // xóa buffer
        delay(100);

        while ( Serial3.available() > 0) Serial3.read(); // đọc input
        Serial3.println(ATcommand); // Gửi lệnh AT
        x = 0;
        previous = millis();
        // Chờ phản hồi
        do {
                if (Serial3.available() != 0) {
                        // Nếu có dữ liệu trong buffer UART, đọc và kiểm tra nó với expected_answer
                        response[x] = Serial3.read();
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
// 4. Hàm gửi lệnh AT 2 expect answer để gửi dữ liệu
int8_t sendATcommand2(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned int timeout) {
        uint8_t x = 0,  answer = 0;

        char response[AT_RESPONSE_MAX_LENGHT];
        unsigned long previous;

        memset(response, '\0', AT_RESPONSE_MAX_LENGHT); // Khởi tạo lại chuỗi về 0
        delay(100);

        while ( Serial3.available() > 0) Serial3.read(); // Xóa buffer
        Serial3.println(ATcommand); // Gửi lệnh AT
        x = 0;
        previous = millis();
        // Chờ phản hồi
        do {
                // Nếu có dữ liệu từ UART thì đọc và kiểm tra
                if (Serial3.available() != 0) {
                        response[x] = Serial3.read();
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
//5. Hàm gửi tin nhắn sms_send gửi bảng ký tự ASCII 8 bit
boolean sms_send(char* phone_number, char* sms_message){
        memset(GLO_cmd_buff, '\0', AT_CMD_MAX_LENGHT);
        Serial.print("Setting SMS mode...");
        sendATcommand("AT+CMGF=1", "OK", RESPONSE_TIMEOUT_MILLISECONDS); // Thiết lập SMS mode
        Serial.println("Sending SMS");
        sprintf(GLO_cmd_buff,"AT+CMGS=\"%s\"", phone_number);
        if(sendATcommand(GLO_cmd_buff, ">", RESPONSE_TIMEOUT_MILLISECONDS)) {// gửi sms message
                snprintf(GLO_cmd_buff, sizeof(GLO_cmd_buff), "%s%c", sms_message, 0x1A); // Kết thúc tin nhắn bằng cách gửi mã ASCII \0x1a  SUB  substitute  ctrl-Z
                //Serial1.println(char(26));
                if (sendATcommand(GLO_cmd_buff, "OK", RESPONSE_TIMEOUT_MILLISECONDS))
                {
                        Serial.println("Sent OK ");
                }
                else
                {
                        Serial.println("Error ");
                }
                return 1;
        }
        else
        {
                Serial.println("FAILED");
                return 0;
        }
}
// 6. Hàm gọi điện tự động ngắt tới số
boolean voice_call(char* phone_number, unsigned int time_end){
        memset(GLO_cmd_buff, '\0', AT_CMD_MAX_LENGHT);
        sprintf(GLO_cmd_buff, "ATD%s;", phone_number);
        if(sendATcommand(GLO_cmd_buff, "OK", RESPONSE_TIMEOUT_MILLISECONDS)==0) {
                return 0;
        }

        delay(time_end);

        sendATcommand("ATH","OK",RESPONSE_TIMEOUT_MILLISECONDS);
        Serial.println("Call disconnected");

        return 1;
}
// 7. Hàm gửi dữ liệu lên Thingspeak
boolean get_data_single_mode(float sensor1, float sensor2){
        uint8_t answer = 0;
        // chế độ Single-connection mode
        if (sendATcommand2("AT+CIPMUX=0", "OK", "ERROR", 1000) == 1)
        {
                // chờ trạng thái thiết lập IP INITIAL
                while(sendATcommand("AT+CIPSTATUS", "INITIAL", 500)  == 0 ) ;
                delay(1000);

                snprintf(GLO_cmd_buff, sizeof(GLO_cmd_buff), "AT+CSTT=\"%s\"", APN);

                // Thiết lập APN, user name and password của nhà mạng
                if (sendATcommand2(GLO_cmd_buff, "OK",  "ERROR", 30000) == 1)
                {
                        // chờ trạng thái thiết lập IP START
                        while(sendATcommand("AT+CIPSTATUS", "START", 500)  == 0 ) ;
                        delay(1000);

                        // Bắt sóng 2.5G Wireless Connection
                        if (sendATcommand2("AT+CIICR", "OK", "ERROR", 30000) == 1)
                        {
                                // chờ trạng thái thiết lập IP GPRSACT
                                while(sendATcommand("AT+CIPSTATUS", "GPRSACT", 500)  == 0 ) ;
                                delay(1000);

                                // Lấy địa chỉ Local IP Address
                                if (sendATcommand2("AT+CIFSR", ".", "ERROR", 10000) == 1)
                                {
                                        // chờ trạng thái thiết lập IP STATUS
                                        while(sendATcommand("AT+CIPSTATUS", "IP STATUS", 500)  == 0 ) ;
                                        delay(1000);
                                        Serial.println("Opening TCP");

                                        snprintf(GLO_cmd_buff, sizeof(GLO_cmd_buff), "AT+CIPSTART=\"TCP\",\"%s\",\"%s\"", IP_SERVER, PORT_SOCKET);

                                        // Mở một TCP socket
                                        if (sendATcommand2(GLO_cmd_buff, "CONNECT OK", "CONNECT FAIL", 30000) == 1)
                                        {
                                                Serial.println("Connected");

                                                // Gửi data to the TCP socket
                                                if (sendATcommand2("AT+CIPSEND", ">", "ERROR", 5000) == 1) {
                                                        snprintf(GLO_cmd_buff, sizeof(GLO_cmd_buff), "GET %s%s%.2f%s%0.2f\n%c", URL_DATA, "&field1=", sensor1,"&field2=", sensor2, 0x1A);         // gửi 0x1A ký tự SUB sau ký tự xuống dòng
                                                        if (sendATcommand2(GLO_cmd_buff, "SEND OK", "ERROR", 10000)==1) {
                                                                answer = 1;
                                                        }
                                                }
                                                else {
                                                        Serial.println("Get failed");         //nếu không gửi được báo failed
                                                        sendATcommand("AT+CIPCLOSE", "OK", 5000);
                                                        sendATcommand("AT+CIPSHUT", "OK", 5000);
                                                        answer = 0;
                                                }
                                                // Đóng socket TCP
                                                sendATcommand2("AT+CIPCLOSE", "CLOSE OK", "ERROR", 10000);
                                        }
                                        else
                                        {
                                                Serial.println("Error opening the connection");
                                                answer = 0;
                                        }
                                }
                                else
                                {
                                        Serial.println("Error getting the IP address");
                                        answer = 0;
                                }
                        }
                        else
                        {
                                Serial.println("Error bring up wireless connection");
                                answer = 0;
                        }
                }
                else
                {
                        Serial.println("Error setting the APN");
                        answer = 0;
                }
        }
        else
        {
                Serial.println("Error setting the single connection");
                answer = 0;
        }

        sendATcommand2("AT+CIPSHUT", "OK", "ERROR", 10000);
        Serial.print(answer);
        delay(2000);
        return answer;
}
// 8. Hàm tính trung bình giá trị cảm biến Digital sensors
float average_humidity(unsigned int sample_number)
{
        float sum = 0.0;
        for (int8_t i = 0; i < sample_number; i++) {
                float temp = dht.readHumidity();
                sum += temp; // đọc giá trị cảm biến
                Serial.print("Sample");
                Serial.print(i);
                Serial.print(": ");
                Serial.println(temp);
                delay(2000); // Tốc độ lấy mẫu của cảm biến
        }
        return sum / sample_number;
}
