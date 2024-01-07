#include <Arduino.h>
#include <esp_task_wdt.h>

TaskHandle_t Task1,Task2;
#define WDT_TIMEOUT 30
uint8_t taskTest=3;

#ifdef __cplusplus
  extern "C" {
    #endif
      uint8_t temprature_sens_read();
    #ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

void PrioriTask(void *pvParameters){
  uint8_t sch=1;
  uint16_t counter =1;
  vTaskDelay(1*1000);

  for(;;){
    esp_task_wdt_add(Task1); 
    esp_task_wdt_reset();
    vTaskDelay(sch*1000);

    Serial.printf("[PrioriTask]-%d\n",counter);
    counter++;

    if (taskTest==1){
      uint32_t i=1;
      while (true){
        Serial.printf(" ------------------------------------------------------------- [PrioriTask]-watchdog->%d [%d]\n",counter,i);
        vTaskDelay(1*1000);
        i++;
      }
    }

    esp_task_wdt_delete(Task1);
  }
}

void RutiniTask(void *pvParameters){
  uint8_t sch=2;
  uint16_t uptime=0;
  vTaskDelay(1*1000);

  uint32_t fh = ESP.getFreeHeap();
  float temp = (temprature_sens_read() - 32) / 1.8;
  Serial.printf("\t\t[RutiniTask]-up:%d second;temp:%.2f C;fh:%d byte\n",uptime,temp,fh);
  for(;;){
    esp_task_wdt_add(Task2); 
    esp_task_wdt_reset();
    vTaskDelay(sch*1000);

    uptime+=sch;
    temp = (temprature_sens_read() - 32) / 1.8;
    fh = ESP.getFreeHeap();
    Serial.printf("\t\t[RutiniTask]-up:%d second;temp:%.2f C;fh:%d byte\n",uptime,temp,fh);

    if (taskTest==2){
      uint32_t i=1;
      while (true){
        Serial.printf(" ------------------------------------------------------------- [RutiniTask]-watchdog->%d [%d]\n",uptime,i);
        vTaskDelay(1*1000);
        i++;
      }
    }

    esp_task_wdt_delete(Task2);
  }
}

void setup() {
  Serial.begin(115200,SERIAL_8N1);
  delay(1000);
  Serial.println(" ========= [WatchDog Test] ========= ");

  if (esp_task_wdt_init(WDT_TIMEOUT, true)==ESP_OK){  //enable panic so ESP32 restarts
    Serial.println("Enable esp_task_wdt");
  } 

  xTaskCreatePinnedToCore(
    PrioriTask, /* Function to implement the task */
    "Task1", /* Name of the task */
    10000,  /* Stack size in words */
    NULL,  /* Task input parameter */
    1,  /* Priority of the task */
    &Task1,  /* Task handle. */
    0); /* Core where the task should run */

  xTaskCreatePinnedToCore(
    RutiniTask, /* Function to implement the task */
    "Task2", /* Name of the task */
    10000,  /* Stack size in words */
    NULL,  /* Task input parameter */
    1,  /* Priority of the task */
    &Task2,  /* Task handle. */
    1); /* Core where the task should run */

  Serial.println("========= RUN =========");
}

uint8_t looping=1;
void loop() {
  esp_task_wdt_add(nullptr); 
  esp_task_wdt_reset();

  Serial.printf("\t\t\t[looping]-%d\n",looping);
  looping++;
  delay(1000);

  if (taskTest==3){
    uint32_t i=1;
    while (true){
      Serial.printf(" ------------------------------------------------------------- [looping]-watchdog->%d [%d]\n",looping,i);
      delay(1*1000);
      i++;
    }
  }

  esp_task_wdt_delete(nullptr);
}


