#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "gesture_model.h"

Adafruit_MPU6050 mpu;
Eloquent::ML::Port::RandomForest classifier; 

const int SAMPLES = 100;
const int FEATURES_PER_AXIS = 5; // mean, std, max, min, abs_max
const int NUM_AXES = 6;
const int NUM_FEATURES = NUM_AXES * FEATURES_PER_AXIS; // 30

float ax_buf[SAMPLES], ay_buf[SAMPLES], az_buf[SAMPLES];
float gx_buf[SAMPLES], gy_buf[SAMPLES], gz_buf[SAMPLES];

const char* LABELS[] = {"shake", "wave", "idle"};

float feat_mean(float* buf, int n) {
  float sum = 0;
  for (int i = 0; i < n; i++) sum += buf[i];
  return sum / n;
}

float feat_std(float* buf, int n, float mean) {
  float sum = 0;
  for (int i = 0; i < n; i++) sum += (buf[i] - mean) * (buf[i] - mean);
  return sqrt(sum / n);
}

float feat_max(float* buf, int n) {
  float m = buf[0];
  for (int i = 1; i < n; i++) if (buf[i] > m) m = buf[i];
  return m;
}

float feat_min(float* buf, int n) {
  float m = buf[0];
  for (int i = 1; i < n; i++) if (buf[i] < m) m = buf[i];
  return m;
}

float feat_abs_max(float* buf, int n) {
  float m = 0;
  for (int i = 0; i < n; i++) if (fabs(buf[i]) > m) m = fabs(buf[i]);
  return m;
}

void extract_features(float* features) {
  float* bufs[6] = {ax_buf, ay_buf, az_buf, gx_buf, gy_buf, gz_buf};
  int idx = 0;
  for (int a = 0; a < 6; a++) {
    float m = feat_mean(bufs[a], SAMPLES);
    features[idx++] = m;
    features[idx++] = feat_std(bufs[a], SAMPLES, m);
    features[idx++] = feat_max(bufs[a], SAMPLES);
    features[idx++] = feat_min(bufs[a], SAMPLES);
    features[idx++] = feat_abs_max(bufs[a], SAMPLES);
  }
}

void collect_window() {
  int sample_interval_ms = 20; // 50Hz
  for (int i = 0; i < SAMPLES; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    ax_buf[i] = a.acceleration.x;
    ay_buf[i] = a.acceleration.y;
    az_buf[i] = a.acceleration.z;
    gx_buf[i] = g.gyro.x;
    gy_buf[i] = g.gyro.y;
    gz_buf[i] = g.gyro.z;
    delay(sample_interval_ms);
  }
}

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

  Serial.println("SMASH ready. Send 'r' to record.");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'r') {
      Serial.println("Recording...");
      collect_window();

      float features[NUM_FEATURES];
      extract_features(features);

      int prediction = classifier.predict(features);
      Serial.println(classifier.idxToLabel(prediction));
    }
  }
}