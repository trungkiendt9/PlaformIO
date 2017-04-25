/*
 Arduino --> ThingSpeak Channel via GSM Shield
 
 Please note: this sketch is based on the Arduino_to_Thingspeak
 sketch created by Hans Schaler:
 https://github.com/iobridge/ThingSpeak-Arduino-Examples/blob/master/Ethernet/Arduino_to_ThingSpeak.ino
 
 and on the example code which can be found from:
 http://www.arduino.org/learning/getting-started/getting-started-with-arduino-gsm-shield-2
 
 Getting Started with ThingSpeak:
 
   * Sign Up for New User Account - https://thingspeak.com/users/new
   * Create a new Channel by selecting Channels and then Create New Channel
   * Enter the Write API Key in this sketch under "ThingSpeak Settings"

 
 Created: October 17, 2011 by Hans Scharler (http://www.nothans.com)
 
 Additional Credits:
 Example sketches from Arduino team, Ethernet by Adrian McEwen
 
 Modified for GSM 2 Shield: Erkki RĂ¤sĂ¤nen, www.ecitec.fi
*/



// include the GSM library
#include <GSM.h>

// PIN number if necessary
#define PINNUMBER ""

// APN information obrained from your network provider
#define GPRS_APN "xxxx.xxx.xx" // replace with your GPRS APN
#define GPRS_LOGIN "" // replace with your GPRS login
#define GPRS_PASSWORD "" // replace with your GPRS password

// initialize the library instances
GSMClient client;
GPRS gprs;
GSM gsmAccess;


// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String writeAPIKey = "xxxxxxxxxxx"; //replace with your API Key
const int updateThingSpeakInterval = 16 * 1000;      // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)

// Variable Setup
long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;


String Incoming;   // for incoming serial data

void setup() {
  
   Serial.setTimeout(5);
   Serial.begin(57600);     // opens serial port, sets data rate to 9600 bps
   StartGSM();
        
        
}


String readData() {
  
      int value1 = analogRead(A0);
      
      int value2 = analogRead(A1);
      
      int value3 = analogRead(A3);
      
      String StringToThingSpeak = "field1=" + String(value1) + "&field2=" + String(value2) + "&field3=" + String(value3);

      return StringToThingSpeak;
}


void loop_()
{
  readData();
}

void loop()
{

  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // Disconnect from ThingSpeak
  if (!client.connected() && lastConnected)
  {
    Serial.println("...disconnected");
    Serial.println();
    
    client.stop();
  }
  
  // Update ThingSpeak
  if(!client.connected() && (millis() - lastConnectionTime > updateThingSpeakInterval))
  {
    String ToThingSpeak = readData();

    Serial.println(ToThingSpeak);
    
    updateThingSpeak(ToThingSpeak);
  }
  
  // Check if Arduino Ethernet needs to be restarted
  if (failedCounter > 3 ) {StartGSM();}
  
  lastConnected = client.connected();
}

void updateThingSpeak(String tsData)
{
  if (client.connect(thingSpeakAddress, 80))
  {         
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);
    
    lastConnectionTime = millis();
    
    if (client.connected())
    {
      Serial.println("Connecting to ThingSpeak...");
      Serial.println();
      
      failedCounter = 0;
    }
    else
    {
      failedCounter++;
  
      Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");   
      Serial.println();
    }
    
  }
  else
  {
    failedCounter++;
    
    Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    
    lastConnectionTime = millis(); 
  }
}

void StartGSM()
{
char server[] = "api.thingspeak.com"; // the base URL
int port = 80; // the port, 80 for HTTP

 Serial.println("Starting Arduino web client.");
 // connection state
 boolean notConnected = true;

 // Start GSM shield
 // pass the PIN of your SIM as a parameter of gsmAccess.begin()
 while(notConnected)
 {
 if((gsmAccess.begin(PINNUMBER)==GSM_READY) &
 (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY))
 notConnected = false;
 else
 {
 Serial.println("Not connected");
 delay(1000);
 }
 }

 Serial.println("connecting...");

 // if you get a connection, report back via serial:
 if (client.connect(server, port))
 {
 Serial.println("connected");
 // Make a HTTP request:
// client.print("GET ");
// client.print(path);
// client.println(" HTTP/1.0");
// client.println();
 } 
 else
 {
 // if you didn't get a connection to the server:
 Serial.println("connection failed");
 }

}

