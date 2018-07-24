/*
   Simple Client Parsing sketch
   Arduino 1.0 version
 */
#include "ThingSpeak.h"
#include <SPI.h>
#include <Ethernet.h>
#include <Stream.h>

///////////////////////KHAI BÁO CÁC ĐỊNH NGHĨA////////////////////////////////
#define USE_ETHERNET_SHIELD
#if !defined(USE_WIFI101_SHIELD) && !defined(USE_ETHERNET_SHIELD) && !defined(ARDUINO_SAMD_MKR1000) && !defined(ARDUINO_AVR_YUN) && !defined(ARDUINO_ARCH_ESP8266)
  #error "Uncomment the #define for either USE_WIFI101_SHIELD or USE_ETHERNET_SHIELD"
#endif

#if defined(ARDUINO_AVR_YUN)
    #include "YunClient.h"
YunClient client;
#else
  #if defined(USE_WIFI101_SHIELD) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_ARCH_ESP8266)
// Use WiFi
    #ifdef ARDUINO_ARCH_ESP8266
      #include <ESP8266WiFi.h>
    #else
      #include <SPI.h>
      #include <WiFi101.h>
    #endif
char ssid[] = "<YOURNETWORK>";        //  your network SSID (name)
char pass[] = "<YOURPASSWORD>";       // your network password
int status = WL_IDLE_STATUS;
WiFiClient client;
  #elif defined(USE_ETHERNET_SHIELD)
// Use wired ethernet shield
    #include <SPI.h>
    #include <Ethernet.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetClient client;
  #endif
