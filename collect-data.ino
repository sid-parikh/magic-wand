#include <Adafruit_LSM6DSOX.h>

// Global variables
#define SAMPLES_PER_COLLECTION (50)
#define SAMPLE_DELAY_MS (10)  // 50 times per 0.5s sample

int collectionCounter = 0;

Adafruit_LSM6DSOX imu;

void setup(void) {
  // Initialize LED pin (built-in LED on the Feather)
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  while (!Serial)
    ;
  Serial.begin(115200);

  if (!imu.begin_I2C()) {
    Serial.println("Failed to connect to IMU!");
    for (;;) delay(1000);
  }

  // Setup banner
  Serial.println("Gesture Data Collection System - STM32F405 Feather");
  Serial.println("LED on = perform gesture, LED off = rest");
  Serial.println("Press enter to collect a sample. There will be a 1 second delay until collection starts. Samples are half a second long.");
  Serial.println("Each sample starts with \"Sample #:\"");
  Serial.println("And then is followed by 100 lines of \"[counter] [aX] [aY] [aZ] [gX] [gY] [gZ]\"");

  imu.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
  imu.setGyroRange(LSM6DS_GYRO_RANGE_125_DPS);
  imu.setAccelDataRate(LSM6DS_RATE_833_HZ);
  imu.setGyroDataRate(LSM6DS_RATE_833_HZ);

  Serial.println("System ready. Press Enter to start collection.");
}

sensors_event_t accel_buffer[SAMPLES_PER_COLLECTION];
sensors_event_t gyro_buffer[SAMPLES_PER_COLLECTION];
sensors_event_t dont_care;
void loop(void) {
  // Check if a sample should be collected based on serial input
  if (Serial.available() > 0) {
    // Clear the serial buffer
    while (Serial.available()) {
      Serial.read();
    }

    // Start the sample collection process
    Serial.println("Starting collection in 1 second...");
    delay(1000);  // Wait 1 second before collecting

    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    for (int i = 0; i < SAMPLES_PER_COLLECTION; ++i) {
      imu.getEvent(accel_buffer + i, gyro_buffer + i, &dont_care);
      delay(SAMPLE_DELAY_MS);
    }

    digitalWrite(LED_BUILTIN, LOW);

    Serial.print("Sample: ");
    Serial.println(collectionCounter);

    for (int i = 0; i < SAMPLES_PER_COLLECTION; ++i) {
      Serial.print(i);
      Serial.print(' ');
      Serial.print(accel_buffer[i].acceleration.x);
      Serial.print(' ');
      Serial.print(accel_buffer[i].acceleration.y);
      Serial.print(' ');
      Serial.print(accel_buffer[i].acceleration.z);
      Serial.print(' ');
      Serial.print(gyro_buffer[i].gyro.x);
      Serial.print(' ');
      Serial.print(gyro_buffer[i].gyro.y);
      Serial.print(' ');
      Serial.println(gyro_buffer[i].gyro.z);
    }
  }
}
