#include <SoftwareSerial.h>
#include<stdio.h>
#include<string.h>
#define DEBUG true
int en=9;

void call(void);
void getgps(void);
String sendData(String command, const int timeout, boolean debug);
SoftwareSerial mySerial(7,8); 
        
void setup()
{
   Serial.begin(9600);
   mySerial.begin(9600); 
  digitalWrite(en, HIGH);
  delay(3000);
  digitalWrite(en, LOW);
  delay(5000);
  call();
  pinMode(en,OUTPUT);
}
int i=0;
void loop()
{
     getgps();
   while(1)
   {
      sendData( "AT+CGNSINF",1000,DEBUG);
       delay(1000);
   } 
}

void call(void)
   {
         sendData( "AT+CSQ",1000,DEBUG);
         sendData("ATD10086;",1000,DEBUG);
         delay(15000);
         sendData( "ATH",1000,DEBUG);
   }
void getgps(void)
{
   sendData( "AT+CGNSPWR=1",1000,DEBUG); 
   sendData( "AT+CGNSSEQ=RMC",1000,DEBUG); 
}


String sendData(String command, const int timeout, boolean debug)
{
    String response = "";    
    mySerial.println(command); 
    long int time = millis();   
    while( (time+timeout) > millis())
    {
      while(mySerial.available())
      {       
        char c = mySerial.read(); 
        response+=c;
      }  
    }    
    if(debug)
    {
      Serial.print(response);
    }    
    return response;
}


