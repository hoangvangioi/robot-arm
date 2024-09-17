import socket
import time


class BluetoothSocketClient:
    def __init__(self, address, port):
        self.address = address
        self.port = port
        self.socket = None
        self.connect()

    def connect(self):
        while True:
            try:
                self.socket = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
                self.socket.connect((self.address, self.port))
                print("Connected to Bluetooth device.")
                break
            except socket.error as e:
                print(f"Error while connecting: {e}")
                print("Attempting to reconnect...")
                time.sleep(0.1)

    def send_message(self, message):
        try:
            self.socket.send(message)
            # print(f"Sent: {message}")
        except socket.error as e:
            print(f"Error while sending message: {e}")

    def receive_message(self):
        try:
            data = self.socket.recv(1024)
            # print(f"Received: {data.decode()}")
            return data.decode()
        except socket.error as e:
            print(f"Error while receiving message: {e}")
            return None

    def close(self):
        if self.socket:
            self.socket.close()
            print("Disconnected from Bluetooth device.")
