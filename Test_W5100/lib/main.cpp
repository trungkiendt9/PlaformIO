/*
   Simple Client Parsing sketch
   Arduino 1.0 version
 */
#include <SPI.h>
#include <Ethernet.h>
#include "ThingSpeak.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192,168,1,11 };
char serverName[] = "192.168.1.12";
EthernetClient client;
unsigned long myChannelNumber = 416101;   // Kênh trên Thingspeak
const char * myWriteAPIKey = "DBSNZ86I2E0DSYKH"; // Mã cho phép ghi trên kênh
int result; // the result of the calculation
int result2; // the result of the calculation
const unsigned long SAMPLE_TIME = 120000;

long Tien;
long MilLit;
long DonGia;

void setup()
{
        Serial.begin(4800);
        Ethernet.begin(mac,ip);
        // print the Ethernet board/shield's IP address:

        if(Ethernet.begin(mac) == 0) { // start ethernet using mac & IP address
                Serial.println("Failed to configure Ethernet using DHCP");
                while(true) // no point in carrying on, so stay in endless loop:
                        ;
        }
        Serial.print("My IP address: ");
        Serial.println(Ethernet.localIP());
        ThingSpeak.begin(client);
        delay(1000); // give the Ethernet shield a second to initialize
        Serial.println("connecting...");
}
void loop()
{
/////////////////////Kết nối tới server 1//////////////////////////////////////
        if (client.connect(serverName, 80)>0) {
                Serial.print("connected... ");
                // client.println("GET /suc-manh-so.htm HTTP/1.1"); //http://192.168.1.1/status.htm HTTP/1.1 206 Partial Content
                //client.println("GET /index.htm HTTP/1.1"); //http://192.168.1.1/status.htm HTTP/1.1 206 Partial Content
                client.println("GET /index.htm HTTP/1.1 206 Partial Content");
                client.println("Authorization: Basic YWRtaW46YWRtaW4=");
                //client.println("Authorization: Bearer d815648beb04c5cac074c3c22085c23cffde59e6");
                client.println("Host: 192.168.1.12");
                client.println("Range: bytes=5100-6100");
                client.println("User-Agent: arduino-ethernet");
                client.println("Connection: close");
                client.println();
        } else {
                Serial.println("connection failed");
        }
///////////////////////////////TRÍCH XUẤT ////////////////////////float: left; width:
        if (client.connected()) {
                /////////////////////////////////////Trích 1//////////////////////
                if(client.find("<div><label>Tien:</label><td>")) {
                        //int Tien; // Kết quả mà trích xuất được
                        if(client.find(" ")) {
                                //if(client.findUntil(" ")) {
                                Tien = client.parseInt(SKIP_WHITESPACE); //Đọc thư viện Stream.h để biết thêm chi tiết
                                Serial.print("Tiền: " );
                                Serial.print(Tien);
                                Serial.print(" VNĐ");
                                Serial.println();
                        }
                        else
                                Serial.println("Tiền not found");
                        //////////////////////////////////////Trích 2///////////////////
                        if(client.find("<div><label>Lit:</label><td>")) {
                                //if(client.find("<div><label>Tien:</label><td>       ")) {
                                //int Lit; // Kết quả mà trích xuất được
                                if(client.find(" ")) {
                                        MilLit = client.parseInt(SKIP_WHITESPACE);
                                        Serial.print("Mili Lít: " );
                                        Serial.print(MilLit);
                                        Serial.print(" ml");
                                        Serial.println();
                                }
                        }
                        else
                                Serial.println("Mili Lít not found");
                        //////////////////////////////////////Trích 3///////////////////
                        if(client.find("<div><label>Don gia:</label><td>")) {
                                //if(client.find("<div><label>Tien:</label><td>       ")) {
                                //int Lit; // Kết quả mà trích xuất được
                                if(client.find(" ")) {
                                        DonGia = client.parseInt(SKIP_WHITESPACE);
                                        Serial.print("Đơn giá: " );
                                        Serial.print(DonGia);
                                        Serial.print(" VNĐ");
                                        Serial.println();
                                }
                        }
                        else {
                                Serial.println("Đơn giá not found");
                        }
                        ////////////////////////////////////////////////////////////////
                        client.stop();// Dừng truy xuất vào serverName
                        delay(200);
                        // ////Gửi lên Thingspeak///
                        // ThingSpeak.writeField(myChannelNumber, 1, result, myWriteAPIKey);
                        // ////Gửi lên Thingspeak///
                        // delay(200);
                        // ThingSpeak.writeField(myChannelNumber, 2, result2, myWriteAPIKey);
                        ThingSpeak.setField(1,(long) Tien);
                        ThingSpeak.setField(2,(long) MilLit);
                        ThingSpeak.setField(3,(long) DonGia);
                        ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
                        Serial.println("Updated to thingspeak" );

                        delay(SAMPLE_TIME); // check again in 10 seconds
                }
                else {
                        Serial.println();
                        Serial.println("not connected");
                        client.stop();
                        delay(200);
                }
//////////////////////////////////////////////////////////////////////////////
        }
}
