#!/usr/bin/env python3

import socket
import time

# GET SAMPLE

def send_chunk(connection, data):
    chunk_size = f"{len(data):X}\r\n"
    full_message = chunk_size + data + "\r\n"
    connection.send(full_message.encode('utf-8'))

def send_chunked_request():
    # Replace with your server's address and port
    server_address = 'localhost'
    server_port = 8666

    # Create a socket connection to the server
    connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection.connect((server_address, server_port))

    # Headers and the first chunk
    headers = (
        "GET /chunking_data.txt HTTP/1.1\r\n"
        "Host: localhost:8666\r\n"
        "Accept-Encoding: identity\r\n"
        "Transfer-Encoding: chunked\r\n"
        "Content-Type: text/plain\r\n\r\n"
    )
    full_initial_message = headers + '0\r\n\r\n'
    connection.send(full_initial_message.encode('utf-8'))

    # Receive the server's response
    response = connection.recv(4096)
    print(response.decode('utf-8'))

    # Close the connection
    connection.close()

if __name__ == '__main__':
    send_chunked_request()