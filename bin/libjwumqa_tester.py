#!/usr/bin/env python
# encoding: utf-8

from libjwumqa import *
import threading
import sys
import time
# import gc

global loop


class SendThread (threading.Thread):
    def __init__(self, name, part, node_id_s, des_id_s):
        threading.Thread.__init__(self)
        self.part = part
        self.name = name
        self.node_id_s = node_id_s
        self.des_id_s = des_id_s

    def run(self):
        counter = 3
        time.sleep(3)
        while counter > 0:
            text = self.node_id_s + '_' + str(counter)
            text_message = JTextMessage(text)
            text_message.set_destination(self.des_id_s)
            res = self.part.send(text_message)
            print('%s send text message: %s, result: %d' % (self.name, text, res))
            time.sleep(1)

            text += '_bytes'
            data_pointer = bytes(text, 'ascii')
            bytes_message = JBytesMessage(data_pointer, len(text))
            bytes_message.set_destination(self.des_id_s)
            res = self.part.send(bytes_message)
            print('%s send bytes message: %s, result: %d' % (self.name, text, res))
            time.sleep(1)

            counter -= 1

        global loop
        loop = False


class RecvThread (threading.Thread):
    def __init__(self, name, part):
        threading.Thread.__init__(self)
        self.part = part
        self.name = name

    def run(self):
        global loop
        time.sleep(3)
        while loop:
            message = self.part.receive(5000)
            if isinstance(message, JTextMessage):
                print(self.name + ' recv text message, src: ' + message.get_source() + ', des: ' + message.get_destination() + ', text: ' + message.get_text())
            elif isinstance(message, JBytesMessage):
                print(self.name + ' recv bytes message, src: ' + message.get_source() + ', des: ' + message.get_destination() + ', text: ' + str(message.get_bytes(), encoding='utf-8'))
            else:
                print(self.name + ' recv nothing!')
            time.sleep(1)


if len(sys.argv) < 4:
    print('libjwumqa_tester input error!')
    print('usage: libjwumqa_tester.py <factory url> <queue name> <node id>')
    sys.exit(0)

print('************************libjwumqa_tester begin************************')
url = sys.argv[1]
print('factory url:', sys.argv[1])
queue_name = sys.argv[2]
print('queue name:', sys.argv[2])
node_id = sys.argv[3]
print('node id:', node_id)
des_id = ""

if len(sys.argv) == 5:
    des_id = sys.argv[4]
    print('des id:', des_id)

factory = JwumqFactory(url)
print('version: %s, %s' % factory.libversion())
print('factory info: %s, %s' % factory.factory_info())

conn = factory.create_connection()
result = conn.connect()
print('FactoryConnection connect to url: %s, result: %s' % (factory.url, result))

session = conn.create_session()

if session is None:
    print('libjwumqa_tester jwumq session is None!')
    conn.close()
    factory.destroy_connection(conn)
    sys.exit(0)

destination = session.create_destination(queue_name)
if destination is None:
    print('libjwumqa_tester jwumq destination is None!')
    conn.destroy_session(session)
    conn.close()
    factory.destroy_connection(conn)
    sys.exit(0)

# pipe = destination.create_part_pipe('pipe_node_python_0')
member = destination.create_part_member(node_id)

loop = True
send_thread = SendThread("Send thread", member, node_id, des_id)
recv_thread = RecvThread("Recv thread", member)

send_thread.start()
recv_thread.start()
send_thread.join()
recv_thread.join()

# destination.destroy_part(pipe)
destination.destroy_part(member)
session.destroy_destination(destination)
conn.destroy_session(session)
conn.close()
factory.destroy_connection(conn)
# del conn
# gc.collect()

print('************************libjwumqa_tester end************************')

