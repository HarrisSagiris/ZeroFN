import socket
import threading
import json
import time
from datetime import datetime

class FortniteServer:
    def __init__(self):
        self.host = '127.0.0.1'
        self.port = 7777
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.bind((self.host, self.port))
        self.server.listen(5)
        print(f'[INFO] Server listening on {self.host}:{self.port}')

    def handle_client(self, client):
        while True:
            try:
                data = client.recv(1024).decode()
                if not data:
                    break
                response = self.generate_response()
                client.send(response.encode())
            except:
                break
        client.close()

    def generate_response(self):
        return json.dumps({
            "status": "success",
            "accountId": "zerofn_player",
            "displayName": "ZeroFN Player",
            "token": "zerofn_token"
        })

    def start(self):
        while True:
            client, addr = self.server.accept()
            print(f'[INFO] Client connected from {addr[0]}:{addr[1]}')
            client_thread = threading.Thread(target=self.handle_client, args=(client,))
            client_thread.start()

if __name__ == '__main__':
    server = FortniteServer()
    server.start()
