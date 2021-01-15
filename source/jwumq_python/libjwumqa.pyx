#!/usr/bin/env python
# encoding: utf-8
import gc
import ctypes
from ctypes import *


class JMessage:
    pass


class JTextMessage(JMessage):
    def __init__(self, text = None):
        self.text = text
        self.destination = ""
        self.source = ""
        print('JTextMessage---' + text)

    def __del__(self):
        print('~JTextMessage---' + self.text)

    def set_text(self, text):
        self.text = text

    def get_text(self):
        return self.text

    def set_destination(self, destination):
        self.destination = destination

    def get_destination(self):
        return self.destination

    def get_source(self):
        return self.source

    def set_source(self, source):
        self.source = source


class JBytesMessage(JMessage):
    def __init__(self, data = None, length = 0):
        self.data = data
        self.length = length
        self.destination = ""
        self.source = ""
        print('JBytesMessage---' + str(self.length))

    def __del__(self):
        print('~JBytesMessage---' + str(self.length))

    def set_bytes(self, data, length):
        self.data = data
        self.length = length

    def get_bytes(self):
        return self.data

    def get_length(self):
        return self.length

    def set_destination(self, destination):
        self.destination = destination

    def get_destination(self):
        return self.destination

    def get_source(self):
        return self.source

    def set_source(self, source):
        self.source = source


