
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;


// REPLACE WITH RECEIVER MAC Address
//40:91:51:50:DA:38
//40:91:51:50:DA:38
//capstone reviver mac adress :94:B9:7E:14:53:D3

uint8_t broadcastAddress[] = {0x94, 0xB9, 0x7E, 0x14, 0x53, 0xD3};
// Structure example to send data
// Must match the receiver structure

typedef struct Data {
  int id;
  float ax;
  float ay;
  float az;
} Data;


struct Data d;


unsigned long lastTime = 0;
unsigned long timerDelay = 60000;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("\r\nLast Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  }
  else {
    Serial.println("Delivery fail");
  }
}

void initmpu() {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  //Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }
  Serial.println("");
  delay(100);

}
void setup() {

  //INTILIAZING THE SESNOR DATA
  Serial.begin(115200);
  initmpu();
  // Init Serial Monitor
  // Serial.begin(115200);


  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Set ESP-NOW role
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Once ESPNow is successfully init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

}



void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // Set values to send
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    float AX;
    float AY;
    float AZ;
    Serial.print("Acceleration X: ");
    Serial.print(a.acceleration.x);
    Serial.print(", Y: ");
    Serial.print(a.acceleration.y);
    Serial.print(", Z: ");
    Serial.print(a.acceleration.z);
    Serial.println(" m/s^2");
    AX = float(a.acceleration.x);
    AY = float(a.acceleration.y);
    AZ = float(a.acceleration.z);
    //Serial.println(AX);
    d.id = 1;
    d.ax = AX;
    d.ay = AY;
    d.az = AZ;
    // Send message via ESP-NOW
    esp_now_send(0, (uint8_t *) &d, sizeof(d));
    lastTime = millis();
  }
}
