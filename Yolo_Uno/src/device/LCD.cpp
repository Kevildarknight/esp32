#include "LCD.h"

// #define LCD_ADDRESS 0x27  // Địa chỉ I2C mặc định của LCD
// #define LCD_COLUMNS 16   // Số cột LCD
// #define LCD_ROWS 2      // Số dòng LCD
#define TRIG_PIN 18     // GPIO18 (D9)
#define ECHO_PIN 21     // GPIO21 (D10)
#define DELAY_TIME 1000 // Đọc mỗi 1 giây
#define TIMEOUT 30000   // Timeout 30ms (~5m)
#define DISPLAY_TIME 5000 // Thời gian hiển thị 5s
#define MY_SCL 11
#define MY_SDA 12
// Định nghĩa các chân ADC
#define ADC_A0 1  // GPIO1
#define ADC_A1 2  // GPIO2
#define ADC_A2 3  // GPIO3
#define ADC_A3 4  // GPIO4
LiquidCrystal_I2C lcd(33,16,2);
extern DHT20 dht20;     // Từ TaskDHT20.cpp
extern float distance;  // Từ UltrasonicSensor.cpp

// Biến lưu giá trị ADC
int adcValues[4] = {0, 0, 0, 0};

void TaskLCD(void *pvParameters)
{
    int displayMode = 0;
    
    while (true)
    {
        switch(displayMode) {
            case 0:
                displayTempHumidity();
                break;
            case 1:
                displayADCValues();
                break;
            case 2:
                displayDistance();
                break;
        }
        
        // Chuyển sang chế độ hiển thị tiếp theo
        displayMode = (displayMode + 1) % 3;
        
        vTaskDelay(DISPLAY_TIME / portTICK_PERIOD_MS);
    }
}

void displayTempHumidity()
{
    // Đọc giá trị từ DHT20
    if (dht20.read() == DHT20_OK)
    {
        float temperature = dht20.getTemperature();
        float humidity = dht20.getHumidity();
        
        // Xóa màn hình
        lcd.clear();
        
        // Hiển thị nhiệt độ trên dòng 1
        lcd.setCursor(0, 0);
        lcd.print("Temp: ");
        lcd.print(temperature, 1);  // Hiển thị với 1 chữ số thập phân
        lcd.print((char)223);       // Ký hiệu độ
        lcd.print("C");
        
        // Hiển thị độ ẩm trên dòng 2
        lcd.setCursor(0, 1);
        lcd.print("Humidity: ");
        lcd.print(humidity, 1);
        lcd.print("%");
        
        Serial.println("LCD: Displaying temperature and humidity");
    }
    else
    {
        // Hiển thị lỗi nếu không đọc được DHT20
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("DHT20 Error!");
        
        Serial.println("LCD: Failed to read DHT20");
    }
}

void    displayADCValues()
{
    // Đọc giá trị từ 4 kênh ADC
    adcValues[0] = analogRead(ADC_A0);
    adcValues[1] = analogRead(ADC_A1);
    adcValues[2] = analogRead(ADC_A2);
    adcValues[3] = analogRead(ADC_A3);
    
    // Xóa màn hình
    lcd.clear();
    
    // Hiển thị A0 và A1 trên dòng 1
    lcd.setCursor(0, 0);
    lcd.print("A0:");
    lcd.print(adcValues[0]);
    lcd.setCursor(8, 0);
    lcd.print("A1:");
    lcd.print(adcValues[1]);
    
    // Hiển thị A2 và A3 trên dòng 2
    lcd.setCursor(0, 1);
    lcd.print("A2:");
    lcd.print(adcValues[2]);
    lcd.setCursor(8, 1);
    lcd.print("A3:");
    lcd.print(adcValues[3]);
    
    Serial.println("LCD: Displaying ADC values");
    Serial.print("A0: "); Serial.print(adcValues[0]);
    Serial.print(" | A1: "); Serial.print(adcValues[1]);
    Serial.print(" | A2: "); Serial.print(adcValues[2]);
    Serial.print(" | A3: "); Serial.println(adcValues[3]);
}

void displayDistance()
{
    // Xóa màn hình
    lcd.clear();
    
    // Hiển thị khoảng cách
    lcd.setCursor(0, 0);
    lcd.print("Distance:");
    
    lcd.setCursor(0, 1);
    if (distance > 0) {
        lcd.print(distance, 1);
        lcd.print(" cm");
        
        // Hiển thị thêm cảnh báo nếu quá gần
        if (distance < 10) {
            lcd.setCursor(10, 1);
            lcd.print("NEAR!");
        }
    } else {
        lcd.print("Out of range");
    }
    
    Serial.println("LCD: Displaying distance");
}

void initLCD()
{
    // Khởi tạo LCD
    lcd.begin();

    // Kiểm tra kết nối
        Serial.println("LCD: Connected successfully");
        
        // Bật đèn nền
        lcd.backlight();
        
        // Hiển thị thông báo khởi động
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("System Starting");
        lcd.setCursor(0, 1);
        lcd.print("Test Mode");
        
        delay(2000);
        
        // Cấu hình các chân ADC
        pinMode(ADC_A0, INPUT);
        pinMode(ADC_A1, INPUT);
        pinMode(ADC_A2, INPUT);
        pinMode(ADC_A3, INPUT);
        analogReadResolution(12);  // 12-bit (0-4095)
        
        // Tạo task để hiển thị
        xTaskCreate(TaskLCD, "Task_LCD", 2048, NULL, 1, NULL);
}