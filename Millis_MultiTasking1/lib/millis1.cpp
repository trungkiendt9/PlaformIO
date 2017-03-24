/*
   Blink 3 LED dùng millis không dùng delay tận dụng thời gian chết
   của vi điều khiển
 */

// KHAI BÁO CÁC THƯ VIỆN LIÊN QUAN
 #include <energia.h>
// KHAO BÁO CÁC ĐỊNH NGHĨA
 #define LED1 RED_LED
 #define LED2 BLUE_LED
 #define LED3 GREEN_LED
//xem pins_energia.h về định nghĩa các chân
//C:\Users\TRUNGKIEN\.platformio\packages\framework-energiativa\variants\EK-TM4C123GXL
//Khai báo biến toàn cục
unsigned long previousMillisRED;
unsigned long previousMillisBLUE;
unsigned long previousMillisGREEN;

unsigned int intervalRED = 3011;
unsigned int intervalBLUE = 427;
unsigned int intervalGREEN = 390;

boolean RED_state = false;
boolean BLUE_state = false;
boolean GREEN_state = false;

// Các hàm khởi tạo
void setup() {
        // initialize the digital pin as an output.
        pinMode(LED1, OUTPUT);
        pinMode(LED2, OUTPUT);
        pinMode(LED3, OUTPUT);
}

// Vòng lặp
void loop() {
        // get current time stamp
        // only need one for both if-statements
        unsigned long currentMillis = millis();

        // time to toggle LED BLUE
        if ((unsigned long)(currentMillis - previousMillisBLUE) >= intervalBLUE) {
                BLUE_state = !BLUE_state;
                digitalWrite(BLUE_LED, BLUE_state);
                // save current time to pin 12's previousMillis
                previousMillisBLUE = currentMillis;
        }
        // time to toggle LED GREEN
        if ((unsigned long)(currentMillis - previousMillisGREEN) >= intervalGREEN) {
                GREEN_state = !GREEN_state;
                digitalWrite(GREEN_LED, GREEN_state);
                // save current time to pin 12's previousMillis
                previousMillisGREEN= currentMillis;
        }
        // time to toggle LED RED
        if ((unsigned long)(currentMillis - previousMillisRED) >= intervalRED) {
                RED_state = !RED_state;
                digitalWrite(RED_LED, RED_state);
                // save current time to pin 12's previousMillis
                previousMillisRED= currentMillis;
        }
}
