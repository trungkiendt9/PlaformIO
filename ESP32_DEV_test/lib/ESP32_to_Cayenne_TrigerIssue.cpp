// This example shows how to connect to Cayenne using an ESP32 and send/receive sample data.
// Make sure you install the ESP32 Board Package via the Arduino IDE Board Manager and select the correct ESP32 board before compiling.

// Tested: IntoRobot Fig, NodeMCU-32S

#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <WiFi.h>
#include <CayenneMQTTESP32.h>
#include <DHT.h>
// WiFi network info.
char ssid[] = "netis";
char wifiPassword[] = "password";
uint8_t my_bssid[6] = { 0x04, 0x8d, 0x38, 0x24, 0xba, 0x3f};

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "eb2c5090-f14b-11e6-8577-0128e408a1ba";
char password[] = "9d625957cc619bc017aa26add1977eec0d076748";
char clientID[] = "6df99ec0-71e3-11e7-b9ca-a3eee7466983";

unsigned long lastMillis = 0;
DHT dht(2, DHT22);
float temp;
float hum;

void setup() {
        Serial.begin(9600);
        WiFi.begin(ssid,wifiPassword,1,my_bssid);

        while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
        }

        Serial.println("");
        Serial.println("WiFi connected.");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());

        Cayenne.begin(username, password, clientID, ssid, wifiPassword);

        pinMode(5, OUTPUT);
        digitalWrite(5, HIGH);

        temp = dht.readTemperature(false); // độ C
        hum = dht.readHumidity();
}

void loop() {
        Cayenne.loop();
        Cayenne.virtualWrite(1, temp, TYPE_TEMPERATURE, UNIT_CELSIUS);
        Cayenne.virtualWrite(2, hum, TYPE_RELATIVE_HUMIDITY, UNIT_PERCENT);
        //Serial.println(WiFi.status());

        //Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
        if (millis() - lastMillis > 5000) {
                lastMillis = millis();
                //Write data to Cayenne here. This example just sends the current uptime in milliseconds.
                float t = dht.readTemperature(false); // độ C
                float h = dht.readHumidity();
                if (isnan(t) || isnan(h)) { //Is Not-A-Number
                        Serial.println("Failed to read from DHT sensor!");
                        return;
                }
                else {
                        temp = t;
                        hum = h;
                }
                // float temp = random(27, 32); // độ C
                // float hum = random(78, 92);

                // Cayenne.virtualWrite(1, temp, TYPE_TEMPERATURE, UNIT_CELSIUS);
                // Cayenne.virtualWrite(2, hum, TYPE_RELATIVE_HUMIDITY, UNIT_PERCENT);

                Serial.println(temp);
                Serial.println(hum);
                // Cayenne.virtualWrite(0, lastMillis);
                //Some examples of other functions you can use to send data.
                //Cayenne.celsiusWrite(1, 22.0);
                //Cayenne.luxWrite(2, 700);
                //Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
        }
}
CAYENNE_IN(0) // Kênh channel 0
{
        digitalWrite(5, !getValue.asInt());

}
//Default function for processing actuator commands from the Cayenne Dashboard.
//You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
        CAYENNE_LOG("CAYENNE_IN_DEFAULT(%u) - %s, %s", request.channel, getValue.getId(), getValue.asString());
        //Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}
