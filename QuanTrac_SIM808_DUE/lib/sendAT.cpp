#include <Arduino.h>
#include "QuanTrac.h"
// 1. Hàm gửi lệnh AT với 1 expected_answer
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout) {
        uint8_t x = 0,  answer = 0;

        char response[AT_RESPONSE_MAX_LENGHT];
        unsigned long previous;

        memset(response, '\0', AT_RESPONSE_MAX_LENGHT); // xóa buffer
        delay(100);

        while ( Serial3.available() > 0) Serial3.read(); // đọc input
        Serial3.println(ATcommand); // Gửi lệnh AT
        x = 0;
        previous = millis();
        // Chờ phản hồi
        do {
                if (Serial3.available() != 0) {
                        // Nếu có dữ liệu trong buffer UART, đọc và kiểm tra nó với expected_answer
                        response[x] = Serial3.read();
                        x++;
                        // Nếu đúng thì trả kết quả answer = 1, thoát hàm
                        if (strstr(response, expected_answer) != NULL)
                        {
                                answer = 1;
                        }
                }
        } while ((answer == 0) && ((millis() - previous) < timeout)); // Nếu sai thì tiếp tục thử lại cho tới hết thời gian timeout
        Serial.println(response); // In giá trị nhận được để debug
        return answer;
}
// 2. Hàm gửi lệnh AT 2 expect answer để gửi dữ liệu
int8_t sendATcommand2(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned int timeout) {
        uint8_t x = 0,  answer = 0;

        char response[AT_RESPONSE_MAX_LENGHT];
        unsigned long previous;

        memset(response, '\0', AT_RESPONSE_MAX_LENGHT); // Khởi tạo lại chuỗi về 0
        delay(100);

        while ( Serial3.available() > 0) Serial3.read(); // Xóa buffer
        Serial3.println(ATcommand); // Gửi lệnh AT
        x = 0;
        previous = millis();
        // Chờ phản hồi
        do {
                // Nếu có dữ liệu từ UART thì đọc và kiểm tra
                if (Serial3.available() != 0) {
                        response[x] = Serial3.read();
                        x++;
                        // Trả về giá trị 1 nếu nhận được expected_answer1
                        if (strstr(response, expected_answer1) != NULL)
                        {
                                answer = 1;
                        }
                        // Trả về giá trị 2 nếu nhận được expected_answer2
                        else if (strstr(response, expected_answer2) != NULL)
                        {
                                answer = 2;
                        }
                }
        }
        // Đợi time out
        while ((answer == 0) && ((millis() - previous) < timeout));
        Serial.println(response); // In giá trị nhận được để debug
        return answer;
}
