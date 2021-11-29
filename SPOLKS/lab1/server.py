import os
import socket
import datetime
import tqdm
import time

MAX_QUERY_SIZE = 1

SOCKET_PORT = 50015
SOCKET_HOST = '127.0.0.1'
CONNECTION_DATA = (SOCKET_HOST, SOCKET_PORT)
BUFFER_SIZE = 1024 * 32
SEPARATOR = "<SEPARATOR>"


class TCPServer:
    SERVER_STOPPED_MESSAGE = b'SERVER STOPPED!'  # b-префикс означает bytes строковый литерал
    LOG_FILE = 'server_log_{}.log'

    RECEIVE_BUFFER_SIZE = 1024
    TIMEOUT = 60

    LOG_DIR = 'logs'
    STORAGE_DIR = 'storage'
    LAST_IP = '-'
    LAST_ID = 0
    PREV_COMMAND = '-'
    PREV_FILE = '-'
    progress = '-'

    upload_recieved = 0
    upload_file_size = 0

    def __init__(self, host='', port=SOCKET_PORT, max_client_count=MAX_QUERY_SIZE, sock=None,
                 log_file=None):  # конструктор класса
        self.max_client_count = max_client_count
        self.host = host
        self.port = port
        self.server_address = (self.host, self.port)
        self.socket = sock

        self.log_file = log_file
        self.startLogging()

        self.progressBarActivated = False

        self.connections = []

    def startLogging(self):
        cur_dir = os.path.abspath(os.path.curdir)  # Получить абсолютный путь файла или каталога
        storage_path = os.path.join(cur_dir,
                                    self.STORAGE_DIR)  # правильно соединяет переданный путь cur_dir к одному или более компонентов пути *STORAGE_DIR
        log_path = os.path.join(cur_dir,
                                self.LOG_DIR)  # правильно соединяет переданный путь cur_dir к одному или более компонентов пути *LOG_DIR

        if not os.path.exists(storage_path):
            os.mkdir(storage_path)  # создает каталог с именем storage_path

        if not os.path.exists(log_path):
            os.mkdir(log_path)  # создает каталог с именем log_path

        log_file = os.path.join(
            log_path,
            self.LOG_FILE.format(datetime.datetime.now().strftime('%d.%m.%Y__%H.%M.%S'))
        )

        if not self.log_file or self.log_file.closed:
            self.log_file = open(log_file, 'w', encoding="utf-8")

        self.log('server created')
        self.log('server storage path {}'.format(storage_path))
        self.log('server log path {}'.format(log_path))

        hostname = socket.gethostname()
        ip_address = socket.gethostbyname(hostname)

        self.log('server ip address: port = {}:{}'.format(ip_address, self.port))
        self.log_file.close()
        self.LOG_FILE = log_file

    def socketOpen(self):
        self.socket.listen(
            self.max_client_count)  # подготавливает сокет для приема соединений, означает максимальное количество подключений, которые операционная система может поставить в очередь для этого сокета
        self.log('open socket for {} clients'.format(self.max_client_count))

    def createSocket(self):
        sock = socket.socket(socket.AF_INET,
                             socket.SOCK_STREAM)  # создать TCP-сокет семейства AF_INET типа потоковый сокет
        #  устанавливает значение опции сокета
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
        sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPIDLE, 8)
        # Время (в секундах) простоя (idle) соединения, по прошествии которого TCP начнёт отправлять проверочные пакеты (keepalive probes), если для сокета включён параметр SO_KEEPALIVE
        sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPINTVL, 2)
        # Время в секундах между отправками отдельных проверочных пакетов (keepalive probes).
        sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPCNT, 10)
        # Максимальное число проверок (keepalive probes) TCP, отправляемых перед сбросом соединения.

        sock.bind(self.server_address)  # bind () используется, когда сокет необходимо сделать сокетом сервера

        self.log('create socket {}'.format(sock))

        return sock

    def clientWait(self):
        # print("wait")
        conn, addr = self.socket.accept()  # Метод Socket.accept() принимает соединение. Сокет должен быть привязан к адресу и прослушивать соединения
        self.connections.append((conn, addr))
        self.log('new client connected {}'.format(addr))
        c_id = int(conn.recv(10))
        conn.send(b'Start')
        # print("c_id", c_id)
        return conn, addr, c_id

    def clientProcessing(self, connection, addr, c_id):
        hostname = socket.gethostname()

        while True:
            data = connection.recv(self.RECEIVE_BUFFER_SIZE)
            if not data:
                # print("not data")
                return

            command, *params = data.split(b' ')  # разбивает строку на части
            self.log('client {} send command {} with params {}'.format(addr, command, params))

            if command == b'ping':
                connection.send(b'ping')
            elif command == b'cont':
                if addr[0] == self.LAST_IP and self.LAST_ID == c_id:
                    if self.PREV_COMMAND == 'U':
                        self.upload_file(connection, self.PREV_FILE, 1)
                    elif self.PREV_COMMAND == 'D':
                        self.download_file(connection, self.PREV_FILE, params[0].decode(encoding='utf-8'))
                self.LAST_IP = '-'
                self.LAST_ID = -1
            elif command == b'help':
                connection.send(b'''help - to see list of commands
                ping - test that the server is alive
                kill - to stop server
                echo - to resend message to a client
                upload - to upload file on the server `upload file_name_on_your_machine.extension`
                download - to download file from a server `download file_name_on_server`
                time - get server time
                ''')
            elif command == b'kill':
                connection.send(b'GoodBy my friend!')
                return -1
            elif command == b'echo':
                connection.send(b' '.join(params))
            elif command == b'upload':
                self.upload_file(connection, params[0].decode(encoding='utf-8'))
            elif command == b'download':
                self.download_file(connection, params[0].decode(encoding='utf-8'))
            elif command == b'time':
                connection.send(str(datetime.datetime.now().time()).encode(encoding='utf-8'))
            else:
                connection.send(b'unknown command, please try again')

    def closeConnection(self, connection):
        client = list(filter(lambda x: x[0] == connection, self.connections))[0]

        self.log('connection closed {}'.format(client[1]))
        self.connections.remove(client)
        try:
            client.send(b'connection closed press enter')
        except Exception as e:
            pass

    def serverStart(self):
        os.chdir(self.STORAGE_DIR)  # изменяем текущий рабочий каталог
        self.log('server started')

        while True:
            try:
                conn, addr, c_id = self.clientWait()
                action = self.clientProcessing(connection=conn, addr=addr, c_id=c_id)
                if action == -1:
                    return

                self.closeConnection(conn)
            except ConnectionResetError as e:
                self.log(str(e))
                self.LAST_IP = addr[0]
                self.LAST_ID = c_id
                if self.progressBarActivated:
                    self.progress.close()
                    self.progressBarActivated = False
                self.closeConnection(conn)
            except Exception as e:
                self.log(str(e))
                self.LAST_IP = addr[0]
                self.LAST_ID = c_id
                if self.progressBarActivated:
                    self.progress.close()
                    self.progressBarActivated = False
                self.closeConnection(conn)

    def log(self, message):
        if self.log_file.closed:
            self.log_file = open(self.LOG_FILE, 'a', encoding="utf-8")

        print('{}: {}'.format(datetime.datetime.now(), message))
        self.log_file.write('{}: {}\n'.format(datetime.datetime.now(), message))

    def stop(self):
        for conn, addr in self.connections:
            if not conn.close:
                conn.send(self.SERVER_STOPPED_MESSAGE)
                conn.close()
                self.log(f'{conn} closed by server')

        self.socket.close()

        self.log(f'socket closed')
        self.log(f'server stopped')

        self.log_file.close()

    def run(self):
        self.socket = self.socket if self.socket else self.createSocket()
        self.socketOpen()
        try:
            self.serverStart()
            self.stop()
        except KeyboardInterrupt as e:
            self.log(str(e))
            self.stop()

    def recvall(self, sock, amount_to_read):
        n = 0
        data = bytearray()
        while n < amount_to_read:
            b = sock.recv(int(int(amount_to_read) - int(n)))
            if not b:
                # print('\nerror')
                # print(f"ERROR n in revall = {n}")
                raise ConnectionResetError("error in the recvall")
                return None
            n += len(b)
            data.extend(b)

        return data

    def upload_file(self, sock, file_name, pos=0):
        time.sleep(0.5)
        self.PREV_COMMAND = 'U'

        if pos == 0:
            mod = "wb"
            sock.send(b'Start')
            received = sock.recv(BUFFER_SIZE).decode()
            file_name, filesize = received.split(SEPARATOR)
            self.upload_file_size = int(filesize)
            self.PREV_FILE = file_name
        else:
            mod = "ab"
            msg = f'{str(self.upload_recieved)}'
            sock.send(bytes(msg, encoding='utf-8'))
            file_name = self.PREV_FILE
            filesize = self.upload_file_size

        file_name = os.path.basename(file_name)
        self.progress = tqdm.tqdm(range(int(filesize)), f"Progress of {file_name}:", unit="B", unit_scale=True,
                                  unit_divisor=1024)
        self.progressBarActivated = True
        self.progress.update(self.upload_recieved)
        if pos == 0:
            total_read = 0
            if int(filesize) >= BUFFER_SIZE:
                amount_to_read = BUFFER_SIZE
            else:
                amount_to_read = int(filesize)
        else:
            total_read = self.upload_recieved
            if int(filesize) - self.upload_recieved >= BUFFER_SIZE:
                amount_to_read = BUFFER_SIZE
            else:
                amount_to_read = int(filesize) - self.upload_recieved
        with open(file_name, mod) as f:
            while True:
                bytes_read = self.recvall(sock, amount_to_read)
                sock.send(b'Start')
                f.write(bytes_read)
                self.progress.update(len(bytes_read))
                total_read += len(bytes_read)
                self.upload_recieved = total_read
                if int(filesize) - total_read >= BUFFER_SIZE:
                    amount_to_read = BUFFER_SIZE
                else:
                    amount_to_read = int(filesize) - total_read
                if total_read == int(filesize):
                    self.progress.close()
                    self.progressBarActivated = False
                    print('All')
                    break
        self.PREV_COMMAND = '-'
        self.PREV_FILE = '-'
        self.upload_recieved = 0
        self.upload_file_size = 0
        f.close()

    def download_file(self, connection, params, pos=0):
        time.sleep(0.5)
        posit = int(pos)
        self.PREV_COMMAND = 'D'
        name_string = params
        if not os.path.isfile(name_string):
            print('File does not exist')
            filesize = '-'
            connection.send(f"{name_string}{SEPARATOR}{filesize}".encode())
            return

        filesize = os.path.getsize(name_string)
        self.PREV_FILE = name_string
        f = open(name_string, "rb")
        if pos == 0:
            connection.send(f"{name_string}{SEPARATOR}{filesize}".encode())
        else:
            self.progress.close()
            self.progressBarActivated = False
            f.seek(posit)
        self.progress = tqdm.tqdm(range(filesize), f"Progress of {name_string}:", unit="B", unit_scale=True,
                                  unit_divisor=1024)
        self.progressBarActivated = True
        self.progress.update(posit)

        read_amount = posit
        while 1:
            part = f.read(BUFFER_SIZE)
            connection.send(part)
            self.progress.update(len(part))
            read_amount += len(part)
            if read_amount == filesize:
                break

        self.progress.close()
        self.progressBarActivated = False
        print('All')
        self.PREV_COMMAND = '-'
        self.PREV_FILE = '-'
        f.close()


if __name__ == '__main__':
    server = TCPServer()
    server.run()
    
