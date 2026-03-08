// #include "Servo.h"
// #include "WiFi.h"
// #include "WiFiUdp.h"
// #include "St7735.h"
// #include "esp_camera.h"
// #include <Wire.h>

// // =================== WiFi 配置 ===================
// const char *ssid = "doomsday";
// const char *password = "123123123";
// extern WiFiUDP udp;

// // =================== 摄像头引脚定义 ===================
// #define PWDN_GPIO_NUM   -1
// #define RESET_GPIO_NUM   40
// #define XCLK_GPIO_NUM    -1
// #define SIOD_GPIO_NUM    41
// #define SIOC_GPIO_NUM    42

// #define Y9_GPIO_NUM      10
// #define Y8_GPIO_NUM      11
// #define Y7_GPIO_NUM      12
// #define Y6_GPIO_NUM      13
// #define Y5_GPIO_NUM      14
// #define Y4_GPIO_NUM      15
// #define Y3_GPIO_NUM      16
// #define Y2_GPIO_NUM      17

// #define VSYNC_GPIO_NUM    2
// #define HREF_GPIO_NUM     1
// #define PCLK_GPIO_NUM     47

// // =================== 函数声明 ===================
// void i2c_scan();
// void manual_reset();

// void setup()
// {
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("\n=========================");
//   Serial.println("ESP32-S3 Camera Test with Debug");
//   Serial.println("=========================");

//   St7735_Init();
//   tft.fillScreen(ST7735_BLACK);
//   tft.setCursor(0, 0);
//   tft.setTextColor(ST7735_WHITE);
//   tft.println("Starting...");
//   delay(1000);

//   Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
//   Serial.printf("Total heap: %d bytes\n", ESP.getHeapSize());
//   Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
//   Serial.printf("Total PSRAM: %d bytes\n", ESP.getPsramSize());
//   // 新增：打印 DMA 可用内存
//   Serial.printf("Free DMA memory: %d bytes\n", heap_caps_get_free_size(MALLOC_CAP_DMA));

//   if (RESET_GPIO_NUM != -1) {
//     manual_reset();
//   }

//   Serial.println("Scanning I2C bus...");
//   i2c_scan();

//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");
//   int wifi_timeout = 0;
//   while (WiFi.status() != WL_CONNECTED && wifi_timeout < 20) {
//     delay(500);
//     Serial.print(".");
//     wifi_timeout++;
//   }
//   if (WiFi.status() == WL_CONNECTED) {
//     Serial.println("\nWiFi connected!");
//     Serial.print("ESP32 IP: ");
//     Serial.println(WiFi.localIP());
//   } else {
//     Serial.println("\nWiFi connection failed (continue anyway)");
//   }

//   // 配置摄像头
//   camera_config_t config;
//   config.pin_d0       = Y2_GPIO_NUM;
//   config.pin_d1       = Y3_GPIO_NUM;
//   config.pin_d2       = Y4_GPIO_NUM;
//   config.pin_d3       = Y5_GPIO_NUM;
//   config.pin_d4       = Y6_GPIO_NUM;
//   config.pin_d5       = Y7_GPIO_NUM;
//   config.pin_d6       = Y8_GPIO_NUM;
//   config.pin_d7       = Y9_GPIO_NUM;
//   config.pin_xclk     = XCLK_GPIO_NUM;
//   config.pin_pclk     = PCLK_GPIO_NUM;
//   config.pin_vsync    = VSYNC_GPIO_NUM;
//   config.pin_href     = HREF_GPIO_NUM;
//   config.pin_sccb_sda = SIOD_GPIO_NUM;
//   config.pin_sccb_scl = SIOC_GPIO_NUM;
//   config.pin_pwdn     = PWDN_GPIO_NUM;
//   config.pin_reset    = RESET_GPIO_NUM;
//   config.xclk_freq_hz = 20000000;
//   config.pixel_format = PIXFORMAT_RGB565;
//   config.frame_size   = FRAMESIZE_96X96;   // 改为更小的分辨率 96x96
//   config.jpeg_quality = 12;
//   config.fb_count     = 1;                  // 单缓冲

//   esp_err_t err = esp_camera_init(&config);
//   if (err != ESP_OK) {
//     Serial.printf("Camera init failed with error: %s (0x%x)\n", esp_err_to_name(err), err);
//     tft.fillScreen(ST7735_RED);
//     tft.setCursor(0, 0);
//     tft.println("Cam Error!");
//     tft.print("Err: ");
//     tft.println(err, HEX);
//     while (1) {
//       delay(100);
//     }
//   }

//   Serial.println("Camera initialized successfully!");
//   tft.fillScreen(ST7735_BLACK);
//   tft.setCursor(0, 0);
//   tft.println("Cam OK!");
//   delay(1000);
// }

// void loop()
// {
//   camera_fb_t *fb = esp_camera_fb_get();
//   if (!fb) {
//     Serial.println("Failed to get frame");
//     tft.fillScreen(ST7735_RED);
//     tft.setCursor(0, 0);
//     tft.println("Frame Error");
//     delay(500);
//     return;
//   }

//   if (fb->format == PIXFORMAT_RGB565) {
//     // 显示图像（可能需要在屏幕上居中显示 96x96 的图像）
//     // 如果屏幕是 160x128，可以在左上角显示
//     tft.drawRGBBitmap(0, 0, (uint16_t*)fb->buf, fb->width, fb->height);
//   } else {
//     tft.fillScreen(ST7735_BLACK);
//     tft.setCursor(0, 0);
//     tft.println("Wrong format");
//     Serial.printf("Unexpected format: %d\n", fb->format);
//   }

//   esp_camera_fb_return(fb);
//   // delay(10);
// }

// void i2c_scan() {
//   Wire.begin(SIOD_GPIO_NUM, SIOC_GPIO_NUM);
//   byte error, address;
//   int nDevices = 0;

//   Serial.println("Scanning...");
//   for (address = 1; address < 127; address++ ) {
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();
//     if (error == 0) {
//       Serial.printf("I2C device found at address 0x%02X\n", address);
//       nDevices++;
//       if (address == 0x30 || address == 0x60) {
//         Serial.println("  -> This is the typical OV2640 address!");
//       }
//     } else if (error == 4) {
//       Serial.printf("Unknown error at address 0x%02X\n", address);
//     }
//   }
//   if (nDevices == 0) {
//     Serial.println("No I2C devices found. Check wiring and power!");
//   } else {
//     Serial.printf("Done. Found %d device(s).\n", nDevices);
//   }
// }

// void manual_reset() {
//   if (RESET_GPIO_NUM == -1) return;
//   pinMode(RESET_GPIO_NUM, OUTPUT);
//   digitalWrite(RESET_GPIO_NUM, LOW);
//   delay(10);
//   digitalWrite(RESET_GPIO_NUM, HIGH);
//   delay(50);
//   Serial.println("Manual reset pulse sent.");
// } 