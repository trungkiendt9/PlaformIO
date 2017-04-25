//Please use Arduino 1.0 or later, Arduino 1.0.4 is strong recommended.use this sketch instead: 
//Arduino will patch a serial link between the computer and the GPRS Shield at 9600bps 8-N-1
//Computer is connected to Hardware UART and SIM908 Shield is connected to the Software UART D2/D3
//Nokia 5110 LCD is connected to D9,D10,D11,D12,D13.
// SoftwareSerial.h is got form Arduino 1.0.4    LCD5110_Graph.h is got in the document.

#include <LCD5110_Graph.h>
#include <SoftwareSerial.h>
 
extern uint8_t SmallFont[];
 
#define rxPin 2
#define txPin 3

SoftwareSerial mySerial(rxPin, txPin);

LCD5110 myGLCD(9,10,11,12,13);
 
extern uint8_t SmallFont[];
 
 
void setup()
{
  myGLCD.InitLCD();   //Initial LCD
  myGLCD.setFont(SmallFont); //set the Font
  mySerial.begin(9600);               // the GPRS baud rate
  Serial.begin(9600);                 // the GPRS baud rate 

 
  myGLCD.clrScr();  // clear the LCD screen
  myGLCD.print(" ANDTech", CENTER, 0);
  myGLCD.print("AND-GLOBAL.COM", LEFT, 15);
  myGLCD.print("SIM908 Beta V1", LEFT, 30);
  myGLCD.update();
  pinMode(11, OUTPUT);
  digitalWrite(11,LOW);
}

void loop()
{
   if (mySerial.available())
    Serial.write(mySerial.read());  // Arduino send the computer command to SIM900
   
   if (Serial.available())
    mySerial.write(Serial.read());  //Arduino send the sim900 feedback to computer
}
