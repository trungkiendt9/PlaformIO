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

#include <SPI.h>
#include <Ethernet.h>

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = {
        0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192, 168, 1, 121);

// fill in your Domain Name Server address here:
//IPAddress myDns(1, 1, 1, 1);

// initialize the library instance:
EthernetClient client;

char server[] = "192.168.68.120"; // Ip của Petrol
//IPAddress server(64,131,82,241);
unsigned long IndexOfChar;
unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers
//char MangHtml[3200];
void httpRequest();
long Tien;
long MilLit;
long DonGia;
int Dem;

void setup() {
        // start serial port:
        Serial.begin(4800);
        while (!Serial) {
                ; // wait for serial port to connect. Needed for native USB port only
        }

        // give the ethernet module time to boot up:
        delay(1000);
        // start the Ethernet connection using a fixed IP address and DNS server:
        Ethernet.begin(mac);
        // print the Ethernet board/shield's IP address:
        Serial.print("My IP address: ");
        Serial.println(Ethernet.localIP());
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
        }
        /////////////////////////////////////////////////////////////////////////////
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
        /////////////////////////////////////////////////////////////////////////////
        if(client.find("<div><label>Don gia:</label><td>")) {
                //if(client.find("<div><label>Tien:</label><td>       ")) {
                //int Lit; // Kết quả mà trích xuất được
                if(client.find(" ")) {
                        DonGia = client.parseInt(SKIP_WHITESPACE);
                        Serial.print("Đơn giá: " );
                        Serial.print(DonGia);
                        Serial.print(" VNĐ");
                        Serial.println();
                        Dem++;
                        Serial.print("Lần: ");
                        Serial.println(Dem);
                }
        }

        ///////////////////////////////////////////////////////////////////////////////
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
                client.println("GET /index.htm HTTP/1.1"); //http://192.168.1.10/status.htm HTTP/1.1 206 Partial Content
                client.println("Authorization: Basic YWRtaW46YWRtaW4=");
                client.println("Host: 192.168.68.120");
                //client.println("Range: bytes=5100-6100");
                client.println("User-Agent: arduino-ethernet");
                client.println("Connection: close");
                client.println();

                // note the time that the connection was made:
                lastConnectionTime = millis();
        } else {
                // if you couldn't make a connection:
                Serial.println("connection failed");
        }
}
