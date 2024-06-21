import sys
import socket
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLineEdit, QPushButton, QLabel, QFormLayout

class PIDSenderApp(QWidget):
    def __init__(self):
        super().__init__()
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

        self.pitch_p_input = QLineEdit(self)
        self.pitch_p_input.setPlaceholderText('Pitch P')
        form_layout.addRow('Pitch P:', self.pitch_p_input)

        self.pitch_i_input = QLineEdit(self)
        self.pitch_i_input.setPlaceholderText('Pitch I')
        form_layout.addRow('Pitch I:', self.pitch_i_input)

        self.pitch_d_input = QLineEdit(self)
        self.pitch_d_input.setPlaceholderText('Pitch D')
        form_layout.addRow('Pitch D:', self.pitch_d_input)

        self.roll_p_input = QLineEdit(self)
        self.roll_p_input.setPlaceholderText('Roll P')
        form_layout.addRow('Roll P:', self.roll_p_input)

        self.roll_i_input = QLineEdit(self)
        self.roll_i_input.setPlaceholderText('Roll I')
        form_layout.addRow('Roll I:', self.roll_i_input)

        self.roll_d_input = QLineEdit(self)
        self.roll_d_input.setPlaceholderText('Roll D')
        form_layout.addRow('Roll D:', self.roll_d_input)

        self.yaw_p_input = QLineEdit(self)
        self.yaw_p_input.setPlaceholderText('Yaw P')
        form_layout.addRow('Yaw P:', self.yaw_p_input)

        self.yaw_i_input = QLineEdit(self)
        self.yaw_i_input.setPlaceholderText('Yaw I')
        form_layout.addRow('Yaw I:', self.yaw_i_input)

        self.yaw_d_input = QLineEdit(self)
        self.yaw_d_input.setPlaceholderText('Yaw D')
        form_layout.addRow('Yaw D:', self.yaw_d_input)

        self.layout.addLayout(form_layout)

        self.connect_button = QPushButton('Connect', self)
        self.connect_button.clicked.connect(self.connect_to_server)
        self.layout.addWidget(self.connect_button)

        self.send_button = QPushButton('Send', self)
        self.send_button.clicked.connect(self.send_pid_values)
        self.layout.addWidget(self.send_button)

        self.status_label = QLabel(self)
        self.layout.addWidget(self.status_label)

        self.setLayout(self.layout)

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

            pitch_p = float(self.pitch_p_input.text())
            pitch_i = float(self.pitch_i_input.text())
            pitch_d = float(self.pitch_d_input.text())
            roll_p = float(self.roll_p_input.text())
            roll_i = float(self.roll_i_input.text())
            roll_d = float(self.roll_d_input.text())
            yaw_p = float(self.yaw_p_input.text())
            yaw_i = float(self.yaw_i_input.text())
            yaw_d = float(self.yaw_d_input.text())

            # Create the data string
            data = f"pitch_p:{pitch_p},pitch_i:{pitch_i},pitch_d:{pitch_d},roll_p:{roll_p},roll_i:{roll_i},roll_d:{roll_d},yaw_p:{yaw_p},yaw_i:{yaw_i},yaw_d:{yaw_d}"
            data_bytes = data.encode()

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
