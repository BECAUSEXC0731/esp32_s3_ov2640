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
// #define PWDN_GPIO_NUM -1
// #define RESET_GPIO_NUM 40
// #define XCLK_GPIO_NUM -1
// #define SIOD_GPIO_NUM 41
// #define SIOC_GPIO_NUM 42

// #define Y9_GPIO_NUM 10
// #define Y8_GPIO_NUM 11
// #define Y7_GPIO_NUM 12
// #define Y6_GPIO_NUM 13
// #define Y5_GPIO_NUM 14
// #define Y4_GPIO_NUM 15
// #define Y3_GPIO_NUM 16
// #define Y2_GPIO_NUM 17

// #define VSYNC_GPIO_NUM 2
// #define HREF_GPIO_NUM 1
// #define PCLK_GPIO_NUM 47

// // =================== 测试模式开关 ===================
// // 取消注释下面一行，启用彩条模式（摄像头输出内部彩条，不采集真实图像）
// // #define ENABLE_COLOR_BAR_TEST

// // 取消注释下面一行，启用手动像素修改测试（将图像分成四块颜色，观察屏幕显示）
// // #define ENABLE_MANUAL_PIXEL_TEST

// // 取消注释下面一行，启用JPEG串口输出测试（需PC端串口图像接收软件）
// #define ENABLE_JPEG_SERIAL_OUT

// // =================== 函数声明 ===================
// void i2c_scan();
// void manual_reset();
// void send_jpeg_over_serial(uint8_t *buf, size_t len);

// void setup()
// {
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("\n=========================");
//   Serial.println("ESP32-S3 Camera Debug Mode");
//   Serial.println("=========================");

//   St7735_Init();
//   tft.fillScreen(ST7735_BLACK);
//   tft.setCursor(0, 0);
//   tft.setTextColor(ST7735_WHITE);
//   tft.println("Debug Start");
//   delay(1000);

//   // 打印内存信息
//   Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
//   Serial.printf("Total heap: %d bytes\n", ESP.getHeapSize());
//   Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
//   Serial.printf("Total PSRAM: %d bytes\n", ESP.getPsramSize());
//   Serial.printf("Free DMA memory: %d bytes\n", heap_caps_get_free_size(MALLOC_CAP_DMA));

//   if (RESET_GPIO_NUM != -1)
//   {
//     manual_reset();
//   }

//   Serial.println("Scanning I2C bus...");
//   i2c_scan();

//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");
//   int wifi_timeout = 0;
//   while (WiFi.status() != WL_CONNECTED && wifi_timeout < 20)
//   {
//     delay(500);
//     Serial.print(".");
//     wifi_timeout++;
//   }
//   if (WiFi.status() == WL_CONNECTED)
//   {
//     Serial.println("\nWiFi connected!");
//     Serial.print("ESP32 IP: ");
//     Serial.println(WiFi.localIP());
//   }
//   else
//   {
//     Serial.println("\nWiFi connection failed (continue anyway)");
//   }

//   // ========== 摄像头配置 ==========
//   camera_config_t config;
//   config.pin_d0 = Y2_GPIO_NUM;
//   config.pin_d1 = Y3_GPIO_NUM;
//   config.pin_d2 = Y4_GPIO_NUM;
//   config.pin_d3 = Y5_GPIO_NUM;
//   config.pin_d4 = Y6_GPIO_NUM;
//   config.pin_d5 = Y7_GPIO_NUM;
//   config.pin_d6 = Y8_GPIO_NUM;
//   config.pin_d7 = Y9_GPIO_NUM;
//   config.pin_xclk = XCLK_GPIO_NUM;
//   config.pin_pclk = PCLK_GPIO_NUM;
//   config.pin_vsync = VSYNC_GPIO_NUM;
//   config.pin_href = HREF_GPIO_NUM;
//   config.pin_sccb_sda = SIOD_GPIO_NUM;
//   config.pin_sccb_scl = SIOC_GPIO_NUM;
//   config.pin_pwdn = PWDN_GPIO_NUM;
//   config.pin_reset = RESET_GPIO_NUM;
//   config.xclk_freq_hz = 10000000; // 降低时钟频率
//   config.jpeg_quality = 12;
//   config.fb_count = 1;

// #ifdef ENABLE_JPEG_SERIAL_OUT
//   // JPEG 模式
//   config.pixel_format = PIXFORMAT_JPEG;
//   config.frame_size = FRAMESIZE_QVGA; // 320x240，便于传输
// #else
//   // RGB565 模式（用于屏幕显示）
//   config.pixel_format = PIXFORMAT_RGB565;
//   config.frame_size = FRAMESIZE_96X96; // 可先测试小分辨率
// #endif

//   esp_err_t err = esp_camera_init(&config);
//   if (err != ESP_OK)
//   {
//     Serial.printf("Camera init failed: %s (0x%x)\n", esp_err_to_name(err), err);
//     tft.fillScreen(ST7735_RED);
//     tft.setCursor(0, 0);
//     tft.println("Cam Error!");
//     tft.print("Err: ");
//     tft.println(err, HEX);
//     while (1)
//       delay(100);
//   }

//   Serial.println("Camera initialized!");

//   // ========== 传感器微调 ==========
//   sensor_t *s = esp_camera_sensor_get();
//   if (s)
//   {
//     // 尝试交换 RGB 字节序（常见花屏原因）
//     // 注意：不同库可能函数名不同，请根据实际情况调整
//     // 如果编译报错，请注释掉下面这行
//     // s->set_rgb565_swap(s, 1);   // 1=交换

