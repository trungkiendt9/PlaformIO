#include <Arduino.h>
#include <EEPROM.h>
#include <SimpleTimer.h>


#define TRUE          1
#define FALSE         0

#define STOP_PIN 8
#define START_PIN 7
#define LED_BLINK  13
#define DC_IN A6

#define TimeKhoiDong_ms 60000
#define TimeTatMay_ms 1000
#define TimeDongDienRaTai_ms 3000


#define ChuKyChay_ngay 10
#define SoChuKy 10

//Khai báo biến toàn cục
int address = 1;
byte Bit_lock = 0;
int CoDien = 1023;
bool flag = 0;


SimpleTimer timer;


void TenTimesTask() {
        //static int k = 0;
        Bit_lock++;
        Serial.print("Ghi ROM ");
        EEPROM.write(address, Bit_lock);
        Serial.print(Bit_lock);
        Serial.print(" / ");
        Serial.println(SoChuKy);


}

void KhoiDong(){
        if (Bit_lock < SoChuKy) {
                digitalWrite(START_PIN, TRUE);
                delay(TimeKhoiDong_ms);
                Serial.println("Khởi động");
        } else {
                digitalWrite(START_PIN, FALSE);
        }

}

void XungRaTai(){
        if (Bit_lock < SoChuKy) {
                digitalWrite(START_PIN, TRUE);
                delay(TimeDongDienRaTai_ms);
                Serial.println("Đóng điện ra tải!");
        } else {
                digitalWrite(START_PIN, FALSE);
        }

}

void TatMay(){
        if (Bit_lock < SoChuKy ) {
                digitalWrite(STOP_PIN, TRUE);
                delay(TimeTatMay_ms);
                Serial.println("Tắt máy");

        } else {
                digitalWrite(STOP_PIN, FALSE);
        }

}

void setup()
{
        // Debug console
        pinMode(STOP_PIN, OUTPUT);
        pinMode(START_PIN, OUTPUT);
        pinMode(LED_BLINK, OUTPUT);

        pinMode(DC_IN, INPUT);

        Serial.begin(4800);
        Serial.println("Đếm số chu kỳ");

        delay(10);
        //EEPROM.write(address, 0);

        timer.setTimer(ChuKyChay_ngay*86400000, TenTimesTask, SoChuKy); //

}

void loop()
{
        Bit_lock = EEPROM.read(address);
        Serial.println(Bit_lock);
        CoDien = analogRead(DC_IN);
        timer.run();
        if (CoDien < 9) {
                KhoiDong();
                digitalWrite(START_PIN, FALSE);
                delay(30000);
                XungRaTai();
                digitalWrite(START_PIN, FALSE);
        };

        // if (CoDien == FALSE && flag == 1) {
        //         digitalWrite(START_PIN, FALSE); // Coi như đã khởi động xong
        //         flag = 1;
        // };
        delay(15000);
        if (CoDien > 800) {
                TatMay();
                digitalWrite(STOP_PIN, FALSE);
                //flag = 0;
        };

        // if (CoDien > 500 && flag == 0) {
        //         digitalWrite(STOP_PIN, FALSE);
        //         flag = 0;
        // };

        Serial.print("Flag = ");
        Serial.println(flag);
        Serial.print("Có điện = ");
        Serial.println(CoDien);
        delay(1000);

}

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
        asm volatile ("jmp 0");
}
