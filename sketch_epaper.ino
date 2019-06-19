#include "it8951.h"

IT8951 epd(GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27);

void setup() {
  Serial.begin(115200);
  Serial.println("Boot");
  Serial.println("Init");
  epd.begin();
}

void loop() {
  Serial.print("width ");
  Serial.println(epd.width());
  Serial.println("Sleep 5s");
  esp_sleep_enable_timer_wakeup(5000000);
  if (esp_light_sleep_start() != ESP_OK){
     Serial.println("Delay 5s");
     delay(5000000);
  }
}
