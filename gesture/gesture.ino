#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

const int SAMPLE_RATE_HZ = 50;
const int WINDOW_DURATION_MS = 2000;
const int SAMPLES_PER_WINDOW = (SAMPLE_RATE_HZ * WINDOW_DURATION_MS) / 1000; // 100
const int SAMPLE_INTERVAL_MS = 1000 / SAMPLE_RATE_HZ; // 20ms

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found");
    while (1) delay(10);
  }
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  Serial.println("READY");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    
    if (cmd == "RECORD") {
      Serial.println("START");
      unsigned long startTime = millis();
      
      for (int i = 0; i < SAMPLES_PER_WINDOW; i++) {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        
        Serial.print(a.acceleration.x, 3); Serial.print(",");
        Serial.print(a.acceleration.y, 3); Serial.print(",");
        Serial.print(a.acceleration.z, 3); Serial.print(",");
        Serial.print(g.gyro.x, 3); Serial.print(",");
        Serial.print(g.gyro.y, 3); Serial.print(",");
        Serial.println(g.gyro.z, 3);
        
        unsigned long nextSampleTime = startTime + (i + 1) * SAMPLE_INTERVAL_MS;
        while (millis() < nextSampleTime) {}
      }
      
      Serial.println("END");
    }
  }
}