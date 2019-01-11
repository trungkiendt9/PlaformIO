#include <Arduino.h>
//F_pwm=15625/65536=0,23 hz
//F_pwm=15625/3900=4,0 hz
// Prescale = 1024
// ICR1A từ 3900 đến 65535 xác định tần số
// OCR1A tính % theo ICR1 để xác định Duty cycle
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
        ICR1 = 3900; // Giá trị TOP để so sánh Duty cycle
//    ICR1 = 65535; // Giá trị TOP để so sánh Duty cycle
// xung răng cưa tràn sau 65535 P_clock
        OCR1A = 1900; // bằng 25% của giá trị TOP
//    OCR1A =5000; // bằng 25% của giá trị TOP
// Value=16838 -> độ rộng 25 %
        TCCR1B |= (1 << CS10)|(1 << CS12); // Pre scale = 1024
//TCCR1B |= (1 << CS10)|(1 << CS11); // Pre scale = 64
// F_clock/64=16mhz/64=250 khz
// F_clock/1024=16mhz/1024=15625Hz
//F_pwm=15625/65535=0,23 hz
//F_pwm=15625/3900=4,0 hz
}
void loop(){
}
