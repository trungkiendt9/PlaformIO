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
 * AM2315 vs DUE. dùng điện trở treo 10k
 * Connect RED of the AM2315 sensor to 3.3V
 * Connect BLACK to Ground
 * Connect WHITE to i2c clock - SCL to SCL pin 21 - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 5
 * Connect YELLOW to i2c data - SDA to SDA pin 20 - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 4
 *  Cài đặt thư viện cho platform atmelsam, framework =arduino
 *  Chuyển mã PDU cho các ký tự UCS2 - 16 bit : http://www.multisilicon.com/blog/a22201774~/pdu.htm
 */
#include <Arduino.h>
#include "QuanTrac.h"
// #include "DHT.h"
#include <Wire.h>   // đã sửa thư viện để sử dụng SCL1 và SDA1 Wire1.begin();
#include "Adafruit_AM2315.h"
#include "MQ2.h"
#include "TinyGPS++.h"



// CÁC HÀM KHỞI TẠO
// DHT dht(DHTPIN, DHTTYPE);
//  void (* resetFunc)(void) = 0;
Adafruit_AM2315 am2315;
MQ2 mq2(MQ2_pin);
TinyGPSPlus gps;
// HÀM KHỞI CHẠY MỘT LẦN
void setup() {
        delay(5000);
        //  watchdogEnable(16000);
        am2315.begin();
        mq2.begin();
        pinMode(POWER_KEY, OUTPUT);
        // khởi tạo thành chuỗi cmd_buff
        memset(GLO_cmd_buff, '\0', AT_CMD_MAX_LENGHT);
        // port mặc định giữa máy tính với RX và TX của DUE
        Serial.begin(9600);
        // port RX3 và TX3 của DUE giao tiếp với SIMCOM
        Serial3.begin(9600);
        // khởi động
        power_on();
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
        delay(1000);
        get_GPS_location();
        delay(4000);
        voice_call(USER_PHONE_NUMBER, AUTO_END_CALL_TIMEOUT);
        //get_data_single_mode_2(t, h);
        Serial.println("AM2315 run");

        if (!am2315.begin()) {
                Serial.println("Sensor not found, check wiring & pullups!");
                return;
        }
}


// VÒNG LẶP VÔ HẠN
void loop() {
        unsigned long currentMillis = millis(); // grab current time
        // Đa nhiệm thứ 1
        if ((unsigned long)(currentMillis - previousMillis) >= interval) {
                // Xử lý dữ liệu của AM2315
                delay(300);
                float t = am2315.readTemperature();
                delay(300);
                float h = am2315.readHumidity();
                Serial.println();
                Serial.println(h);
                delay(300);
                Serial.println(t);
                if (isnan(h) || isnan(t)) {
                        Serial.println("Failed to read from AM2315 sensor!");
                        am2315.begin();
                        return;
                }
                get_data_single_mode_2(t, h);
                previousMillis = millis();
        }

        // Đa nhiệm thứ 2
        if ((unsigned long)(currentMillis - previousMillis1) >= interval_smoke) {
                // Xử lý MQ2
                //float* values= mq2.read(true); //True để in ra màn hình các giá trị
                float* values= mq2.read(true); //True để in ra màn hình các giá trị
                //lpg = values[0];
                //lpg = mq2.readLPG();           // Đọc giá trị LPG
                //co = values[1];
                co = mq2.readCO();       // Đọc giá trị CO
                //smoke = values[2];
                smoke = mq2.readSmoke();  // Đọc giá trị Smoke

                if ((co > 100 || smoke > 100 )) {
                        Serial.println("Phát hiện khả năng cháy"); // while raining at threshold level - see text
                        smoke_counter++;
                        Serial.println(co);
                        Serial.println(smoke);
                        Serial.println(smoke_counter);           // increment sensing counter
                }

                if (smoke_counter > Smoke_max_count) {                          // end of the time delay
                        // sms_send(USER_PHONE_NUMBER, "Canh bao phat hien kha nang co chay!");
                        char buffer[480];
                        //sprintf(buffer, "Canh bao phat hien kha nang co chay!. Toa do %6f", Latitude);
                        snprintf(buffer, sizeof(buffer), "Canh bao phat hien kha nang co chay!. %s%.6f,%.6f.","https://www.google.com/maps/?q=", Latitude, Longtitude);
                        sms_send(USER_PHONE_NUMBER, buffer);
                        voice_call(USER_PHONE_NUMBER, 30000);
                        voice_call(USER_PHONE_NUMBER, 30000);
                        voice_call(USER_PHONE_NUMBER, 30000);
                }

                if (co < 100 && smoke < 100 ) {
                        smoke_counter = 0;
                }
                previousMillis1 = millis();
        }
        // Đa nhiệm thứ 3
        if ((unsigned long)(currentMillis - previousMillis2) >= interval_rain) {
                // Xử lý dữ liệu của cảm biến mưa
                int rainSenseReading = analogRead(RainSen_pin);
                //Serial.println(rainSenseReading);     // use this for serial monitoring if available
                delay(250);                                            // relax
                // check to see how long it is raining at the threshold level,
                // rain strength value from 0 - 1023,
                // full rain -to- no rain.
                if (rain_counter >= Rain_max_count) {                          // end of the time delay
                        Serial.println("Phát hiện mưa");
                        get_data_single_mode_1((1023.0 - rainSenseReading)/100.0);
                }
                // If raining is continuing for x amount of time,
                // raise an alert .
                // When raining is no longer detected, reset the counter.
                if (rainSenseReading <Rain_threshold) {          // while raining at threshold level - see text
                        rain_counter++;                        // increment sensing counter
                }
                else if (rainSenseReading >Rain_threshold) {    // if not raining at threshold level
                        Serial.println("Không mưa");
                        rain_counter = 0;                        // reset the counter to 0
                        get_data_single_mode_1(0.0);
                }
                //get_data_single_mode_2(t, h);
                previousMillis2 = millis();
        }

        // Đa nhiệm thứ 4
        if ((unsigned long)(currentMillis - previousMillis3) >= interval_gps) {
                // Lấy tọa độ
                get_GPS_location();
                // Gửi tọa độ lên server
                get_data_single_mode_3(Latitude, Longtitude, Altitude);
                previousMillis3 = millis();
        }
        //  watchdogReset();
}



