#include <Servo.h>
#include <SPI.h>
#include <RF24.h>

Servo servo1_base;
Servo servo2_shoulder;
Servo servo3_elbow;
Servo servo4_forearm;
Servo servo5_wrist;
Servo servo6_gripper;

RF24 radio(10, 9); // CE = 10, CSN = 9
const uint64_t address = 0xF0F0F0F0E1LL;

struct __attribute__((packed)) RadioPacket {
  uint8_t j1;
  uint8_t j2;
  uint8_t j3;
  uint8_t j4;
  uint8_t j5;
  uint8_t gripper;
};
RadioPacket packet;

void setup() {
  Serial.begin(115200);

  servo1_base.attach(3);
  servo2_shoulder.attach(4);
  servo3_elbow.attach(5);
  servo4_forearm.attach(6);
  servo5_wrist.attach(7);
  servo6_gripper.attach(8);

  if (!radio.begin()) {
    Serial.println("ОШИБКА: Радиомодуль RX не найден!");
    while (1) {}
  }

  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(115);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, address);

  radio.startListening();

  Serial.println("Arduino готова к приему команд!");
}

void loop() {
  if (radio.available()) {
    radio.read(&packet, sizeof(packet));

    servo1_base.write(constrain(packet.j1, 0, 180));
    servo2_shoulder.write(constrain(packet.j2, 0, 180));
    servo3_elbow.write(constrain(packet.j3, 0, 180));
    servo4_forearm.write(constrain(packet.j4, 0, 180));
    servo5_wrist.write(constrain(packet.j5, 0, 180));
    servo6_gripper.write(constrain(packet.gripper, 0, 90));

    Serial.print("RX OK! Base: ");
    Serial.print(packet.j1);
    Serial.print(" Shoulder: ");
    Serial.println(packet.j2);
  }
}