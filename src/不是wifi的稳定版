#include "Servo.h"
#include "WiFi.h"
#include "WiFiUdp.h"
#include "St7735.h"
#include "esp_camera.h"
#include <Wire.h>

// =================== WiFi 配置 ===================
const char *ssid = "doomsday";
const char *password = "123123123";
// #define DISABLE_WIFI

// =================== 摄像头引脚定义 ===================
#define PWDN_GPIO_NUM   -1
#define RESET_GPIO_NUM  40
#define XCLK_GPIO_NUM   -1
#define SIOD_GPIO_NUM   42
#define SIOC_GPIO_NUM   41

#define Y9_GPIO_NUM      10
#define Y8_GPIO_NUM      11
#define Y7_GPIO_NUM      12
#define Y6_GPIO_NUM      13
#define Y5_GPIO_NUM      14
#define Y4_GPIO_NUM      15
#define Y3_GPIO_NUM      16
#define Y2_GPIO_NUM      17

#define VSYNC_GPIO_NUM    2
#define HREF_GPIO_NUM     1
#define PCLK_GPIO_NUM     47

// =================== 屏幕参数 ===================
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 160

// 帧缓冲区
uint16_t *screenBuffer = nullptr;

// 预计算映射表（旋转+缩放）
int *xMap = nullptr;   // 源Y坐标映射
int *yMap = nullptr;   // 源X坐标映射
int last_img_w = 0, last_img_h = 0;  // 缓存上次分辨率

void manual_reset();
void camera_setup();

bool camera_ok = false;
bool has_psram = false;

void setup() {
  // 分配屏幕缓冲区
  screenBuffer = (uint16_t*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t));
  if (!screenBuffer) while(1);

  // 预分配映射表
  xMap = (int*)malloc(SCREEN_WIDTH * sizeof(int));
  yMap = (int*)malloc(SCREEN_HEIGHT * sizeof(int));
  if (!xMap || !yMap) while(1);

  St7735_Init();
  tft.fillScreen(ST7735_BLACK);
  
  has_psram = (ESP.getPsramSize() > 0);

  if (RESET_GPIO_NUM != -1) manual_reset();

  Wire.begin(SIOD_GPIO_NUM, SIOC_GPIO_NUM);

#ifndef DISABLE_WIFI
  WiFi.begin(ssid, password);
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    timeout++;
  }
#endif

  camera_setup();

  if (!camera_ok) while(1) delay(1000);

  tft.fillScreen(ST7735_BLACK);
}

void loop() {
  if (!camera_ok) {
    delay(1000);
    return;
  }

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    delay(100);
    return;
  }

  if (fb->format == PIXFORMAT_RGB565) {
    uint16_t *img = (uint16_t *)fb->buf;
    int img_w = fb->width;
    int img_h = fb->height;

    // 若分辨率改变，重新计算映射表
    if (img_w != last_img_w || img_h != last_img_h) {
      // 水平映射：目标 x -> 源 Y (旋转后的行)
      for (int x = 0; x < SCREEN_WIDTH; x++) {
        xMap[x] = (x * img_h) / SCREEN_WIDTH;
      }
      // 垂直映射：目标 y -> 源 X (旋转后的列，顺时针旋转90°)
      for (int y = 0; y < SCREEN_HEIGHT; y++) {
        // 源X = img_w - 1 - (y * img_w) / SCREEN_HEIGHT
        yMap[y] = img_w - 1 - (y * img_w) / SCREEN_HEIGHT;
      }
      last_img_w = img_w;
      last_img_h = img_h;
    }

    // 填充缓冲区：同时完成旋转、缩放、字节交换
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
      int src_x_base = yMap[y];
      int dst_row = y * SCREEN_WIDTH;
      for (int x = 0; x < SCREEN_WIDTH; x++) {
        int src_y = xMap[x];
        uint16_t pixel = img[src_y * img_w + src_x_base];
        // 字节交换
        screenBuffer[dst_row + x] = (pixel << 8) | (pixel >> 8);
      }
    }

    // 一次性绘制
    tft.drawRGBBitmap(0, 0, screenBuffer, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  esp_camera_fb_return(fb);
}

// =================== 摄像头配置 ===================
void camera_setup() {
  camera_config_t config;
  memset(&config, 0, sizeof(config));

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 8000000;
  config.pixel_format = PIXFORMAT_RGB565;

  if (has_psram) {
    config.frame_size = FRAMESIZE_QVGA;   // 320x240
    config.fb_count = 2;
    config.fb_location = CAMERA_FB_IN_PSRAM;
  } else {
    config.frame_size = FRAMESIZE_QQVGA;  // 160x120
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    camera_ok = false;
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s) {
    s->set_brightness(s, 0);
    s->set_contrast(s, 0);
    s->set_saturation(s, 0);
    s->set_whitebal(s, 1);
    s->set_gain_ctrl(s, 0);
    s->set_exposure_ctrl(s, 1);
    s->set_hmirror(s, 0);
    s->set_vflip(s, 0);
  }
  camera_ok = true;
}

void manual_reset() {
  if (RESET_GPIO_NUM == -1) return;
  pinMode(RESET_GPIO_NUM, OUTPUT);
  digitalWrite(RESET_GPIO_NUM, LOW);
  delay(20);
  digitalWrite(RESET_GPIO_NUM, HIGH);
  delay(100);
}