#!/usr/bin/env python3

import http.client
import time

def send_chunked_request():
    # Replace with your server's address and port
    server_address = '127.0.0.1'
    server_port = 8000

    # Create a connection to the server
    connection = http.client.HTTPConnection(server_address, server_port)

    # Send the initial request with chunked transfer encoding
    headers = {
        'Transfer-Encoding': 'chunked',
        'Content-Type': 'text/plain'
    }
    connection.putrequest('POST', '/wsite')
    for key, value in headers.items():
        connection.putheader(key, value)
    connection.endheaders()

    # Function to send a chunk
    def send_chunk(data):
        chunk_size = f"{len(data):X}\r\n"
        connection.send(chunk_size.encode('utf-8'))
        connection.send(data.encode('utf-8'))
        connection.send(b'\r\n')

    # Send 5 chunks every 2 seconds
    for i in range(5):
        send_chunk(f"chunk {i + 1}")
        time.sleep(2)

    # Send the final chunk of size 0 to indicate the end of the message
    connection.send(b'0\r\n\r\n')

    # Get the server's response
    response = connection.getresponse()
    print(response.status, response.reason)
    print(response.read().decode('utf-8'))

    # Close the connection
    connection.close()

if __name__ == '__main__':
    send_chunked_request()