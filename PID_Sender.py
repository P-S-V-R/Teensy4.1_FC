import sys
import socket
from PyQt5.QtWidgets import (QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLineEdit, QPushButton, QLabel, QFormLayout, QScrollArea, QGroupBox)

class PIDSenderApp(QWidget):
    def __init__(self):
        super().__init__()
        self.data_fields = []  # Initialize data_fields before calling initUI
        self.initUI()
        self.my_socket = None

    def initUI(self):
        self.setWindowTitle('PID Sender')

        self.layout = QVBoxLayout()

        form_layout = QFormLayout()

        self.ip_input = QLineEdit(self)
        self.ip_input.setPlaceholderText('IP Address')
        form_layout.addRow('IP Address:', self.ip_input)

        self.port_input = QLineEdit(self)
        self.port_input.setPlaceholderText('Port')
        form_layout.addRow('Port:', self.port_input)

        self.variable_input_layout = QVBoxLayout()
        self.add_variable_input()

        add_variable_button = QPushButton('Add Variable', self)
        add_variable_button.clicked.connect(self.add_variable_input)
        form_layout.addRow(add_variable_button)

        self.scroll_area = QScrollArea()
        self.scroll_area.setWidgetResizable(True)
        self.scroll_content = QWidget()
        self.scroll_layout = QVBoxLayout(self.scroll_content)
        self.scroll_layout.addLayout(self.variable_input_layout)
        self.scroll_area.setWidget(self.scroll_content)

        self.layout.addLayout(form_layout)
        self.layout.addWidget(self.scroll_area)

        self.connect_button = QPushButton('Connect', self)
        self.connect_button.clicked.connect(self.connect_to_server)
        self.layout.addWidget(self.connect_button)

        self.send_button = QPushButton('Send', self)
        self.send_button.clicked.connect(self.send_pid_values)
        self.layout.addWidget(self.send_button)

        self.status_label = QLabel(self)
        self.layout.addWidget(self.status_label)

        self.setLayout(self.layout)

    def add_variable_input(self):
        layout = QHBoxLayout()
        name_input = QLineEdit(self)
        name_input.setPlaceholderText('Variable Name')
        value_input = QLineEdit(self)
        value_input.setPlaceholderText('Value')
        layout.addWidget(name_input)
        layout.addWidget(value_input)
        self.variable_input_layout.addLayout(layout)
        self.data_fields.append((name_input, value_input))

    def connect_to_server(self):
        try:
            ip = self.ip_input.text()
            port = int(self.port_input.text())
            self.my_socket = socket.socket()
            self.my_socket.connect((ip, port))
            self.status_label.setText(f"Connected to {ip} on port {port}")
        except Exception as e:
            self.status_label.setText(f"Error connecting to server: {e}")

    def send_pid_values(self):
        try:
            if not self.my_socket:
                self.status_label.setText("Not connected to server")
                return

            data = []
            for name_input, value_input in self.data_fields:
                name = name_input.text()
                value = float(value_input.text())
                data.append(f"{name}:{value}")

            data_string = ",".join(data)
            data_bytes = data_string.encode()

            # Calculate checksum
            checksum = sum(data_bytes) % 256

            # Construct the packet
            packet = bytearray()
            packet.append(0xFF)  # Start byte
            packet.extend(data_bytes)
            packet.append(checksum)
            packet.append(0x00)  # End byte

            # Send the packet
            self.my_socket.send(packet)
            self.status_label.setText("Message sent successfully")
        except Exception as e:
            self.status_label.setText(f"Error sending message: {e}")

    def closeEvent(self, event):
        if self.my_socket:
            self.my_socket.close()
        event.accept()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = PIDSenderApp()
    ex.show()
    sys.exit(app.exec_())
