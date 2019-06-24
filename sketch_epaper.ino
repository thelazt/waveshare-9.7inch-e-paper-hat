#include "it8951.h"

IT8951 epd(GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27);

uint16_t test[1000] = { 0 };

void setup() {
  Serial.begin(115200);
  Serial.println("Boot");
  Serial.println("Init");

  epd.begin(4000000);
  Serial.println("Ready");
  //Serial.println(epd.display(0, 0, UINT16_MAX, UINT16_MAX, 0));
  //Serial.println(epd.display());
  for (int i = 0; i < 1000; i++)
    test[i] = 0x00ff;

  epd.clear();
  Serial.println("Cleared");
}

int x = 0;
int y = 0;
int n = 0;
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
  Serial.print(x);  
  Serial.print(", ");  
  Serial.print(y);  
  Serial.println(" Load");  
  Serial.println(epd.load(test, 1000, x, y, 50, 80));
Serial.println("Display");  
  if (n++ % 5 == 0)
    Serial.println(epd.display(0, 0, 2000, 2000));
 else
    Serial.println(epd.display(x,y, 50, 80));
  

  Serial.print("width ");
  Serial.println(epd.width());
  Serial.println("Sleep 5s");
  /* esp_sleep_enable_timer_wakeup(1000000);
  if (esp_light_sleep_start() != ESP_OK)*/{
     Serial.println("Delay 5s");
     delay(1000);
  }
}
