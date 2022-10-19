

#include <ESP8266WiFi.h>
#include <espnow.h>
#include<SoftwareSerial.h>

#define BUFFER_LENGTH 256
char msg[BUFFER_LENGTH]; 
SoftwareSerial s(3,1);// this pins are gpio pins of Rx and Tx 

typedef struct Data{
  int id;
  float ax;
  float ay;
  float az;
}Data;


struct Data d;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  char macStr[18];
//  Serial.print("Packet received from: ");
//  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
//           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
//  Serial.println(macStr);
  memcpy(&d, incomingData, sizeof(d));
}
 
void setup() {
  // Initialize Serial Monitor
  //Serial.begin(115200);
  s.begin(9600);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    //Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop(){
  // Access the variables for each board
  String ids =  (String) d.id;
  String AX =   (String) d.ax;
  String  AY =  (String) d.ay;
  String AZ =   (String) d.az;

   

  
snprintf(msg,BUFFER_LENGTH,"{\"id\":\"%s\",\"AX\":\"%s\",\"AY\":\"%s\",\"AZ\":\"%s\"}"
,ids.c_str(),AX.c_str(),AY.c_str(),AZ.c_str());
s.println(msg);

delay(5000);
  
}
