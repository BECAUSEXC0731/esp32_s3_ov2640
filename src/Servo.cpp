// #include <Arduino.h>
// #include <WiFi.h>
// #include <WiFiUdp.h>
// #include <ESP32Servo.h>
// #include "pid.h"
// #include "main.h"


 
// const int packetSize = 2;

// // === 舵机引脚 ===
// const int PAN_PIN = 18;
// const int TILT_PIN = 17;
// Servo panServo;
// Servo tiltServo;

// // === 平滑滤波 ===
// class ExponentialFilter {
// private:
//     float alpha;      // 滤波系数 (0-1)
//     float value;      // 当前滤波值
//     bool initialized;
    
// public:
//     ExponentialFilter(float a = 0.3) : alpha(a), value(0), initialized(false) {}
    
//     int update(int newValue) {
//         if (!initialized) {
//             value = newValue;
//             initialized = true;
//         } else {
//             value = alpha * newValue + (1 - alpha) * value;
//         }
//         return (int)value;
//     }
// };

// // === 死区控制 ===
// class DeadZone {
// private:
//     int threshold;
//     int lastValue;
//     bool initialized;
    
// public:
//     DeadZone(int thresh = 3) : threshold(thresh), lastValue(0), initialized(false) {}
    
//     bool shouldUpdate(int newValue) {
//         if (!initialized) {
//             lastValue = newValue;
//             initialized = true;
//             return true;
//         }
        
//         if (abs(newValue - lastValue) < threshold) {
//             return false;
//         }
        
//         lastValue = newValue;
//         return true;
//     }
// };

// ExponentialFilter panFilter(0.3);  // ESP32端也做一次平滑
// ExponentialFilter tiltFilter(0.2);
// DeadZone panDeadZone(3);
// DeadZone tiltDeadZone(2);

// int currentPan = 90;
// int currentTilt = 90;
// unsigned long lastUpdateTime = 0;
// const unsigned long UPDATE_INTERVAL = 50;  // 最小更新间隔50ms

// void Servo_init() {
//     Serial.begin(115200);
    
//     // 初始化舵机
//     ESP32PWM::allocateTimer(0);
//     ESP32PWM::allocateTimer(1);
//     panServo.setPeriodHertz(50);
//     tiltServo.setPeriodHertz(50);
//     panServo.attach(PAN_PIN);
//     tiltServo.attach(TILT_PIN);
    
//     // 初始位置
//     panServo.write(90);
//     tiltServo.write(90);
//     currentPan = 90;
//     currentTilt = 90;
    
//     // 启动UDP
//     udp.begin(localPort);
//     Serial.println("Servo controller initialized");
// }

// void Servo_run() {
//     int packetLen = udp.parsePacket();
//     if (packetLen == packetSize) {
//         uint8_t buffer[2];
//         udp.read(buffer, packetSize);
        
//         int rawPan = buffer[0];
//         int rawTilt = buffer[1];
        
//         // 安全限制
//         rawPan = constrain(rawPan, 0, 180);
//         rawTilt = constrain(rawTilt, 0, 180);
        
//         // 应用滤波
//         int filteredPan = panFilter.update(rawPan);
//         int filteredTilt = tiltFilter.update(rawTilt);
        
//         // 检查死区和时间间隔
//         unsigned long currentTime = millis();
//         bool shouldMovePan = panDeadZone.shouldUpdate(filteredPan);
//         bool shouldMoveTilt = tiltDeadZone.shouldUpdate(filteredTilt);
        
//         if ((shouldMovePan || shouldMoveTilt) && 
//             (currentTime - lastUpdateTime) >= UPDATE_INTERVAL) {
            
//             // 平滑移动
//             if (shouldMovePan) {
//                 panServo.write(filteredPan);
//                 currentPan = filteredPan;
//             }
            
//             if (shouldMoveTilt) {
//                 tiltServo.write(filteredTilt);
//                 currentTilt = filteredTilt;
//             }
            
//             lastUpdateTime = currentTime;
            
//             // 打印调试信息
//             Serial.print("UDP -> Pan:");
//             Serial.print(currentPan);
//             Serial.print(" Tilt:");
//             Serial.print(currentTilt);
//             Serial.print(" Raw:(");
//             Serial.print(rawPan);
//             Serial.print(",");
//             Serial.print(rawTilt);
//             Serial.println(")");
//         }
//     }
    
//     // 小延迟避免CPU占用过高
//     delay(5);
// }