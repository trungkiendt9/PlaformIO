/*
   Khai báo các hàm sử dụng.
   Khai báo các định nghĩa.
 */
// KHAI BÁO CÁC HÀM CHỨC NĂNG
#include <Arduino.h>
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout);
int8_t sendATcommand2(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned int timeout);
// void Start();
void power_on();
boolean sms_send(char* phone_number, char* sms_message);
boolean voice_call(char* phone_number, unsigned int time_end);
boolean connect_server();
boolean get_data(float* sensor1, float* sensor2);
boolean get_data_single_mode_1(float sensor3); // Gửi cảm biến mưa field3
boolean get_data_single_mode_2(float sensor1, float sensor2); // Gửi nhiệt độ và độ ẩm field1, field2
boolean get_data_single_mode_3(float sensor4, float sensor5,float sensor6); // Gửi tọa độ Lat, Long, Độ cao, field4,5,6
boolean get_GPS_location();
float average_humidity(unsigned int sample_number);
void software_Reset();

// KHAI BÁO CÁC ĐỊNH NGHĨA
const unsigned int AT_CMD_MAX_LENGHT = 200;  // Chiều dài câu lệnh
const unsigned int AT_RESPONSE_MAX_LENGHT = 512;  // Chiều dài ký tự phản hồi
const unsigned int POWER_KEY = 6;  // Chân khởi động module SIM kết nối với chân 6 của Arduino Due
const unsigned int AUTO_END_CALL_TIMEOUT = 10000; // Khoảng thời gian tự động kết thúc cuộc gọi: Test thử 2 hồi chuông
const unsigned long RESPONSE_TIMEOUT_MILLISECONDS = 10000;   // Thời gian chờ phản hồi tính bằng milliseconds
const char PIN[] = "";  // Mã pin của SIM nếu có
const char APN[] = "v-internet";  // APN của Viettel
char USER_PHONE_NUMBER[] = "0988004457";  // Chuỗi ký tự số điện thoại nhà quản lý
char REPORT_START[] = "Thiet bi quan trac bat dau hoat dong.";
const char IP_SERVER[] = "184.106.153.149";  // IP của thingspeak.com
const char URL_DATA[] = "/update?key=QZLN6Q1ZPQOOG93R";  // Kênh dữ liệu https://thingspeak.com/channels/51851
const char PORT_SOCKET[] = "80";   // Chân kích hoạt mở khởi động bộ truyền thông, xem sơ đồ nguyên lý của module
unsigned long interval = 900000; // CHU KỲ THỜI GIAN LẤY MẪU NHIỆT ĐỘ VÀ ĐỘ ẨM...
unsigned long interval_smoke = 1000; // CHU KỲ THỜI GIAN LẤY MẪU CHÁY
unsigned long interval_rain = 900000; // CHU KỲ THỜI GIAN LẤY MẪU MƯA
unsigned long interval_gps = 3600000; // CHU KỲ THỜI GIAN LẤY MẪU GPS
unsigned long previousMillis = 0; // millis() trả về số unsigned long. cho Đa nhiệm 1
unsigned long previousMillis1 = 0; // millis() trả về số unsigned long. cho Đa nhiệm 2
unsigned long previousMillis2 = 0; // millis() trả về số unsigned long. cho Đa nhiệm 3
unsigned long previousMillis3 = 0; // millis() trả về số unsigned long. cho Đa nhiệm 4
const unsigned int MQ2_pin = A0;
const unsigned int RainSen_pin = A1;
const unsigned int Rain_threshold = 700;
const unsigned int Rain_max_count = 2;
const unsigned int Smoke_max_count = 5;
// #define DHTPIN 53  //Pin data out của DHT22
// #define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321


// KHAI BÁO CÁC BIẾN TOÀN CỤC
char GLO_cmd_buff[AT_CMD_MAX_LENGHT];  // Chuỗi ký tự lưu câu lệnh
int8_t GLO_cmd_length = 0;  // Độ dài của câu lệnh hiện tại
int lpg, co, smoke; // Nồng độ các khí tính theo ppm
int rain_counter = 0; // Khẳ năng mưa
int smoke_counter = 0; // Khả năng cháy
float Latitude, Longtitude, Altitude; // Vĩ độ, kinh độ và độ cao
