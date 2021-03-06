
//#define BLYNK_PRINT Serial

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
char auth[] = "eb171b9d8bdd49b481e046b845d38f98"; //vkte02 Toán rau
char server[]          = "blynk-cloud.com";
unsigned int port      = 8442;
// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "v-internet";
char user[] = "";
char pass[] = "";
// Khai báo các biến toàn cục
bool Bit_lock = FALSE;
bool Bit_V2 = FALSE;
// Hardware Serial on Mega, Leonardo, Micro
// #define SerialAT Serial1
// Chân chức năng trên arduino
#define STOP_PIN 6
#define UP_PIN 7
#define DOWN_PIN  8
#define LED_BLINK  13
// #define A_CHANNEL 3
// #define B_CHANNEL 5
// #define C_CHANNEL 2
// #define D_CHANNEL 4


BlynkTimer timer;

// or Software Serial on Uno, Nano
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(9, 10); // RX, TX chân PWM

TinyGsm modem(SerialAT);

// Hàm đọc giá trị Virtual PIN V0 viết giá trị lên LED_BLINK
BLYNK_WRITE(V0) //Send data from app to hardware, hàm chỉ được gọi khi ta bấm nút V0 mà thôi
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);

        digitalWrite(STOP_PIN, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);


        // process received value
}
BLYNK_WRITE(V1) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        digitalWrite(UP_PIN, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        // process received value

}
BLYNK_WRITE(V2) //Send data from app to hardware
{
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        //Serial.println(pinValue);
        digitalWrite(DOWN_PIN, pinValue);
        digitalWrite(LED_BLINK, pinValue);
        delay(300);
        if (pinValue == TRUE) {
                Blynk.notify("Bật máy!");
        } else {
                Blynk.notify("Tắt máy!");
        }


        // process received value
}
// BLYNK_WRITE(V3) //Send data from app to hardware
// {
//         bool pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
//         //Serial.println(pinValue);
//         if (pinValue && Bit_lock==TRUE) {
//                 Bit_lock = FALSE; // đổi trạng thái
//                 Blynk.virtualWrite(V10, "Không khóa");
//                 //digitalWrite(STOP_PIN, 0);
//                 Serial.print("Bit lock:");
//                 //Serial.println(Bit_lock);
//         } else if (pinValue && Bit_lock==FALSE) {
//                 Bit_lock = TRUE;
//                 Blynk.virtualWrite(V10, "Khóa");
//                 digitalWrite(LED_BLINK, 1);
//                 //digitalWrite(STOP_PIN, 1);
//                 Serial.print("Bit lock:");
//                 //Serial.println(Bit_lock);
//         }



// digitalWrite(LOCK_PIN, pinValue);
// process received value
//}
void CheckConnection() {   // check every 11s if connected to Blynk server
        if (!Blynk.connected()) {
                //Serial.println("Khong ket noi toi Blynk server");
                //Blynk.connectNetwork(apn, user, pass); // try to connect to server with default timeout
                //Blynk.connect(10000);
                software_Reset();
                //Blynk.begin(auth, modem, apn, user, pass);
        }
        else {
                //Serial.println("Da ket noi Blynk server");
        }
}

void setup()
{
        // Debug console
        pinMode(STOP_PIN, OUTPUT);
        pinMode(UP_PIN, OUTPUT);
        pinMode(DOWN_PIN, OUTPUT);
        pinMode(LED_BLINK, OUTPUT);

        // pinMode(A_CHANNEL, INPUT);
        // pinMode(B_CHANNEL, INPUT);
        // pinMode(C_CHANNEL, INPUT);
        // pinMode(D_CHANNEL, INPUT);

        Serial.begin(4800);

        delay(10);

        // Set GSM module baud rate
        SerialAT.begin(4800); // Rate tối đa của SoftwareSerial là 9600
        delay(3000);

        // Restart takes quite some time
        // To skip it, call init() instead of restart()
        //Serial.println("Khoi dong modem...");
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
        //Blynk.virtualWrite(V10, "Không khóa");
}

void loop()
{
        if (Blynk.connected()) {
                Blynk.run();
        }
        timer.run();
// // Code for A_CHANNEL
//         int a_rf = digitalRead(A_CHANNEL);
//         if (Bit_lock == 0) {
//                 digitalWrite(STOP_PIN, a_rf);
//                 digitalWrite(LED_BLINK, a_rf);
//                 delay(200);
//         }
// // Code for B_CHANNEL
//         int b_rf = digitalRead(B_CHANNEL);
//         if (Bit_lock == 0) {
//                 digitalWrite(UP_PIN, b_rf);
//                 digitalWrite(LED_BLINK, b_rf);
//                 delay(200);
//         }
// // Code for C_CHANNEL
//         int c_rf = digitalRead(C_CHANNEL);
//         if (Bit_lock == 0) {
//                 digitalWrite(DOWN_PIN, c_rf);
//                 digitalWrite(LED_BLINK, c_rf);
//                 delay(200);
//         }
// // Code for D_CHANNEL
//         int d_rf = digitalRead(D_CHANNEL);
//         if (d_rf && Bit_lock == TRUE) {
//                 //Bit_lock = FALSE; // đổi trạng thái
//                 //Blynk.virtualWrite(V10, "Không khóa");
//                 //digitalWrite(STOP_PIN, 0);
//                 //Serial.print("Bit lock:");
//                 //  //Serial.println(Bit_lock);
//         } else if (d_rf == TRUE && Bit_lock == FALSE) {
//                 Bit_lock = TRUE;
//                 Blynk.virtualWrite(V10, "Khóa");
//                 digitalWrite(LED_BLINK, 1);
//                 digitalWrite(STOP_PIN, 1);
//                 Serial.print("Bit lock:");
//                 //Serial.println(Bit_lock);
//         }

}

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
        asm volatile ("jmp 0");
}