// TRIỂN KHAI CÁC HÀM
//1. Hàm setup chạy 1 lần
// void Start(){
// // Khởi động
//         power_on();
//         delay(5000);
// }

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
                delay(3000); // Đợi cho module khởi động hoàn tất

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
boolean get_data_single_mode_1(float sensor3){
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
                                                        snprintf(GLO_cmd_buff, sizeof(GLO_cmd_buff), "GET %s%s%.2f\n%c", URL_DATA, "&field3=", sensor3, 0x1A);         // gửi 0x1A ký tự SUB sau ký tự xuống dòng
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
        sendATcommand("AT+CIPCLOSE", "OK", 5000);
        sendATcommand2("AT+CIPSHUT", "OK", "ERROR", 10000);
        Serial.print(answer);
        //delay(2000);
        return answer;
}
//////////////////////////////////////////////////////////////
boolean get_data_single_mode_2(float sensor1, float sensor2){
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
        sendATcommand("AT+CIPCLOSE", "OK", 5000);
        sendATcommand2("AT+CIPSHUT", "OK", "ERROR", 10000);
        Serial.print(answer);
        //delay(2000);
        return answer;
}
boolean get_data_single_mode_3(float sensor4, float sensor5, float sensor6){
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
                                                        snprintf(GLO_cmd_buff, sizeof(GLO_cmd_buff), "GET %s%s%.6f%s%0.6f%s%0.2f\n%c", URL_DATA, "&field4=", sensor4,"&field5=", sensor5,"&field6=", sensor6, 0x1A);         // gửi 0x1A ký tự SUB sau ký tự xuống dòng
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
        sendATcommand("AT+CIPCLOSE", "OK", 5000);
        sendATcommand2("AT+CIPSHUT", "OK", "ERROR", 10000);
        Serial.print(answer);
        //delay(2000);
        return answer;
}

// 8. Hàm tính trung bình giá trị cảm biến Digital sensors
// float average_humidity(unsigned int sample_number)
// {
//         float sum = 0.0;
//         for (int8_t i = 0; i < sample_number; i++) {
//                 float temp = dht.readHumidity();
//                 sum += temp; // đọc giá trị cảm biến
//                 Serial.print("Sample");
//                 Serial.print(i);
//                 Serial.print(": ");
//                 Serial.println(temp);
//                 delay(2000); // Tốc độ lấy mẫu của cảm biến
//         }
//         return sum / sample_number;
// }
//9. Hàm lấy dữ liệu tọa độ GPS
boolean get_GPS_location(){
        sendATcommand("AT+CGNSPWR = 1", "OK", RESPONSE_TIMEOUT_MILLISECONDS);         // Khởi động chạy GPS
        sendATcommand(" AT+CGNSTST = 1", "OK", RESPONSE_TIMEOUT_MILLISECONDS);         // Bật chế độ NEMA
        Serial.println("Get GPS location");
        Serial.println("Edit by Trungkiendt9");
        int count = 0;
        while (count <= 100) {
                while (Serial3.available() > 0)
                        gps.encode(Serial3.read());

                // Let's display the new location and altitude
                // whenever either of them have been updated.
                if (gps.location.isUpdated() || gps.altitude.isUpdated())
                {
                        Serial.print("Location: ");
                        Latitude = gps.location.lat();
                        Serial.print(Latitude, 6);
                        Serial.print(",");
                        Longtitude = gps.location.lng();
                        Serial.print(Longtitude, 6);
                        Serial.print("  Altitude: ");
                        Altitude = gps.altitude.meters();
                        Serial.println(Altitude, 6);
                        count++;
                }
                //delay(2000);
        }
        sendATcommand("AT+CGNSPWR = 0", "OK", RESPONSE_TIMEOUT_MILLISECONDS); // Tắt chế độ GPS
        sendATcommand(" AT+CGNSTST = 0", "OK", RESPONSE_TIMEOUT_MILLISECONDS); // Tắt chế độ NEMA
        delay(1000);
        get_data_single_mode_3(Latitude, Longtitude, Altitude);
}
