#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <ArduinoJson.h>

// Пины радиомодуля для ESP32
#define CE_PIN 27
#define CSN_PIN 26
#define SCK_PIN 14
#define MISO_PIN 12
#define MOSI_PIN 13

RF24 radio(CE_PIN, CSN_PIN);
const uint64_t address = 0xF0F0F0F0E1LL;

// Компактная бинарная структура для радиоэфира (6 байт)
struct __attribute__((packed)) RadioPacket {
  uint8_t j1;
  uint8_t j2;
  uint8_t j3;
  uint8_t j4;
  uint8_t j5;
  uint8_t gripper;
};
// Стартовые углы
RadioPacket packet = {90, 90, 90, 90, 90, 45};

void TaskRadioMaster(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    if (Serial.available() > 0) {
      String receivedData = Serial.readStringUntil('\n');

      // Парсим JSON от Raspberry Pi
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, receivedData);

      if (!error) {
        // Если в JSON есть данные оси, обновляем структуру
        if (doc["base"].is<int>()) packet.j1 = doc["base"];
        if (doc["shoulder"].is<int>()) packet.j2 = doc["shoulder"];
        if (doc["elbow"].is<int>()) packet.j3 = doc["elbow"];
        if (doc["forearm"].is<int>()) packet.j4 = doc["forearm"];
        if (doc["wrist"].is<int>()) packet.j5 = doc["wrist"];
        if (doc["gripper"].is<int>()) packet.gripper = doc["gripper"];
        // В будущем добавим сюда остальные оси

        // Отправляем 6-байтовый пакет в эфир
        bool success = radio.write(&packet, sizeof(packet));
        if (success) {
          Serial.println("[ESP32] RADIO_TX_OK");
        } else {
          Serial.println("[ESP32] RADIO_TX_FAIL");
        }
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS); // Отдаем время ядру
  }
}

void setup() {
  Serial.begin(115200);

  // Переназначаем аппаратную шину SPI на твои пины
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CSN_PIN);

  if (!radio.begin()) {
    Serial.println("[ESP32] ОШИБКА: Радиомодуль не найден!");
    while (1) {}
  }

  radio.setPALevel(RF24_PA_MIN); // Для тестов на столе (минимум помех)
  radio.setChannel(115);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address);
  radio.stopListening(); // ESP32 работает ТОЛЬКО на передачу

  // Запуск задачи FreeRTOS на ядре 1
  xTaskCreatePinnedToCore(TaskRadioMaster, "RadioMaster", 4096, NULL, 1, NULL, 1);
}

void loop() {}