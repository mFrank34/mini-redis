#!/usr/bin/env python3
import socket
import sys
import threading


class Client:
    def __init__(self, host="127.0.0.1", port=8080):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((host, port))
        self.running = True
        self.reader_thread = threading.Thread(target=self._read_loop, daemon=True)
        self.reader_thread.start()

    def _read_loop(self):
        buffer = ""
        while self.running:
            try:
                data = self.sock.recv(1024)
            except OSError:
                break
            if not data:
                print("\n[disconnected]")
                self.running = False
                break
            buffer += data.decode()
            while "\n" in buffer:
                line, buffer = buffer.split("\n", 1)
                if line:
                    print(f"\n{line}\n> ", end="", flush=True)

    def send(self, command: str):
        self.sock.sendall((command + "\n").encode())

    def close(self):
        self.running = False
        self.sock.close()


if __name__ == "__main__":
    client = Client(host="127.0.0.1", port=8080)
    try:
        while client.running:
            command = input("> ")
            if command == "exit":
                break
            client.send(command)
    except (EOFError, KeyboardInterrupt):
        pass
    finally:
        client.close()
