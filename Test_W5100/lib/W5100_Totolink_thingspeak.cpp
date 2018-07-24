/*
   Repeating Web client

   This sketch connects to a a web server and makes a request
   using a Wiznet Ethernet shield. You can use the Arduino Ethernet shield, or
   the Adafruit Ethernet shield, either one will work, as long as it's got
   a Wiznet Ethernet module on board.

   This example uses DNS, by assigning the Ethernet client with a MAC address,
   IP address, and DNS address.

   Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

   created 19 Apr 2012
   by Tom Igoe
   modified 21 Jan 2014
   by Federico Vanzati

   http://www.arduino.cc/en/Tutorial/WebClientRepeating
   This code is in the public domain.

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
char ssid[] = "<YOURNETWORK>";         //  your network SSID (name)
char pass[] = "<YOURPASSWORD>";        // your network password
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
///////////////////////////////////////////////////////////////////

// #include <SPI.h>
// // #include <Ethernet.h>
// // #include "ThingSpeak.h"
// // #include <Stream.h>
// //
// // // assign a MAC address for the ethernet controller.
// // // fill in your address here:
// // byte mac[] = {
// //         0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
// // };
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192, 168, 68, 121);

// fill in your Domain Name Server address here:
//IPAddress myDns(1, 1, 1, 1);

// initialize the library instance:
//EthernetClient client;

char server[] = "192.168.68.1"; // Ip của Petrol
//IPAddress server(64,131,82,241);
/////Kênh Thingspeak////////////////
unsigned long myChannelNumber = 416101;   // Kênh trên Thingspeak
const char * myWriteAPIKey = "DBSNZ86I2E0DSYKH"; // Mã cho phép ghi trên kênh
unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 20L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers
//char MangHtml[3200];
void httpRequest();
void UpThingspeak();
float Tien;
long MilLit;
long DonGia;
int Dem;
//////////////////
void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
        asm volatile ("jmp 0");
}
///////////////////
void setup() {
        // start serial port:
        Serial.begin(9600);
        Ethernet.begin(mac);
        // print the Ethernet board/shield's IP address:

        if(Ethernet.begin(mac) == 0) { // start ethernet using mac & IP address
                Serial.println("Failed to configure Ethernet using DHCP");
                software_Reset();
                while(true) ; // no point in carrying on, so stay in endless loop:

        }
        Serial.print("My IP address: ");
        Serial.println(Ethernet.localIP());
        ThingSpeak.begin(client);
        delay(1000); // give the Ethernet shield a second to initialize
        Serial.println("connecting...");
        httpRequest();
}

void loop() {

        // if there's incoming data from the net connection.
        // send it out the serial port.  This is for debugging
        // purposes only:
        // char c = client.read();
        // //IndexOfChar++;
        // //Serial.print(IndexOfChar);
        // //Serial.print("........");
        // Serial.write(c);
        //Serial.println();

        // if ten seconds have passed since your last connection,
        // then connect again and send data:

        if(client.find("var wanTx=")) {
                //int Tien; // Kết quả mà trích xuất được
                if(client.find("\"")) {
                        //if(client.findUntil(" ")) {
                        Tien = client.parseInt(SKIP_WHITESPACE); //Đọc thư viện Stream.h để biết thêm chi tiết
                        Serial.print("Tiền: " );
                        Serial.print(Tien);
                        Serial.print(" VNĐ");
                        Dem++;
                        Serial.println();
                }
        }
        /////////////////////////////////////////////////////////////////////////////
        // if(client.find("<div><label>Lit:</label><td")) {
        //         //if(client.find("<div><label>Tien:</label><td>       ")) {
        //         //int Lit; // Kết quả mà trích xuất được
        //         if(client.find(">")) {
        //                 MilLit = client.parseInt(SKIP_WHITESPACE);
        //                 Serial.print("Mili Lít: " );
        //                 Serial.print(MilLit);
        //                 Serial.print(" ml");
        //                 Serial.println();
        //         }
        // }
        /////////////////////////////////////////////////////////////////////////////
        // if(client.find("<div><label>Don gia:</label><td")) {
        //         //if(client.find("<div><label>Tien:</label><td>       ")) {
        //         //int Lit; // Kết quả mà trích xuất được
        //         if(client.find(">")) {
        //                 DonGia = client.parseInt(SKIP_WHITESPACE);
        //                 Serial.print("Đơn giá: " );
        //                 Serial.print(DonGia);
        //                 Serial.print(" VNĐ");
        //                 Serial.println();
        //                 Dem++;
        //                 Serial.print("Lần: ");
        //                 Serial.println(Dem);
        //         }
        // }
        /////UP THINGSPEAK//////////////////////////////////////////////
        if(Dem == 1) {
                client.stop();
                Serial.println(Dem);
                ThingSpeak.setField(1,(float) Tien);
                // ThingSpeak.setField(2,(long) MilLit);
                // ThingSpeak.setField(3,(long) DonGia);
                ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
                Serial.println("Updated to thingspeak" );
                Dem = 0;
        }
        //UpThingspeak();
        ////////////////////////////////////Delay//////////////////////////////////////
        if (millis() - lastConnectionTime > postingInterval) {
                Dem = 0;
                httpRequest();

        }

}

// this method makes a HTTP connection to the server:
void httpRequest() {
        // close any connection before send a new request.
        // This will free the socket on the WiFi shield
        client.stop();

        // if there's a successful connection:
        if (client.connect(server, 80)) {
                Serial.println("connecting...");
                // send the HTTP GET request:
                client.println("GET /stats.htm HTTP/1.1"); //http://192.168.1.10/status.htm HTTP/1.1 206 Partial Content
                client.println("Authorization: Basic YWRtaW46YWRtaW4=");
                client.println("Host: 192.168.68.1");
                //client.println("Range: bytes=5100-6100");
                client.println("User-Agent: arduino-ethernet");
                client.println("Connection: close");
                client.println();

                // note the time that the connection was made:
                lastConnectionTime = millis();
        } else {
                // if you couldn't make a connection:
                Serial.println("connection failed");
                //software_Reset();
        }
}
/////Gửi dữ liệu lên kênh Thingspeak//////////////
// void UpThingspeak(){
//         if(Dem == 2) {
//                 Serial.println(Dem);
//                 ThingSpeak.setField(1,(long) Tien);
//                 ThingSpeak.setField(2,(long) MilLit);
//                 ThingSpeak.setField(3,(long) DonGia);
//                 ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
//                 Serial.println("Updated to thingspeak" );
//                 Dem = 0;
//         }
// }
