#include <Arduino.h>
#include "QuanTrac.h"
//Hàm gửi tin nhắn sms_send gửi bảng ký tự ASCII 8 bit
boolean sms_send(char* phone_number, char* sms_message){
//        memset(GLO_cmd_buff, '\0', AT_CMD_MAX_LENGHT);
        Serial.print("Setting SMS mode...");
        sendATcommand("AT+CMGF=1", "OK", RESPONSE_TIMEOUT_MILLISECONDS); // Thiết lập SMS mode
        Serial.println("Sending SMS");
        sprintf(GLO_cmd_buff,"AT+CMGS=\"%s\"", phone_number);
        if(sendATcommand(GLO_cmd_buff, ">", RESPONSE_TIMEOUT_MILLISECONDS)) {// gửi sms message
                snprintf(GLO_cmd_buff, sizeof(GLO_cmd_buff), "%s%c", sms_message, 0x1A); // Kết thúc tin nhắn bằng cách gửi mã ASCII \0x1a  SUB  substitute  ctrl-Z
                //Serial1.println(char(26));
                if (sendATcommand(GLO_cmd_buff, "OK", RESPONSE_TIMEOUT_MILLISECONDS))
                {
                        Serial.println("Sent OK");
                }
                else
                {
                        Serial.println("ERROR");
                }
                return 1;
        }
        else
        {
                Serial.println("FAILED");
                return 0;
        }
}
