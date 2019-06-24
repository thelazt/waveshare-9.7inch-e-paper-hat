#include "it8951.h"

IT8951 epd(GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27);

uint16_t test[1000] = { 0 };

void setup() {
  Serial.begin(115200);
  Serial.println("Boot");
  Serial.println("Init");

  epd.begin(4000000);
  Serial.print("Device Info:\n Panel ");
  Serial.print(epd.width());
  Serial.print(" x ");
  Serial.println(epd.height());
  Serial.print(" Address 0x");
  Serial.println(epd.defaultImageBuffer(), HEX);
  Serial.print(" Firmware: ");
  Serial.println(epd.getFW());
  Serial.print(" LUT: ");
  Serial.println(epd.getLUT());

  for (int i = 0; i < 1000; i++)
    test[i] = 0xaaaa;
  
  if (epd.active())
    Serial.println("active");
  if (epd.clear(false))
    Serial.println("cleared");
}

int x = 0;
int y = 0;
int n = 0;
void loop() {
  Serial.print("Loop ");
  Serial.println(n);
  if (!epd.active()){
    Serial.print("Reset ");
    epd.reset();
  }
  x += 50;
  if (x > 1150){
    x = 0;
    y+= 80;
  }
  if (y > 745){
    y = 0;
  }
  epd.waitForDisplay();  
  Serial.print("Load ");
  Serial.println(epd.load(test, 1000, x, y, 50, 80));
  epd.waitForDisplay();  
  if (n++ % 5 == 0){
    Serial.print("Full refresh ");
    Serial.println(epd.display(0, 0, 2000, 2000));
  } else {
    Serial.print("Partial refresh ");
    Serial.println(epd.display(x,y, 50, 80));
  }
  epd.waitForDisplay();  
  Serial.print("Sleep ");
  Serial.println(epd.sleep());
  Serial.println();
  /* esp_sleep_enable_timer_wakeup(1000000);
  if (esp_light_sleep_start() != ESP_OK)*/{
     delay(1000);
  }
}
