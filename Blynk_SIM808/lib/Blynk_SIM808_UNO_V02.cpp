
#define BLYNK_PRINT Serial

// Select your modem:
#define TINY_GSM_MODEM_SIM800
//#define TINY_GSM_MODEM_SIM900
//#define TINY_GSM_MODEM_M590
//#define TINY_GSM_MODEM_A6
void software_Reset();


//#define BLYNK_HEARTBEAT 30

#include <TinyGsmClient.h>
#include <BlynkSimpleSIM800.h>
#define TRUE          1
#define FALSE         0

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "26916e9f5716461fabcb4fc1c92def26";
char server[]          = "blynk-cloud.com";
unsigned int port      = 8442;
// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "v-internet";
char user[] = "";
char pass[] = "";
// Khai báo các biến toàn cục
bool Bit_lock = FALSE;
// Hardware Serial on Mega, Leonardo, Micro
// #define SerialAT Serial1
// Chân chức năng trên arduino
#define STOP_PIN 6
#define UP_PIN 7
#define DOWN_PIN  8
#define LED_BLINK  13


BlynkTimer timer;

// or Software Serial on Uno, Nano
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(9, 10); // RX, TX chân PWM

TinyGsm modem(SerialAT);

// Hàm đọc giá trị Virtual PIN V0 viết giá trị lên LED_BLINK
BLYNK_WRITE(V0) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        Serial.println(pinValue);
        if (Bit_lock == 0) {
                digitalWrite(STOP_PIN, pinValue);
        }
        // process received value
}
BLYNK_WRITE(V1) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        Serial.println(pinValue);
        if (Bit_lock == 0) {
                digitalWrite(UP_PIN, pinValue);
        }
        // process received value
}
BLYNK_WRITE(V2) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        Serial.println(pinValue);
        if (Bit_lock == 0) {
                digitalWrite(DOWN_PIN, pinValue);
        }

        // process received value
}
BLYNK_WRITE(V3) //Send data from app to hardware
{
        bool pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        Serial.println(pinValue);
        if (pinValue&& Bit_lock==TRUE) {
                Bit_lock = FALSE; // đổi trạng thái
                Blynk.virtualWrite(V10, "Không khóa");
                //digitalWrite(STOP_PIN, 0);
                Serial.print("Bit lock:");
                Serial.println(Bit_lock);
        } else if (pinValue&& Bit_lock==FALSE) {
                Bit_lock = TRUE;
                Blynk.virtualWrite(V10, "Khóa");
                //digitalWrite(STOP_PIN, 1);
                Serial.print("Bit lock:");
                Serial.println(Bit_lock);
        }



        // digitalWrite(LOCK_PIN, pinValue);
        // process received value
}
void CheckConnection() {   // check every 11s if connected to Blynk server
        if (!Blynk.connected()) {
                Serial.println("Khong ket noi toi Blynk server");
                //Blynk.connectNetwork(apn, user, pass); // try to connect to server with default timeout
                //Blynk.connect(10000);
                software_Reset();
                //Blynk.begin(auth, modem, apn, user, pass);
        }
        else {
                Serial.println("Da ket noi Blynk server");
        }
}

void setup()
{
        // Debug console
        pinMode(STOP_PIN, OUTPUT);
        pinMode(UP_PIN, OUTPUT);
        pinMode(DOWN_PIN, OUTPUT);
        pinMode(LED_BLINK, OUTPUT);

        Serial.begin(4800);

        delay(10);

        // Set GSM module baud rate
        SerialAT.begin(4800); // Rate tối đa của SoftwareSerial là 9600
        delay(3000);

        // Restart takes quite some time
        // To skip it, call init() instead of restart()
        Serial.println("Khoi dong modem...");
        //delay(3000);
        //software_Reset();
        modem.restart();

        // Unlock your SIM card with a PIN
        //modem.simUnlock("1234");

        // Blynk.config(modem, auth, server, port);
        // Blynk.connectNetwork(apn, user, pass);
        // Blynk.connect();

        Blynk.begin(auth, modem, apn, user, pass);
        timer.setInterval(5000L, CheckConnection);
        Blynk.virtualWrite(V10, "Không khóa");
}

void loop()
{
        if (Blynk.connected()) {
                Blynk.run();
        }
        timer.run();


}

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
        asm volatile ("jmp 0");
}
