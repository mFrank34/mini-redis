#!/usr/bin/env python3
import socket

def send_command(command: str, host="127.0.0.1", port=6379) -> str:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((host, port))
        s.sendall(command.encode())
        response = s.recv(1024).decode()
    return response

if __name__ == "__main__":
    while True:
        command = input("> ")
        if command == "exit":
            break
        response = send_command(command, host="127.0.0.1", port=8080)
        print(response)