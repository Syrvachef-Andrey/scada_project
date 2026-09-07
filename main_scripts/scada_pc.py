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
        self.setGeometry(500, 500, 350, 350)

        self.mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self.mqtt_client.connect(BROKER_ADDRESS, 1883, 60)
        self.mqtt_client.loop_start()

        self.list_of_names_of_angles = ['base', 'shoulder', 'elbow', 'forearm', 'wrist', 'gripper']
        self.angles = {
            "base": 90, "shoulder": 90, "elbow": 90,
            "forearm": 90, "wrist": 90, "gripper": 90
        }

        self.labels = {}

        widget = QWidget()
        layout = QVBoxLayout()

        for i, axis_name in enumerate(self.list_of_names_of_angles):
            lbl = QLabel(f"Ось {i + 1} ({axis_name}): 90°")
            self.labels[axis_name] = lbl
            layout.addWidget(lbl)

            slider = QSlider(Qt.Horizontal)
            slider.setMinimum(0)
            slider.setMaximum(180)
            slider.setValue(90)

            slider.valueChanged.connect(
                lambda val, name=axis_name, idx=i: self.on_slider_change(val, name, idx)
            )
            layout.addWidget(slider)

        widget.setLayout(layout)
        self.setCentralWidget(widget)

    def on_slider_change(self, value, axis_name, idx):
        self.labels[axis_name].setText(f"Ось {idx + 1} ({axis_name}): {value}°")

        self.angles[axis_name] = value

        self.mqtt_client.publish("scada/arm/angles", json.dumps(self.angles))


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = ScadaWindow()
    window.show()
    sys.exit(app.exec_())