import socket
from multiprocessing import Process, Lock, Queue
from datetime import datetime
import sys
import os
import os.path
import time
from commands import server_commands, client_commands, help_list

IP = ''

PORT = 9001
BUFFER_SIZE = 1024
NUM_OF_PROCESSES = 2

TIMEOUT = 20

OK_STATUS = 200
SERVER_ERROR = 500




def send_status_and_message(client, request, status, message):
    message = str("" + request + " " + str(status) + " " + message)
    client.send(message.encode('utf-8'))

def send_status(client, request, status):
    message = str("" + request + " " + str(status))
    client.send(message.encode('utf-8'))

def is_file_exist(file_name):
    return os.path.exists(file_name)



def child_process(process_id, server, mutex, process_queue):
    while True:
        process_queue.put("free")
        mutex.acquire()

        client, client_info = server.accept()
        process_queue.get()

        mutex.release()

        client_ip = client_info[0]
        client_port = client_info[1]

        print("[*] Accepted connection from: %s:%d by process %d" % (client_ip, client_port, process_id))

        client_obj = {
                        "socket": client,
                        "ip": client_ip,
                        "is_closed": False,
                        "port": client_port
                    }

        handle_client(client_obj)
        print("[*] Connection with: %s:%d closed by process %d" % (client_ip, client_port, process_id))

        if process_queue.qsize() > NUM_OF_PROCESSES:
            print("[*] Too many free processes,so %d kills itself" % (process_id))
            exit(0)


def handle_client(client):
    while True:
        if (client["is_closed"] == False):
            try:
                request = client['socket'].recv(BUFFER_SIZE).decode('utf-8')
                request = request.strip()
                if request != '':
                    print("[*] Received: %s" %request)
                    handle_client_request(client, request)
            except ConnectionResetError:
                client['socket'].close()
                return


def echo(client, body):
    time.sleep(0.01)
    send_data(client, body)

def send_time(client):
    server_time = "Server time: " + str(datetime.now())[:19]
    send_data(client, server_time)

def exit_client(client):
    clients_pool.remove(client)
    client['is_closed'] = True
    client['socket'].close()

def handle_client_request(client, request):
    command = request.split()
    name_command = command[0]

    if (len(command) == 2):
        body = command[1]

    if (client_commands.get(name_command) == "download"):
        if (is_file_exist(body)):
            send_status(client['socket'], name_command, OK_STATUS)
            download(client, body)
        else:
            no_file = "File: " + body + " is not exist."
            send_status_and_message(client['socket'], name_command, SERVER_ERROR, "No such file")

    elif (client_commands.get(name_command) == "echo"):
        send_status(client['socket'], name_command, OK_STATUS)
        echo(client, body)

    elif (client_commands.get(name_command) == "time"):
        send_status(client['socket'], name_command, OK_STATUS)
        send_time(client)

    elif (client_commands.get(name_command) == "exit"):
        send_status(client['socket'], name_command, OK_STATUS)
        exit_client(client)

    else:
        send_status_and_message(client['socket'], name_command, SERVER_ERROR, "Unknown command")


def check_client_available(client_ip, command):
    i = TIMEOUT
    while(i > 0):
        waiting_client = search_by_ip(clients_pool, client_ip)
        sys.stdout.write("Waiting for a client: %d seconds \r" %i)
        sys.stdout.flush()

        if(waiting_client):
            sys.stdout.flush()
            print("\nClient has returned")
            sys.stdout.flush()
            return

        i -= 1
        time.sleep(1)


    waiting_client = search_by_ip(waiting_clients, client_ip)
    if (len(waiting_clients) > 0 and waiting_client != False):
        waiting_clients.remove(waiting_client)

    if (command == "upload"):
        os.remove(waiting_client['file_name'])

    sys.stdout.flush()
    print("\nClient was disconnected")
    sys.stdout.flush()


def search_by_ip(list, ip):
    found_client = [element for element in list if element['ip'] == ip]
    return found_client[0] if len(found_client) > 0 else False

def search_by_socket(list, socket):
    found_client = [element for element in list if element['socket'] == socket]
    return found_client[0] if len(found_client) > 0 else False



def save_to_waiting_clients(ip, command, file_name, progress):
    waiting_clients.append(
        {
            'ip': ip,
            'command': command,
            'file_name': file_name,
            'progress': progress
        })

def handle_disconnect(client, command, file_name, progress):
    save_to_waiting_clients(client['ip'], command, file_name, progress)
    clients_pool.remove(client)
    client['socket'].close()
    check_client_available(client['ip'], command)



def wait_ok(client):
    while (client['socket'].recv(2).decode('utf-8') != "OK"):
        print("wait for OK")

def send_ok(client):
    client['socket'].send("OK".encode('utf-8'))

def get_data(client):
    return client['socket'].recv(BUFFER_SIZE).decode('utf-8')

def send_data(client, data):
    client['socket'].send(str(data).encode('utf-8'))

def download(client, file_name):
    f = open (file_name, "rb+")

    size = int(os.path.getsize(file_name))

    send_data(client, size)

    wait_ok(client)

    waiting_clients = []
    waiting_client = search_by_ip(waiting_clients, client['ip'])
    if (len(waiting_clients) > 0 and waiting_client != False):
        waiting_clients.remove(waiting_client)

    data_size_recv = int(get_data(client))

    if (waiting_client):
        if (waiting_client['file_name'] == file_name and waiting_client['command'] == 'download'):
            data_size_recv = int(waiting_client['progress'])
            send_data(client, data_size_recv)
    else:
        send_data(client, data_size_recv)

    wait_ok(client)

    f.seek(data_size_recv, 0)
    while (data_size_recv < size):
        try:
            data_file = f.read(BUFFER_SIZE)
            client['socket'].sendall(data_file)
            data_size_recv += BUFFER_SIZE
            f.seek(data_size_recv)

        except socket.error as e:
            f.close()
            handle_disconnect(client, "download", file_name, data_size_recv)
            client['is_closed'] = True
            return

        except KeyboardInterrupt:
            server.close()
            client.socket.close()
            os._exit(1)
    f.close()


if __name__ == '__main__':
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    server.bind((IP, PORT))
    server.listen(2)

    print("Hello, listened on %s:%d" % (IP, PORT))

    clients_pool = []
    waiting_clients = []

    process_queue = Queue()

    client_ID = 0

    mutex = Lock()

    num_of_processes = NUM_OF_PROCESSES

    for process_ID in range(NUM_OF_PROCESSES):
        client_handle = Process(target=child_process, args=(process_ID, server, mutex, process_queue))
        client_handle.start()

    while True:
        time.sleep(1)
        if (process_queue.empty()):
            client_handle = Process(target=child_process, args=(num_of_processes, server, mutex, process_queue))
            client_handle.start()
            print("[*] New process number %d has been created" % (num_of_processes))
            num_of_processes += 1
