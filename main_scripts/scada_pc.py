import json
import paho.mqtt.client as mqtt
import os
import sys
import PyQt5
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget, QSlider, QLabel
from PyQt5.QtCore import Qt

pyqt_path = os.path.dirname(PyQt5.__file__)
plugin_path = os.path.join(pyqt_path, 'Qt5', 'plugins', 'platforms')
os.environ['QT_QPA_PLATFORM_PLUGIN_PATH'] = plugin_path

BROKER_ADDRESS = "192.168.0.100"


class ScadaWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("SCADA: Robot Control")
        self.setGeometry(100, 100, 350, 120)

        self.mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self.mqtt_client.connect(BROKER_ADDRESS, 1883, 60)
        self.mqtt_client.loop_start()

        widget = QWidget()
        layout = QVBoxLayout()

        self.lbl_axis = QLabel("Ось 1 (Base): 90°")
        layout.addWidget(self.lbl_axis)

        self.slider = QSlider(Qt.Horizontal)
        self.slider.setMinimum(0)
        self.slider.setMaximum(180)
        self.slider.setValue(90)
        self.slider.valueChanged.connect(self.on_slider_change)
        layout.addWidget(self.slider)

        widget.setLayout(layout)
        self.setCentralWidget(widget)

    def on_slider_change(self, value):
        self.lbl_axis.setText(f"Ось 1 (Base): {value}°")
        # Формируем JSON пакет
        payload = {"base": value, "shoulder": 90}
        self.mqtt_client.publish("scada/arm/angles", json.dumps(payload))


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = ScadaWindow()
    window.show()
    sys.exit(app.exec_())