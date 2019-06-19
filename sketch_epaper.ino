#include "it8951.h"

IT8951 epd(GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27);

uint16_t test[1000] = { 0 };

void setup() {
  Serial.begin(115200);
  Serial.println("Boot");
  Serial.println("Init");

  epd.begin(4000000);

  Serial.println(epd.display(0, 0, UINT16_MAX, UINT16_MAX, 0));
  //Serial.println(epd.display());
  for (int i = 0; i < 1000; i++)
    test[i] = 0x00ff;

 // epd.clear(true, 200, 200, 100, 100);
}

int x = 0;
int y = 0;
void loop() {
  Serial.println("Looping");
  x += 50;
  if (x > 1150){
    x = 0;
    y+=80;
  }
  if (y > 745){
    y = 0;
  }
    
  Serial.println(epd.loadImage(test, 1000, x, y, 50, 80));
  Serial.println(epd.display(x, y, 50, 80, 3));
  

  Serial.print("width ");
  Serial.println(epd.width());
  Serial.println("Sleep 5s");
  esp_sleep_enable_timer_wakeup(5000000);
  if (esp_light_sleep_start() != ESP_OK){
     Serial.println("Delay 5s");
     delay(5000000);
  }
}
