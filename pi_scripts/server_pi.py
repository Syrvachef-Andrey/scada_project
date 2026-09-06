import paho.mqtt.client as mqtt
import serial
import time

try:
    esp32_serial = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
    time.sleep(2)
    print("Успешно: Порт ESP32 открыт!")
except Exception as e:
    print(f"Ошибка открытия порта ESP32")
    esp32_serial = None

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print("Успешно подключено к брокеру Mosquitto!")
        client.subscribe("scada/arm/angles")
    else:
        print(f"Ошибка подключения, код: {reason_code}")


def on_message(client, userdata, msg):
    payload_str = msg.payload.decode('utf-8')
    print(f"Получено от SCADA: {payload_str}")

    if esp32_serial:
        data_to_send = payload_str + '\n'
        esp32_serial.write(data_to_send.encode('utf-8'))
        print("Доставлено")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect("localhost", 1883, 60)

print("Server is started. Waiting command from SCADA")
client.loop_forever()