class JwumqFactory:

    class JwumqConnection:

        class JwumqSession:

            class JwumqDestination:

                class JwumqPartPipe:

                    def __init__(self, libjwumq, session, destination, node_id):
                        self.libjwumq = libjwumq
                        self.handle = None
                        self.destination = destination
                        self.node_id = node_id
                        self.session = session
                        print('JwumqPartPipe')

                    def __del__(self):
                        print('~JwumqPartPipe')

                    def create(self):
                        self.libjwumq.jwumq_create_part_pipe_c.restype = ctypes.c_void_p
                        self.libjwumq.jwumq_create_part_pipe_c.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
                        self.handle = self.libjwumq.jwumq_create_part_pipe_c(self.destination, self.node_id.encode("utf-8"))

                    def destroy(self):
                        self.libjwumq.jwumq_destroy_part_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                        self.handle = self.libjwumq.jwumq_destroy_part_c(self.destination, self.handle)

                    def send(self, message):
                        res = 0
                        if isinstance(message, JTextMessage):
                            self.libjwumq.jwumq_create_text_message_c.restype = ctypes.c_void_p
                            self.libjwumq.jwumq_create_text_message_c.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
                            send_msg = self.libjwumq.jwumq_create_text_message_c(self.session, message.get_text().encode("utf-8"))

                            self.libjwumq.jwumq_send_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                            res = self.libjwumq.jwumq_send_c(self.handle, send_msg)

                            self.libjwumq.jwumq_destroy_message_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                            self.libjwumq.jwumq_destroy_message_c(self.session, send_msg)
                        elif isinstance(message, JBytesMessage):
                            self.libjwumq.jwumq_create_bytes_message_c.restype = ctypes.c_void_p
                            self.libjwumq.jwumq_create_bytes_message_c.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]
                            send_msg = self.libjwumq.jwumq_create_bytes_message_c(self.session, message.data, message.length)

                            self.libjwumq.jwumq_send_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                            res = self.libjwumq.jwumq_send_c(self.handle, send_msg)

                            self.libjwumq.jwumq_destroy_message_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                            self.libjwumq.jwumq_destroy_message_c(self.session, send_msg)
                        else:
                            print('unknown message type')
                        return res

                    def receive(self, timeout):
                        self.libjwumq.jwumq_receive_c.restype = ctypes.c_void_p
                        self.libjwumq.jwumq_receive_c.argtypes = [ctypes.c_void_p, ctypes.c_int, POINTER(ctypes.c_int), POINTER(ctypes.c_int)]
                        type = c_int(0)
                        length = c_int(0)
                        recv_msg = self.libjwumq.jwumq_receive_c(self.handle, timeout, pointer(type), pointer(length))
                        if recv_msg is None:
                            print('recv message is None')
                            return None
                        if type.value == 1:
                            self.libjwumq.jwumq_get_text_message_c.restype = ctypes.c_char_p
                            self.libjwumq.jwumq_get_text_message_c.argtypes = [ctypes.c_void_p]
                            text = self.libjwumq.jwumq_get_text_message_c(recv_msg)
                            self.libjwumq.jwumq_destroy_message_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                            self.libjwumq.jwumq_destroy_message_c(self.session, recv_msg)
                            return JTextMessage(text.decode("utf8"))
                        elif type.value == 2:
                            self.libjwumq.jwumq_get_bytes_message_c.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]
                            data = ctypes.create_string_buffer(length.value)
                            text = self.libjwumq.jwumq_get_bytes_message_c(recv_msg, data, length.value)
                            self.libjwumq.jwumq_destroy_message_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                            self.libjwumq.jwumq_destroy_message_c(self.session, recv_msg)
                            return JBytesMessage(data, length.value)
                        else:
                            print('unknown message type ', type)
                            self.libjwumq.jwumq_destroy_message_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                            self.libjwumq.jwumq_destroy_message_c(self.session, recv_msg)
                            return None

                class JwumqPartMember:

                    def __init__(self, libjwumq, session, destination, node_id):
                        self.libjwumq = libjwumq
                        self.handle = None
                        self.destination = destination
                        self.node_id = node_id
                        self.session = session
                        print('JwumqPartMember')

                    def __del__(self):
                        print('~JwumqPartMember')

                    def create(self):
                        self.libjwumq.jwumq_create_part_member_c.restype = ctypes.c_void_p
                        self.libjwumq.jwumq_create_part_member_c.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
                        self.handle = self.libjwumq.jwumq_create_part_member_c(self.destination, self.node_id.encode("utf-8"))

                    def destroy(self):
                        self.libjwumq.jwumq_destroy_part_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                        self.handle = self.libjwumq.jwumq_destroy_part_c(self.destination, self.handle)

                    def send(self, message):
                        res = 0
                        if isinstance(message, JTextMessage):
                            self.libjwumq.jwumq_create_text_message_c.restype = ctypes.c_void_p
                            self.libjwumq.jwumq_create_text_message_c.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
                            send_msg = self.libjwumq.jwumq_create_text_message_c(self.session, message.get_text().encode("utf-8"))

                            self.libjwumq.jwumq_message_set_destination_c.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
                            self.libjwumq.jwumq_message_set_destination_c(send_msg, message.get_destination().encode("utf-8"))

                            self.libjwumq.jwumq_send_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                            res = self.libjwumq.jwumq_send_c(self.handle, send_msg)

                            self.libjwumq.jwumq_destroy_message_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                            self.libjwumq.jwumq_destroy_message_c(self.session, send_msg)
                        elif isinstance(message, JBytesMessage):
                            self.libjwumq.jwumq_create_bytes_message_c.restype = ctypes.c_void_p
                            self.libjwumq.jwumq_create_bytes_message_c.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]
                            send_msg = self.libjwumq.jwumq_create_bytes_message_c(self.session, message.data, message.length)

                            self.libjwumq.jwumq_message_set_destination_c.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
                            self.libjwumq.jwumq_message_set_destination_c(send_msg, message.get_destination().encode("utf-8"))

                            self.libjwumq.jwumq_send_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                            res = self.libjwumq.jwumq_send_c(self.handle, send_msg)

                            self.libjwumq.jwumq_destroy_message_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                            self.libjwumq.jwumq_destroy_message_c(self.session, send_msg)
                        else:
                            print('unknown message type')
                        return res

                    def receive(self, timeout):
                        self.libjwumq.jwumq_receive_c.restype = ctypes.c_void_p
                        self.libjwumq.jwumq_receive_c.argtypes = [ctypes.c_void_p, ctypes.c_int, POINTER(ctypes.c_int), POINTER(ctypes.c_int)]
                        type = c_int(0)
                        length = c_int(0)
                        recv_msg = self.libjwumq.jwumq_receive_c(self.handle, timeout, pointer(type), pointer(length))
                        if recv_msg is None:
                            print('recv message is None')
                            return None
                        if type.value == 1:
                            self.libjwumq.jwumq_get_text_message_c.restype = ctypes.c_char_p
                            self.libjwumq.jwumq_get_text_message_c.argtypes = [ctypes.c_void_p]
                            text = self.libjwumq.jwumq_get_text_message_c(recv_msg)

                            self.libjwumq.jwumq_get_message_destination_c.restype = ctypes.c_char_p
                            self.libjwumq.jwumq_get_message_destination_c.argtypes = [ctypes.c_void_p]
                            destination = self.libjwumq.jwumq_get_message_destination_c(recv_msg)

                            self.libjwumq.jwumq_get_message_source_c.restype = ctypes.c_char_p
                            self.libjwumq.jwumq_get_message_source_c.argtypes = [ctypes.c_void_p]
                            source = self.libjwumq.jwumq_get_message_source_c(recv_msg)

                            self.libjwumq.jwumq_destroy_message_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                            self.libjwumq.jwumq_destroy_message_c(self.session, recv_msg)
                            msg = JTextMessage(text.decode("utf8"))
                            msg.set_destination(destination.decode("utf8"))
                            msg.set_source(source.decode("utf8"))
                            return msg
                        elif type.value == 2:
                            self.libjwumq.jwumq_get_bytes_message_c.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]
                            data = ctypes.create_string_buffer(length.value)
                            res = self.libjwumq.jwumq_get_bytes_message_c(recv_msg, data, length.value)

                            self.libjwumq.jwumq_get_message_destination_c.restype = ctypes.c_char_p
                            self.libjwumq.jwumq_get_message_destination_c.argtypes = [ctypes.c_void_p]
                            destination = self.libjwumq.jwumq_get_message_destination_c(recv_msg)

                            self.libjwumq.jwumq_get_message_source_c.restype = ctypes.c_char_p
                            self.libjwumq.jwumq_get_message_source_c.argtypes = [ctypes.c_void_p]
                            source = self.libjwumq.jwumq_get_message_source_c(recv_msg)

                            self.libjwumq.jwumq_destroy_message_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                            self.libjwumq.jwumq_destroy_message_c(self.session, recv_msg)
                            msg = JBytesMessage(data, length.value)
                            msg.set_destination(destination.decode("utf8"))
                            msg.set_source(source.decode("utf8"))
                            return msg
                        else:
                            print('unknown message type ', type)
                            self.libjwumq.jwumq_destroy_message_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                            self.libjwumq.jwumq_destroy_message_c(self.session, recv_msg)
                            return None

                def __init__(self, libjwumq, session, queue_name):
                    self.libjwumq = libjwumq
                    self.handle = None
                    self.session = session
                    self.queue_name = queue_name
                    print('JwumqDestination')

                def __del__(self):
                    print('~JwumqDestination')

                def create(self):
                    self.libjwumq.jwumq_create_destination_c.restype = ctypes.c_void_p
                    self.libjwumq.jwumq_create_destination_c.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
                    self.handle = self.libjwumq.jwumq_create_destination_c(self.session, self.queue_name.encode("utf-8"))

                def destroy(self):
                    self.libjwumq.jwumq_destroy_destination_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                    self.handle = self.libjwumq.jwumq_destroy_destination_c(self.session, self.handle)

                def create_part_pipe(self, node_id):
                    part_pipe = self.JwumqPartPipe(self.libjwumq, self.session, self.handle, node_id)
                    part_pipe.create()
                    if part_pipe.handle is None:
                        return None
                    return part_pipe

                def create_part_member(self, node_id):
                    part_member = self.JwumqPartMember(self.libjwumq, self.session, self.handle, node_id)
                    part_member.create()
                    if part_member.handle is None:
                        return None
                    return part_member

                def destroy_part(self, part):
                    part.destroy()

            def __init__(self, libjwumq, conn):
                self.libjwumq = libjwumq
                self.handle = None
                self.conn = conn
                print('JwumqSession')

            def __del__(self):
                print('~JwumqSession')

            def create(self):
                self.libjwumq.jwumq_create_session_c.restype = ctypes.c_void_p
                self.libjwumq.jwumq_create_session_c.argtypes = [ctypes.c_void_p]
                self.handle = self.libjwumq.jwumq_create_session_c(self.conn)

            def destroy(self):
                self.libjwumq.jwumq_destroy_session_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                self.handle = self.libjwumq.jwumq_destroy_session_c(self.conn, self.handle)

            def create_destination(self, queue_name):
                destination = self.JwumqDestination(self.libjwumq, self.handle, queue_name)
                destination.create()
                if destination.handle is None:
                    return None
                return destination

            def destroy_destination(self, destination):
                destination.destroy()

            def create_text_message(self, text):
                self.libjwumq.jwumq_create_text_message_c.restype = ctypes.c_void_p
                self.libjwumq.jwumq_create_text_message_c.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
                message = self.libjwumq.jwumq_create_text_message_c(self.handle, text.encode("utf-8"))
                if message is None:
                    return None
                return message

            def destroy_message(self, message):
                self.libjwumq.jwumq_destroy_message_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
                self.libjwumq.jwumq_destroy_message_c(self.handle, message)

        def __init__(self, libjwumq, factory):
            self.libjwumq = libjwumq
            self.factory = factory
            self.handle = None
            print('JwumqConnection')

        def __del__(self):
            print('~JwumqConnection')

        def create(self):
            self.libjwumq.jwumq_create_connection_c.restype = ctypes.c_void_p
            self.libjwumq.jwumq_create_connection_c.argtypes = [ctypes.c_void_p]
            self.handle = self.libjwumq.jwumq_create_connection_c(self.factory)

        def destroy(self):
            self.libjwumq.jwumq_destroy_connection_c.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
            self.handle = self.libjwumq.jwumq_destroy_connection_c(self.factory, self.handle)

        def create_session(self):
            session = self.JwumqSession(self.libjwumq, self.handle)
            session.create()
            if session.handle is None:
                return None
            return session

        def destroy_session(self, session):
            session.destroy()

        def connect(self):
            self.libjwumq.jwumq_connection_c.argtypes = [ctypes.c_void_p]
            return self.libjwumq.jwumq_connection_c(self.handle)

        def close(self):
            self.libjwumq.jwumq_close_c.argtypes = [ctypes.c_void_p]
            self.libjwumq.jwumq_close_c(self.handle)

    def __init__(self, url):
        self.url = url
        self.version = 'jwumq_py 2.0.0.20200702'
        self.libjwumq = cdll.LoadLibrary("./libjwumqa.so")
        self.libjwumq.jwumq_create_factory_c.restype = ctypes.c_void_p
        self.handle = self.libjwumq.jwumq_create_factory_c(bytes(self.url, "utf8"))
        print('JwumqFactory ' + str(self.handle))

    def __del__(self):
        self.libjwumq.jwumq_destroy_factory_c.argtypes = [ctypes.c_void_p]
        self.libjwumq.jwumq_destroy_factory_c(self.handle)
        print('~JwumqFactory')

    def libversion(self):
        self.libjwumq.jwumq_version_c.restype = c_char_p
        return self.version, self.libjwumq.jwumq_version_c().decode("utf8")

    def factory_info(self):
        return self.version, self.url

    def create_connection(self):
        conn = self.JwumqConnection(self.libjwumq, self.handle)
        conn.create()
        return conn

    def destroy_connection(self, conn):
        # if conn:
        #     del conn
        #     gc.collect()
        #     print('destroy_connection')
        conn.destroy()
        pass


