/*
   Khoa điện - điện tử
   vktech.edu.vn
   ee.vktech@gmail.com

   Khuyến nghị về độ ẩm không khí
   Comfortable: 30% - 60%
   Recommended: 45% - 55%
   High: 55% - 80%

   • If outside temperature is 20 to 40 degrees, humidity indoors should not be more than 40 percent.
   • If outside temperature is 10 to 20 degrees, humidity indoors should not be more than 35 percent.
   • If outside temperature is 0 to 10 degrees, humidity indoors should not be more than 30 percent.
   • If outside temperature is 10-below to 0, humidity indoors should not be more than 25 percent.
   • If outside temperature is 20-below to 10-below, humidity indoors should not be more than 20 percent.
   • If outdoor temperature is lower than 20-below, inside humidity should not be more than 15 percent.


 */
#include "DHT.h"

#define DHTPIN PD_0     // Chân DATA nối với PD0

// Uncomment loại cảm biến bạn sử dụng, nếu DHT11 thì uncomment DHT11 và comment DHT22
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Kết nối
// DHT       | TIVA C
//----------------
// VCC(1)    |  3.3V
// DATA(2)   |  PD0
// NC(3)     |  x
// GND(4)    |  GND

// Kết nối chân 1 của DHT với 3.3V
// Chân 2 kết nối với PD0 của TIVA C
// Chân 4 nối với GND
// Nối trở 10k giữa chân 1 và chân 2

// Khởi tạo cảm biến
DHT dht(DHTPIN, DHTTYPE);
class env_param {
// Khai báo các biến trong class là private mặc định
// (private/public/protected)
public:
float temperatureC;
float humidity;
float temperatureF;
// Khai báo các hàm khởi tạo và hàm con của kiểu env_param
public:
env_param(float tC, float h, float tF);
};

void setup() {
        // Khởi tạo cổng serial baud 9600
        Serial.begin(9600);
        Serial.println("DHTxx test!");
        // Bắt đầu đọc dữ liệu
        dht.begin();
}
//env_param ENV(dht.readTemperature(), dht.readHumidity(), dht.readTemperature(true));
void loop() {
        // Đợi chuyển đổi dữ liệu khoảng 2s
        delay(3000);

        float h = dht.readHumidity();
        // Đọc giá trị nhiệt độ C (mặc định)
        float t = dht.readTemperature();
        // Đọc giá trị nhiệt độ F(isFahrenheit = true)
        float f = dht.readTemperature(true);

        // Kiểm tra quá trình đọc thành công hay không
        if (isnan(h) || isnan(t) || isnan(f)) {
                Serial.println("Failed to read from DHT sensor!");
                return;
        }

        // Hiển thị thông tin về mức độ phù hợp của độ ẩm
        if (h>=30&&h<=60) {
                Serial.println("GOOD!. Độ ẩm khá thoải mái......");
        } else if (h>60) {
                Serial.println("WET!. Độ ẩm cao!!!!!!!!!!!!!");
        } else {
                Serial.println("DRY DRY!. Độ ẩm thấp");
        }

        // Tính chỉ số nhiệt độ F (mặc định)
        //  float hif = dht.computeHeatIndex(f, h);
        // Tính chỉ số nhiệt độ C (isFahreheit = false)
        //  float hic = dht.computeHeatIndex(t, h, false);

        // IN thông tin ra màn hình
        Serial.print("Do am: ");
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Nhiet do: ");
        Serial.print(t);
        Serial.println(" *C ");
        //  Serial.print(f);
        //  Serial.print(" *F\t");
        //  Serial.print("Heat index: ");
        //  Serial.print(hic);
        //  Serial.print(" *C ");
        //  Serial.print(hif);
        //  Serial.println(" *F");
}
// TRIỂN KHAI CÁC HÀM CỦA KIỂU DỮ LIỆU env_param
// 6. Hàm khởi tạo gán giá trị cho các biến của env_param
env_param::env_param(float tC, float h, float tF) {
        temperatureC = tC;
        humidity = h;
        temperatureF = tF;
        if (isnan(temperatureC) || isnan(humidity) || isnan(temperatureF)) {
                Serial.println("Lỗi khi đọc dữ liệu DHT22");
                return;
        }
}
