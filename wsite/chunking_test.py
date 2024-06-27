#!/usr/bin/env python3

import socket
import time

def send_chunked_request():
    # Replace with your server's address and port
    server_address = 'localhost'
    server_port = 8444

    # Create a socket connection to the server
    connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection.connect((server_address, server_port))

    # Function to send chunk
def send_chunk(connection, data):
    chunk_size = f"{len(data):X}\r\n"
    full_message = chunk_size + data + "\r\n"
    connection.send(full_message.encode('utf-8'))

	# Send initial headers
    initial_headers = (
		"POST /wsite HTTP/1.1\r\n"
		"Host: localhost:8444\r\n"
		"Accept-Encoding: identity\r\n"
		"Transfer-Encoding: chunked\r\n"
		"Content-Type: text/plain\r\n\r\n"
	)

	# Send the initial headers and the first chunk together
    first_chunk = f"chunk 1"
    full_initial_message = initial_headers + f"{len(first_chunk):X}\r\n" + first_chunk + "\r\n"
    connection.send(full_initial_message.encode('utf-8'))
    time.sleep(2)

	# Send 4 more chunks every 2 seconds
    for i in range(2, 6):
        send_chunk(connection, f"chunk {i}")
        time.sleep(2)

    # Send the final chunk of size 0 to indicate the end of the message
    connection.send(b'0\r\n\r\n')

    # Receive the server's response
    response = connection.recv(4096)
    print(response.decode('utf-8'))

    # Close the connection
    connection.close()

if __name__ == '__main__':
    send_chunked_request()
# def send_chunk(data):
# 	chunk_size = f"{len(data):X}\r\n"
# 	full_message = chunk_size + data + "\r\n"
# 	print(full_message)

# # Simulate sending initial headers and the first chunk together
# initial_headers = (
# 	"POST /wsite HTTP/1.1\r\n"
# 	"Host: localhost:8444\r\n"
# 	"Accept-Encoding: identity\r\n"
# 	"Transfer-Encoding: chunked\r\n"
# 	"Content-Type: text/plain\r\n\r\n"
# )

# first_chunk = "chunk 1"
# full_initial_message = initial_headers + f"{len(first_chunk):X}\r\n" + first_chunk + "\r\n"
# print(full_initial_message)
# time.sleep(2)

# # Simulate sending 4 more chunks every 2 seconds
# for i in range(2, 6):
# 	send_chunk(f"chunk {i}")
# 	time.sleep(2)

# # Simulate sending the zero-length chunk to indicate the end of the message
# print("0\r\n\r\n")