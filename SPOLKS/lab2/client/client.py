import socket
from commands import client_commands
import os
import os.path
import sys


import time
import re

PORT = 9001

WINDOW_SIZE = 4096

BUFFER_SIZE = 1024
TIMEOUT = 20

OK_STATUS = 200

UPLOAD_PROGRESS = 0
DOWNLOAD_PROGRESS = 0



def get_data():
    data, address = client.recvfrom(BUFFER_SIZE)
    data = data.decode('utf-8')
    return [data, address]

def send_data(data):
    client.sendto(str(data).encode('utf-8'), server_address)


def handle_input_request(request):
    command = request.split()
    name_command = command[0]

    if (len(command) == 2):
        body = command[1]

    if (client_commands.get(name_command) == "echo"):
        send_data(request)
        if (wait_for_ack(name_command) == False):
            return
        echo()

    if (client_commands.get(name_command) == "time"):
        send_data(request)
        if (wait_for_ack(name_command) == False):
            return
        get_time()

    if (client_commands.get(name_command) == "download"):
        send_data(request)
        if (wait_for_ack(name_command) == False):
            return
        download(body, request)

    if (client_commands.get(name_command) == "upload"):
        if (is_file_exist(body)):
            send_data(request)
            if (wait_for_ack(name_command) == False):
                return
            upload(body, request)
        else:
            show_error_message("No such file exists")


    if (client_commands.get(name_command) == "exit"):
        send_data(request)
        if (wait_for_ack(name_command) == False):
            return
        client.close()
        os._exit(1)

def wait_for_ack(command_to_compare):
    while True:
        response = get_data()[0].split(" ", 2)

        if not response:
            return False

        sent_request = response[0]
        status = response[1]

        if (len(response) > 2):
            message = response[2]
        else: message = None

        if (command_to_compare == sent_request and int(status) == OK_STATUS):
            return True
        elif (message):
            print(message)
            return False
        else:
            return False

def is_server_available(request, command):
    global client

    client.close()

    i = TIMEOUT

    while(i > 0):
        try:
            client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            client.connect(server_address)
            client.sendto(request.encode('utf-8'), server_address)
            wait_for_ack(command)
            return True

        except socket.error as er:
            sys.stdout.write("Waiting for a server: %d seconds \r" %i)
            sys.stdout.flush()

        i -= 1
        time.sleep(1)

    sys.stdout.flush()
    print("\nServer was disconnected")
    sys.stdout.flush()
    return False


def is_file_exist(file_name):
    return os.path.exists(file_name)

def echo():
    print(get_data()[0])

def get_time():
    print(get_data()[0])

def handle_disconnect():
    print("wait server")
    time.sleep(1)


def download(file_name, request):
    global WINDOW_SIZE

    send_data(WINDOW_SIZE)

    WINDOW_SIZE = int(get_data()[0])
    server_window = WINDOW_SIZE

    size = int(get_data()[0])

    send_data(DOWNLOAD_PROGRESS)

    data_size_recv = int(get_data()[0])

    if (data_size_recv == 0):
        f = open(file_name, "wb")
    else:
        f = open(file_name, "rb+")

    current_pos = data_size_recv

    i = 0
    progress_bar = 10


    while (1):
        try:

            client.settimeout(2)
            data = client.recvfrom(BUFFER_SIZE)[0]
            if data:
                if data == b'EOF':
                    break
                else:
                    i += 1
                    f.seek(current_pos, 0)
                    f.write(data)
                    current_pos += len(data)
                    server_window = server_window - len(data)
                    if (server_window == 0):
                        server_window = WINDOW_SIZE
                        send_data(current_pos)
            else:
                print("Server disconnected")
                return

            progress = (current_pos / size) * 100
            if (progress >= progress_bar):
                print("Download progress: %d%% " % progress)
                progress_bar += 10

        except KeyboardInterrupt:
            print("KeyboardInterrupt was handled")
            send_data("ERROR")
            f.close()
            client.close()
            os._exit(1)
        except client.timeout:
            print
            "Session Timed Out"
        except socket.timeout:
            print
            "Session Timed Out"
    f.close()
    print("\n" + file_name + " was downloaded")

def upload(file_name, request):
    global WINDOW_SIZE

    f = open (file_name, "rb+")

    size = int(os.path.getsize(file_name))

    send_data(WINDOW_SIZE)

    WINDOW_SIZE = int(get_data()[0])
    server_window = WINDOW_SIZE

    send_data(size)

    send_data(0)

    data_size_recv = int(get_data()[0])

    current_pos = data_size_recv

    f.seek(current_pos, 0)

    progress_bar = 10
    while (1):
        try:
            if (current_pos >= size):
                client.sendto(b"EOF", server_address)
                break
            else:
                data_file = f.read(BUFFER_SIZE)
                client.sendto(data_file, server_address)
                current_pos = current_pos + BUFFER_SIZE
                f.seek(current_pos)

            server_window = server_window - BUFFER_SIZE
            if (server_window == 0):
                server_window = WINDOW_SIZE
                send_data(current_pos)

            progress = (current_pos / size) * 100
            if (progress >= progress_bar):
                print("Upload progress: %d%% " % progress)
                progress_bar += 10

        except KeyboardInterrupt:
            print("KeyboardInterrupt handled")
            client.sendto(b"ERROR", server_address)
            f.close()
            client.close()
            os._exit(1)


    f.close()
    print("\n" + file_name + " was uploaded")


def exit():
    pass

def check_valid_request(request):
    command = request.split()
    if (len(command) == 0):
        return False
    else: return True

def show_status():
    pass

def show_error_message(error):
    print(error)

def show_start_message():
    print("\nWelcome to client cli!")

is_valid_address = False

REGULAR_IP = '^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$'
regex = re.compile(REGULAR_IP)


while (is_valid_address == False):
    addr = input("\nInput host addres: ")
    if (regex.match(addr)):
        is_valid_address = True
        HOST = addr
    else:
        try:
            HOST = socket.gethostbyname(addr)
            is_valid_address = True
        except socket.error:
            print("Please, input valid address")
            is_valid_address = False
        except client.timeout:
            print
            "Session Timed Out"

show_start_message()
server_address = (HOST, PORT)
client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client.connect(server_address)



while True:
    request = input()
    handle_input_request(request)
