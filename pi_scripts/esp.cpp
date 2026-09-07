#include <Arduino.h>

// Задача FreeRTOS для чтения данных
void TaskReadSerial(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    // Если в USB порт пришли данные от малинки
    if (Serial.available() > 0) {
      String receivedData = Serial.readStringUntil('\n');

      // Эхо-ответ обратно на малинку для проверки
      Serial.print("[ESP32 FreeRTOS] Принял пакет: ");
      Serial.println(receivedData);

      // На следующем этапе мы будем отправлять этот пакет по радиомодулю
    }
    // Отдаем процессорное время другим задачам (важно для FreeRTOS)
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);

  // Создаем задачу на ядре 1
  xTaskCreatePinnedToCore(
    TaskReadSerial,   // Функция задачи
    "SerialRead",     // Имя задачи
    4096,             // Размер стека
    NULL,             // Параметры
    1,                // Приоритет (1 - базовый)
    NULL,             // Дескриптор задачи
    1                 // Номер ядра
  );
}

void loop() {
  // В архитектуре FreeRTOS стандартный loop остается пустым
  // Все процессы разбиты на независимые задачи (Tasks)
}