//     // 基础设置
//     s->set_brightness(s, 0);
//     s->set_contrast(s, 0);
//     s->set_saturation(s, 0);
//     s->set_whitebal(s, 1);
//     s->set_gain_ctrl(s, 1);
//     s->set_exposure_ctrl(s, 1);
//     s->set_hmirror(s, 0);
//     s->set_vflip(s, 0);

// #ifdef ENABLE_COLOR_BAR_TEST
//     // 启用彩条模式
//     s->set_colorbar(s, 1);
//     Serial.println("Color bar test enabled.");
// #endif

//     Serial.println("Sensor settings applied.");
//   }
//   else
//   {
//     Serial.println("Failed to get sensor.");
//   }

//   tft.fillScreen(ST7735_BLACK);
//   tft.setCursor(0, 0);
//   tft.println("Ready");
//   delay(1000);
// }

// void loop()
// {
//   camera_fb_t *fb = esp_camera_fb_get();
//   if (!fb)
//   {
//     Serial.println("Failed to get frame");
//     tft.fillScreen(ST7735_RED);
//     tft.setCursor(0, 0);
//     tft.println("Frame Error");
//     delay(500);
//     return;
    
//   }
//   if (fb->format == PIXFORMAT_RGB565)//-------------------------------------------------------------
//     {
//       uint16_t *img = (uint16_t *)fb->buf;
//       // 手动交换每个像素的高低字节（测试字节序）
//       for (int i = 0; i < fb->width * fb->height; i++)
//       {
//         uint16_t p = img[i];
//         img[i] = (p << 8) | (p >> 8); // 高低字节交换
//       }
//       tft.drawRGBBitmap(0, 0, img, fb->width, fb->height);
//     }
//     else
//     {
//       // ... 错误处理
//     }

//     esp_camera_fb_return(fb);

// #ifdef ENABLE_JPEG_SERIAL_OUT
//   // JPEG 串口输出测试
//   Serial.printf("JPEG frame size: %d bytes\n", fb->len);
//   send_jpeg_over_serial(fb->buf, fb->len);
//   esp_camera_fb_return(fb);
//   delay(100); // 降低频率避免串口阻塞
//   return;
// #endif

//   // 对于 RGB565 模式
//   if (fb->format != PIXFORMAT_RGB565)
//   {
//     Serial.printf("Unexpected format: %d\n", fb->format);
//     tft.fillScreen(ST7735_BLACK);
//     tft.setCursor(0, 0);
//     tft.println("Wrong format");
//     esp_camera_fb_return(fb);
//     delay(100);
//     return;
//   }

//   uint16_t *img = (uint16_t *)fb->buf;
//   int w = fb->width;
//   int h = fb->height;

// #ifdef ENABLE_MANUAL_PIXEL_TEST
//   // 手动修改像素：将图像分成四块颜色
//   for (int y = 0; y < h; y++)
//   {
//     for (int x = 0; x < w; x++)
//     {
//       int idx = y * w + x;
//       if (x < w / 2 && y < h / 2)
//       {
//         img[idx] = 0xF800; // 红色
//       }
//       else if (x >= w / 2 && y < h / 2)
//       {
//         img[idx] = 0x07E0; // 绿色
//       }
//       else if (x < w / 2 && y >= h / 2)
//       {
//         img[idx] = 0x001F; // 蓝色
//       }
//       else
//       {
//         img[idx] = 0xFFFF; // 白色
//       }
//     }
//   }
//   Serial.println("Manual pixel test applied.");
// #endif

//   // 显示图像
//   tft.drawRGBBitmap(0, 0, img, w, h);

//   esp_camera_fb_return(fb);
//   // delay(10);
// }

// // =================== I2C 扫描 ===================
// void i2c_scan()
// {
//   Wire.begin(SIOD_GPIO_NUM, SIOC_GPIO_NUM);
//   byte error, address;
//   int nDevices = 0;

//   Serial.println("Scanning...");
//   for (address = 1; address < 127; address++)
//   {
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();
//     if (error == 0)
//     {
//       Serial.printf("I2C device found at address 0x%02X\n", address);
//       nDevices++;
//       if (address == 0x30 || address == 0x60)
//       {
//         Serial.println("  -> Typical OV2640 address!");
//       }
//     }
//     else if (error == 4)
//     {
//       Serial.printf("Unknown error at address 0x%02X\n", address);
//     }
//   }
//   if (nDevices == 0)
//   {
//     Serial.println("No I2C devices found. Check wiring and power!");
//   }
//   else
//   {
//     Serial.printf("Done. Found %d device(s).\n", nDevices);
//   }
// }

// // =================== 手动复位 ===================
// void manual_reset()
// {
//   if (RESET_GPIO_NUM == -1)
//     return;
//   pinMode(RESET_GPIO_NUM, OUTPUT);
//   digitalWrite(RESET_GPIO_NUM, LOW);
//   delay(10);
//   digitalWrite(RESET_GPIO_NUM, HIGH);
//   delay(50);
//   Serial.println("Manual reset pulse sent.");
// }

// // =================== JPEG 串口发送 (修正版) ===================
// #ifdef ENABLE_JPEG_SERIAL_OUT
// void send_jpeg_over_serial(uint8_t *buf, size_t len)
// {
//   // 摄像头生成的 buf 已经包含完整的 FF D8 ... FF D9
//   // 不需要也不应该再手动添加帧头帧尾，否则会导致数据重复，PC端解析失败

//   // 可选：为了便于PC端区分每一帧，可以在前后加一些非JPEG的特殊标记，
//   // 但最简单的做法是直接发送原始数据，依靠识别 FF D8 和 FF D9 来切割。

//   Serial.write(buf, len);
//   // 发送一个换行符作为帧结束的标志（可选，方便调试，但不影响JPEG解析）
//   // Serial.println();
// }
// #endif