#endif
////////////////////////// KHAI BÁO CÁC BIẾN TOÀN CỤC/////////////////////////
//byte ip[] = { 192,168,0,112 }; // Địa chỉ IP nếu không nhận được từ DCHP
//byte ip[] = { 192,168,1,112 }; // Địa chỉ IP nếu không nhận được từ DCHP
byte ip[] = {192,168,68,121}; // Địa chỉ IP nếu không nhận được từ DCHP VKTECH
char serverName[] = "192.168.68.120"; // Địa chỉ IP của Petrol
// char serverName[] = "192.168.1.10";
unsigned long myChannelNumber = 416101;   // Kênh trên Thingspeak
const char * myWriteAPIKey = "DBSNZ86I2E0DSYKH"; // Mã cho phép ghi trên kênh
unsigned long interval = 30000; // CHU KỲ THỜI GIAN LẤY MẪU ĐỌC CÂY XĂNG
unsigned long previousMillis = 0; // millis() trả về số unsigned long. cho Đa nhiệm 1
long Tien;
long MilLit;
long DonGia;
//////////////////////////// CÁC HÀM KHỞI TẠO/////////////////////////////////
void TruyXuatServer();
void TrichTien();
void TrichLit();
void TrichDonGia();
// void TrichDanTri();
void setup()
{
        Serial.begin(9600);
        Ethernet.begin(mac);
        // print the Ethernet board/shield's IP address:

        if(Ethernet.begin(mac) == 0) { // start ethernet using mac & IP address
                Serial.println("Failed to configure Ethernet using DHCP");
                while(true) ; // no point in carrying on, so stay in endless loop:

        }
        Serial.print("My IP address: ");
        Serial.println(Ethernet.localIP());
        ThingSpeak.begin(client);
        delay(1000); // give the Ethernet shield a second to initialize
        Serial.println("connecting...");
}
void loop()
{
        unsigned long currentMillis = millis(); // grab current time
//////////////////// LẤY THÔNG TIN TỪ WEBSERVER///////////////////////////////
// Đa nhiệm thứ 1
        if ((unsigned long)(currentMillis - previousMillis) >= interval) {
                delay(100);
                TruyXuatServer();
                delay(400);
//////////////////////////////Trích xuất thông tin////////////////////////////
                if (client.connected()) {
                        TrichTien();
                        TrichLit();
                        TrichDonGia();
                        //TrichDanTri();
                        //client.stop();
                        // delay(5000); // check again in 10 seconds
                }
                else {
                        Serial.println();
                        Serial.print(".");
                        client.stop();
                        delay(1000);
                }
//////////////////////////////Gửi data lên Cloud/////////////////////////////
                // ThingSpeak.setField(1,(long) Tien);
                // ThingSpeak.setField(2,200);
                // ThingSpeak.setField(3,(long) DonGia);
                // ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
                ThingSpeak.writeField(myChannelNumber, 1, 400, myWriteAPIKey);
                delay(20000);
                Serial.println("Sent update to thingspeak" );

                previousMillis = millis();
        }

/////////////////////////////// TRÍCH XUẤT THÔNG TIN TIỀN //////////////////////////

}
///////////////////////////Triển khai các hàm//////////////////////////////////
//1.Hàm truy xuất server
void TruyXuatServer()
{
        client.stop();
        if (client.connect(serverName, 80)>0) {
                Serial.print("connected... ");
                // client.println("GET /suc-manh-so.htm HTTP/1.1"); //http://192.168.1.1/status.htm HTTP/1.1 206 Partial Content
                client.println("GET /index.htm HTTP/1.1"); //http://192.168.1.1/status.htm HTTP/1.1 206 Partial Content
                client.println("Authorization: Basic YWRtaW46YWRtaW4=");
                //client.println("Authorization: Bearer d815648beb04c5cac074c3c22085c23cffde59e6");
                // client.println("Host: dantri.com.vn");
                client.println("Host: 192.168.68.120");
                //client.println("Range: bytes=0-1024");
                client.println("User-Agent: arduino-ethernet");
                client.println("Connection: close");
                client.println();
        } else {
                Serial.println("connection failed");
        }
}
//2. Hàm trích xuất thông tin Tiền
void TrichTien()
{
        //if(client.find("ProductName = \"Internet Box")) {
        if(client.find("<div><label>Tien:</label><td")) {
                //int Tien; // Kết quả mà trích xuất được
                if(client.find(">")) {
                        //if(client.findUntil(" ")) {
                        Tien = client.parseInt(SKIP_WHITESPACE,'\x01'); //Đọc thư viện Stream.h để biết thêm chi tiết
                        Serial.print("Tiền:" );
                        Serial.print(Tien);
                        Serial.print(" VNĐ");
                        Serial.println();
                }
        }
        else {
                Serial.println("Tiền not found");
        }
}
//3. Hàm trích xuất thông tin số Lít
void TrichLit()
{
        if(client.find("<div><label>Lit:</label><td")) {
                //if(client.find("<div><label>Tien:</label><td>       ")) {
                //int Lit; // Kết quả mà trích xuất được
                if(client.find(">")) {
                        MilLit = client.parseInt(SKIP_WHITESPACE,'\x01');
                        Serial.print("Mili Lít:" );
                        Serial.print(MilLit);
                        Serial.print(" ml");
                        Serial.println();
                }
        }
        else {
                Serial.println("Mili Lít not found");
        }
}
//4. Hàm trích xuất thông tin Đơn giá
void TrichDonGia()
{
        if(client.find("<div><label>Don gia:</label><td")) {
                //if(client.find("<div><label>Tien:</label><td>       ")) {
                //int Lit; // Kết quả mà trích xuất được
                if(client.find(">")) {
                        DonGia = client.parseInt(SKIP_WHITESPACE,'\x01');
                        Serial.print("Đơn giá:" );
                        Serial.print(DonGia);
                        Serial.print(" VNĐ");
                        Serial.println();
                }
        }
        else {
                Serial.println("Đơn giá not found");
        }
}
//5. Hàm trích xuất Test từ server Dantri.com.vn
// void TrichDanTri()
// {
//         if(client.find("value=\"2/2/201")) {
//                 //if(client.find("<div><label>Tien:</label><td>       ")) {
//                 //int Lit; // Kết quả mà trích xuất được
//                 if(client.find("8")) {
//                         DonGia = client.parseInt(SKIP_WHITESPACE,'\x01');
//                         Serial.print("Test Dân Trí:" );
//                         Serial.print(DonGia);
//                         Serial.print(" được tìm thấy");
//                         Serial.println();
//                 }
//         }
//         else {
//                 Serial.println("Dân Trí not found");
//         }
// }
