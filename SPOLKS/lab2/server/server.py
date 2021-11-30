import socket
import threading
from datetime import datetime
from time import time
import os
import os.path
import time
from commands import client_commands
from server_cli import server_cli


IP = "0.0.0.0"
PORT = 9001
PORT_TCP = 9002
BUFFER_SIZE = 1024
WINDOW_SIZE = 4096

TIMEOUT = 20

OK_STATUS = 200
SERVER_ERROR = 500


def is_file_exist(file_name):
    return os.path.exists(file_name)

def echo(addr, body):
    time.sleep(0.001)
    send_data(addr, body)

def send_time(addr):
    server_time = "Server time: " + str(datetime.now())[:19]
    send_data(addr, server_time)

def exit_client(addr):
    clients_addr.remove(addr)

def save_to_waiting_clients(addr, command, file_name, progress):
    waiting_clients.append(
        {
            'addr': addr[0],
            'command': command,
            'file_name': file_name,
            'progress': progress
        })


def search_by_addr(list, addr):
    found_client = [element for element in list if element['addr'] == addr[0]]
    return found_client[0] if len(found_client) > 0 else False

def handle_disconnect(client, command, file_name, progress):
    save_to_waiting_clients(addr, command, file_name, progress)
    time.sleep(1)
    print("lost connection")

def download(addr, file_name):
    global WINDOW_SIZE

    f = open(file_name, "rb+")

    size = int(os.path.getsize(file_name))

    print("File size: %f" % (size / (1024 * 1024)))

    client_window = int(get_data()[0])

    if (WINDOW_SIZE > client_window):
        WINDOW_SIZE = client_window

    send_data(addr, WINDOW_SIZE)

    send_data(addr, size)

    data_size_recv = int(get_data()[0])

    waiting_client = search_by_addr(waiting_clients, addr)
    if (len(waiting_clients) > 0 and waiting_client != False and waiting_client["file_name"] == file_name and
                waiting_client['command'] == 'download'):
        waiting_clients.remove(waiting_client)
        data_size_recv = int(waiting_client['progress'])

    send_data(addr, data_size_recv)

    f.seek(data_size_recv, 0)

    current_pos = data_size_recv

    time_start = time.time()

    while (1):
        try:
            if (current_pos >= size):
                server.sendto(b"EOF", addr)
                break
            else:
                data_file = f.read(BUFFER_SIZE)
                server.sendto(data_file, addr)
                current_pos = current_pos + BUFFER_SIZE
                f.seek(current_pos)

            client_window = client_window - BUFFER_SIZE
            if (client_window == 0):

                received_data = get_data()[0]
                client_window = WINDOW_SIZE

                if (received_data == "ERROR"):
                    handle_disconnect(addr, "download", file_name, data_size_recv)
                    break
                else:
                    data_size_recv = int(received_data)



        except KeyboardInterrupt:
            f.close()
            server.close()
            os._exit(1)

    time_end = time.time()

    print(time_start, time_end)
    delta_time = (time_end - time_start)

    print("Total time: %f ms" %delta_time)

    speed = (size/1024**2)/delta_time

    print("Average speed: %f m/s" % speed)

    f.close()

def upload(addr, file_name):
    global WINDOW_SIZE

    client_window = int(get_data()[0]) #1

    if (WINDOW_SIZE > client_window):
        WINDOW_SIZE = client_window

    send_data(addr, WINDOW_SIZE) #2

    size = int(get_data()[0]) #3

    data_size_recv = get_data()[0] #4

    if (data_size_recv):
        data_size_recv = int(data_size_recv)

    waiting_client = search_by_addr(waiting_clients, addr)
    if (len(waiting_clients) > 0 and waiting_client != False and waiting_client["file_name"] == file_name and waiting_client['command'] == 'upload'):
        waiting_clients.remove(waiting_client)
        data_size_recv = int(waiting_client['progress'])

    send_data(addr, data_size_recv) #5

    if (data_size_recv == 0):
        f = open(file_name, "wb")
    else:
        f = open(file_name, "rb+")


    current_pos = data_size_recv

    f.seek(current_pos, 0)

    time_start = time.time()

    while (1):
        try:

            data = server.recvfrom(BUFFER_SIZE)[0]

            if data:
                if data == b"ERROR":
                    handle_disconnect(addr, "upload", file_name, data_size_recv)
                    break

                if data == b"EOF":
                    break
                else:
                    f.seek(current_pos, 0)
                    f.write(data)
                    current_pos += len(data)
                    client_window = client_window - len(data)
                    if (client_window == 0):
                        client_window = WINDOW_SIZE

                        received_data = get_data()[0]

                        if (received_data == "ERROR"):
                            handle_disconnect(addr, "upload", file_name, data_size_recv)
                            break
                        else:
                            data_size_recv = int(received_data)

        except KeyboardInterrupt:
            f.close()
            server.close()
            os._exit(1)

    time_end = time.time()

    delta_time = (time_end - time_start)

    print("Total time: %f ms" %delta_time)

    speed = (size/1024**2)/delta_time

    print("Average speed: %f m/s" % speed)

    f.close()


def add_client_address(addr):
    if not addr in clients_addr:
        clients_addr.append(addr)
        print("Accepted client", addr)

def get_data():
    data, address = server.recvfrom(BUFFER_SIZE)
    data = data.decode('utf-8')
    return [data, address]

def send_data(addr, data):
    server.sendto(str(data).encode('utf-8'), addr)

def send_status_and_message(addr, request, status, message):
    message = str("" + request + " " + str(status) + " " + message)
    send_data(addr, message)

def send_status(addr, request, status):
    message = str("" + request + " " + str(status))
    send_data(addr, message)


def handle_client_request(addr, request):
    command = request.split()
    name_command = command[0]

    if (len(command) == 2):
        body = command[1]

    if (client_commands.get(name_command) == "download"):
        if (is_file_exist(body)):
            send_status(addr, name_command, OK_STATUS)
            download(addr, body)
        else:
            no_file = "File: " + body + " is not exist."
            send_status_and_message(addr, name_command, SERVER_ERROR, "No such file")


    elif (client_commands.get(name_command) == "upload"):
        send_status(addr, name_command, OK_STATUS)
        upload(addr, body)


    elif (client_commands.get(name_command) == "echo"):
        send_status(addr, name_command, OK_STATUS)
        echo(addr, body)

    elif (client_commands.get(name_command) == "time"):
        send_status(addr, name_command, OK_STATUS)
        send_time(addr)

    elif (client_commands.get(name_command) == "exit"):
        send_status(addr, name_command, OK_STATUS)
        exit_client(addr)

    else:
        send_status_and_message(addr, name_command, SERVER_ERROR, "Unknown command")


server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

server_address = (IP, PORT)
server.bind(server_address)

server_cli = threading.Thread(target=server_cli, args=(server, ))
server_cli.start()

clients_addr = []
waiting_clients = []
time.sleep(1)
print("Hello, listening on %s:%d(UDP)" % (IP, PORT))

while True:
        request, addr = get_data()

        add_client_address(addr)
        print("Received a command: ", request)
        handle_client_request(addr, request)
