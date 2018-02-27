/*
* ----------------------------------------------------------------------------
* "THE BEER-WARE LICENSE" (Revision 42):
* <phk@FreeBSD.ORG> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a beer in return Poul-Henning Kamp
* ----------------------------------------------------------------------------
*/
/*
* DuinoClient
*  
* This sketch connects to a website using an Arduino Wiznet Ethernet shield
* and display content in a LCD display. 
* 
* Circuit:
*  Ethernet shield attached to pins 10, 11, 12, 13
*  LCD Matrix 16x2 attached to pins 
*/


#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal.h>

// your local ip address
IPAddress ip(10,1,1,8);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// server to client access
char server[] = "hewertho.mn";    // name address
char path[] = "/api/duino/index.php"; // path or file to read

// Initialize the Ethernet client library
EthernetClient client;

// Initialize the LCD Matrix
LiquidCrystal lcd(8,6,5,4,3,2);

// Variables for content 
char city[16];
char temp[11];
char time[5];

boolean hasStats = false; // until this is true default text will be displed

// frequency time to arduino to do requests
long updateFrequency = 5 * 60000; // 60000 =1 minute

void setup() {
  lcd.begin(16, 2);
  Ethernet.begin(mac, ip);
  delay(1000);
}

void loop() {
  updateInformation();
  
  if(hasStats) {
    printTemperature();
  } else {
    printDefaultText();
  }
  
  delay(updateFrequency);
}

/**
 *  Update the status
 */
void updateInformation() {
    if(client.connect(server, 80)) {
      sendRequest(client);
      lcd.println("Connected       ");
    } else {
      lcd.println("Connection failed");
      return;
    }
    
    extractData(client);
    client.stop();
}

/** 
* Send the request to server
*/
void sendRequest(EthernetClient client) {
  // Make a HTTP request:
  client.print("GET "); // path to access
  client.print(path); 
  client.println(" HTTP/1.1");

  client.print("Host: ");
  client.println(server);

  client.println("User-Agent: Arduino");
  client.println("Connection: close");
  client.println();
}

/**
 * Extract the data and set the variables
 */
 void extractData(EthernetClient client) {
   char currentValue[16];
   boolean dataFlag = false; // True if data has started
   boolean endFlag = false; // True if data is reached
   int j = 0;
   int i = 0;

    while(client.connected() && !endFlag) {
      char c = client.read();
      if(c == '<') {
        dataFlag = true;
        hasStats = true;
      }
      else if(dataFlag && c == '>') { // End of data
        setStatValue(j, currentValue);
        endFlag = true;
      }
      else if(dataFlag && c == '|') { // Next dataset
        setStatValue(j++, currentValue);
        char currentValue[7];
        i=0;
      }
      else if(dataFlag) { // Data
        currentValue[i++] = c;
      }
    }   
}
 
/**
* set a simple stat value depending on the position in the string returned
* @param integer position
* @param string value
*/
void setStatValue(int position, char value[]) {
    
  switch(position) {
    case 0:
      for(int i=0; i<16; i++) {
        city[i] = value[i];
      }
      break;
    case 1:
      for(int i=0; i<11; i++) {
        temp[i] = value[i];
      }
   break;
   case 2:
      for(int i=0; i<5; i++) {
        time[i] = value[i];
      }
    break;
  }
}
  
/*
* print the actual temperature
*/
void printTemperature() {
  printCity();
  printTemp();
  printTime();
}

/*
* print the city name
*/
void printCity() {
  lcd.clear();
  lcd.home();
  lcd.println(city);
}
/**
* print the temperature
*/
void printTemp() {
  lcd.setCursor(0,2);
  lcd.print(temp);
}

/*
* print the time
*/
void printTime() {
 lcd.setCursor(11,2);
 lcd.print(time); 
}

/**
* Print a default text
*/ 
void printDefaultText() {
  lcd.home();
  lcd.print("Temperature from");
  lcd.setCursor(0,1);
  lcd.print("hewertho.mn     ");
  delay(2000);
}