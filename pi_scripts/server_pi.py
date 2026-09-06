import paho.mqtt.client as mqtt


def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print("Успешно подключено к брокеру Mosquitto!")
        client.subscribe("scada/arm/angles")
    else:
        print(f"Ошибка подключения, код: {reason_code}")


def on_message(client, userdata, msg):
    payload_str = msg.payload.decode('utf-8')
    print(f"Получено от SCADA: {payload_str}")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect("localhost", 1883, 60)

print("Server is started. Waiting command from SCADA")
client.loop_forever()
