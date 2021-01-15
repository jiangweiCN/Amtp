from ctypes import *
import google.protobuf
from google.protobuf import json_format
import amtpap_msg_pb2
import threading


def log(func):
    def wrapper(self, *args, **kw):
        print('%s------call --------------------------%s():' % (self.mq_id, func.__name__))
        return func(self, *args, **kw)
    return wrapper


class LibTest(object):

    def __init__(self, lib_name, mq_id, cmd_addr, data_addr):
        self.lib_name = lib_name
        self.mq_id = mq_id
        self.lib_jwumq = None
        self.cmd_addr = cmd_addr
        self.data_addr = data_addr
        self.handle = None

    @log
    def open(self):
        self.lib_jwumq = cdll.LoadLibrary(self.lib_name)

    @log
    def version(self):
        self.lib_jwumq.amtpsa_version.restype = c_char_p
        print('libamtpsa.so version:%s' % string_at(self.lib_jwumq.amtpsa_version(), -1))

    @log
    def load(self):
        self.lib_jwumq.amtpsa_load.restype = POINTER(c_void_p)
        self.handle = self.lib_jwumq.amtpsa_load()
        # print('libamtpsa.so handle:%d' % self.handle)

    @log
    def connect(self):
        print('command address :%s\ndata address :%s' % (self.cmd_addr, self.data_addr))
        return self.lib_jwumq.amtpsa_connection(self.handle, c_char_p(self.mq_id.encode("ascii")), c_char_p(self.cmd_addr.encode("ascii")), c_char_p(self.data_addr.encode("ascii")))

    @log
    def set_limit(self, limit):
        return self.lib_jwumq.amtpsa_setlimit(self.handle, limit)

    @log
    def waitformsg(self):
        msg_len = c_int(0)
        result = self.lib_jwumq.amtpsa_waitformsg(self.handle, byref(msg_len), 5000)
        return result, msg_len.value

    @log
    def readmsg(self, msg_len):
        recv_buf = bytes([0 for x in range(0, msg_len)])
        src_id_buf = bytes([0 for x in range(0, 256)])
        result = self.lib_jwumq.amtpsa_readmsg(self.handle, src_id_buf, recv_buf, c_int(msg_len))
        src_id = bytes.decode(src_id_buf)
        return result, src_id, recv_buf

    @log
    def release(self):
        self.lib_jwumq.amtpsa_release(self.handle)


def main():
    print('*****************************Jwumq tester for python begin*****************************')

    lib = LibTest('../../bin/libamtpsa.so', 'server_python_1', 'tcp://127.0.0.1:5581', 'tcp://127.0.0.1:5580')
    lib.open()
    lib.version()
    lib.load()
    print('lib connect: %d' % lib.connect())
    print('lib set limit: %d' % lib.set_limit(100))

    recv_thread = RecvThread(lib)
    recv_thread.start()
    recv_thread.join()

    lib.release()

    print('*****************************Jwumq tester for python end*****************************')


class RecvThread (threading.Thread):
    def __init__(self, lib):
        threading.Thread.__init__(self)
        self.lib = lib

    def run(self):
        print("########################## Begin recv ##########################")
        MsgRecv(self.lib)
        print("########################## End recv ##########################")


def MsgRecv(lib):
    counter = 0
    while counter < 5:
        (result, msg_len) = lib.waitformsg()
        print("wait fot msg result:%d, msg len:%d" % (result, msg_len))
        counter += 1

        if result > 0 and msg_len > 0:
            (read_result, src_id, recv_buf) = lib.readmsg(msg_len)
            print("read result:%d, src_id:%s" % (read_result, src_id))
            msg = amtpap_msg_pb2.CmdPrimitive()
            msg.ParseFromString(recv_buf)

            print(recv_buf)
            print(msg)

            if msg.msg_id == amtpap_msg_pb2.LOGIN_V1:
                login = amtpap_msg_pb2.LoginV1()
                login.ParseFromString(msg.payload_data)
                # print(json_format.MessageToDict(login, True))
                print(login)
        elif result == -106:
            print("amtpsa wait for msg time out(%d)!" % result)
        elif result == 0:
            print("amtpsa no msg(%d)!" % result)
        else:
            print("amtpsa wait for msg error(%d)!" % result)


main()
