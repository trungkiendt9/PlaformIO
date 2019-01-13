#include <Arduino.h>
//F_pwm=15625/65536=0,23 hz
//F_pwm=15625/3900=4,0 hz
//F_pwm=15625/3900=4,0 hz
// Prescale = 1024
// ICR1A từ 3900 đến 65535 xác định tần số
// OCR1A tính % theo ICR1 để xác định Duty cycle
#define FREQ_PIN A5 // Biến trờ 100k để thay đổi tần số
#define DUTY_PIN A6 // Biến trở 100k để thay đổi độ rộng xung
#define OUT_PIN 9 // Chân output của TIMER1A (PB1 chân 9 và PB2 chân 10)

uint16_t pot_freq_previous;
uint16_t pot_duty_previous;
uint16_t pot_freq;
uint16_t pot_duty;

void setup(){
        TCCR1A=0; TCCR1B=0;
// RESET lại 2 thanh ghi
        DDRB |= (1 << PB1);
// Đầu ra PB1 là OUTPUT ( pin 9)
        TCCR1A |= (1 << WGM11);
        TCCR1B |= (1 << WGM12)|(1 << WGM13);
// chọn Fast PWM, chế độ chọn TOP_value tự do  ICR1
        TCCR1A |= (1 << COM1A1);
// So sánh thường( none-inverting)
        ICR1 = 7812; // Giá trị TOP để so sánh Duty cycle
//    ICR1 = 65535; // Giá trị TOP để so sánh Duty cycle
// xung răng cưa tràn sau 65535 P_clock
        OCR1A = 3906; // bằng 25% của giá trị TOP
//    OCR1A =5000; // bằng 25% của giá trị TOP
// Value=16838 -> độ rộng 25 %
        TCCR1B |= (1 << CS10)|(1 << CS12); // Pre scale = 1024
//TCCR1B |= (1 << CS10)|(1 << CS11); // Pre scale = 64
// F_clock/64=16mhz/64=250 khz
// F_clock/1024=16mhz/1024=15625Hz
//F_pwm=15625/65535=0,23 hz
//F_pwm=15625/3900=4,0 hz
       Serial.begin(9600);
}
void loop(){
        // ICR1 = 65535; // Tấn số bé nhất 0,238Hz
        // OCR1A = 32767; // Duty cycle
        // ICR1 = 3900; // Tần số lớn nhất 4Hzq
        // OCR1A = 1800; // Duty cycle
// Đọc giá trị điện áp đầu vào
        uint16_t temp_pot_freq = analogRead(FREQ_PIN); //0-1023
        delayMicroseconds(150);
        uint16_t temp_pot_duty = analogRead(DUTY_PIN); //0-1023
        delayMicroseconds(150);

// Code cho PWM FREQUENCY và DUTY
        //pot_freq = map(temp_pot_freq, 0, 1023, 3000, 65535); // Hàm chuyển tỷ lệ nhanh
        pot_freq = map(temp_pot_freq, 0, 1023, 3000, 60000); // Hàm chuyển tỷ lệ nhanh
        uint16_t pot_freq_diff;
        if (pot_freq > pot_freq_previous) {
          pot_freq_diff = pot_freq - pot_freq_previous;
        } else {
          pot_freq_diff = pot_freq_previous - pot_freq;
        }

        pot_duty = map (temp_pot_duty, 0, 1023, 1, 100);
        uint16_t pot_duty_diff;
        if (pot_duty > pot_duty_previous) {
          pot_duty_diff = pot_duty - pot_duty_previous;
        } else {
          pot_duty_diff = pot_duty_previous - pot_duty;
        }
        //Serial.println(pot_freq_diff);
        pot_duty_previous = pot_duty;
        pot_freq_previous = pot_freq;
        // Serial.println(pot_duty);
        if (pot_freq_diff > 300 || pot_duty_diff > 2) {
                pot_duty = (pot_freq/100)*pot_duty;
                ICR1 = pot_freq;
                OCR1A = pot_duty;
                // Serial.println(pot_freq);
                // Serial.println(pot_duty);
                // Serial.println(pot_freq_diff);
                // Serial.println(pot_duty_diff);
        }
        // delayMicroseconds(5000);
        delay(100);



}
