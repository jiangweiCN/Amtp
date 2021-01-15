# import os
from ctypes import *
import threading


# def find_all_ip():
#     ip = os.popen("ifconfig  | grep -w inet | grep -v '127.0.0.1' | awk '{print $2}'").read()
#     ip = ip.strip('\n')
#     mac = os.popen("ifconfig  | grep -w ether | awk '{print $2}'").read()
#     mac = mac.strip('\n')
#     eth = os.popen("ifconfig |grep -w flags | grep -v 'lo' | cut -d ':' -f1| awk '{print $1}'").read()
#     eth = eth.strip('\n')
#     # print(eth)
#     # print(mac)
#     # print(ip)
#     return eth.split("\n"), mac.split("\n"), ip.split("\n")


def log(func):
    def wrapper(self, *args, **kw):
        print('%s------call --------------------------%s():' % (self.mq_id, func.__name__))
        return func(self, *args, **kw)
    return wrapper


host_info_event = threading.Event()
host_info_str = ""

config_info_event = threading.Event()
config_info_str = ""

update_config_ack_event = threading.Event()
update_config_ack = ""


def recv_callback(command, des, data, length):
    data_str = str(data.decode())
    print('recv_callback recv command: %d, des: %s, length: %d]' % (command, des, length))
    print('recv_callback recv: %s' % data_str[0:length])
    if command == 5:
        global host_info_event
        global host_info_str
        host_info_event.set()
        host_info_str = data_str[0:length]
    elif command == 7:
        global config_info_event
        global config_info_str
        config_info_event.set()
        config_info_str = data_str[0:length]
    elif command == 9:
        global update_config_ack_event
        global update_config_ack
        update_config_ack_event.set()
        update_config_ack = data_str[0:length]


CMPFUNC = CFUNCTYPE(None, c_int, c_char_p, c_char_p, c_int)
callback = CMPFUNC(recv_callback)


class DealerConf(Structure):
    _fields_ = [
        ("type", c_ushort),
        ("identity", c_char * 64),
        ("address", c_char * 64),
        ("linger", c_int),
        ("read_timeout", c_int),
        ("bind", c_bool)
            ]


class LibJwumq(object):

    def __init__(self, lib_name, mq_id, addr):
        self.lib_name = lib_name
        self.mq_id = mq_id
        self.lib_jwumq = None
        self.addr = addr
        self.handle = None

    @log
    def open(self):
        print('Recv request to get server hostname.', self.lib_name)
        self.lib_jwumq = cdll.LoadLibrary(self.lib_name)

    @log
    def version(self):
        self.lib_jwumq.jwumq_version.restype = c_char_p
        print('libjwumq.so version:%s' % string_at(self.lib_jwumq.jwumq_version(), -1))

    @log
    def load(self):
        self.lib_jwumq.jwumq_dealer_load.restype = POINTER(c_void_p)
        self.handle = self.lib_jwumq.jwumq_dealer_load()
        print('libjwumq.so handle:', self.handle)

    @log
    def setup(self):
        print('monitor address :%s' % self.addr)
        jwumq_setup = self.lib_jwumq.jwumq_setup_c
        jwumq_setup.restype = c_int
        conf_s = DealerConf(2, b"jwumq_web", bytes(self.addr, "ascii"), 0, 5000, False)
        result = jwumq_setup(self.handle, byref(conf_s), callback)
        print('Setup result: %d.' % result)
        return result

    @log
    def send(self, command, data, data_len):
        print('send :%d, %s, %d' % (command, data, data_len))
        jwumq_send = self.lib_jwumq.jwumq_send_c
        jwumq_send.restype = c_int
        char_p_src = bytes(self.mq_id, "ascii")
        char_p_des = bytes("jwumq_factory_monitor", "ascii")
        char_p_data = bytes(data, "ascii")
        result = jwumq_send(self.handle, command, char_p_src, char_p_des, char_p_data, data_len)
        print('Send result: %d.' % result)
        return result

    @log
    def read_host_info(self, timeout):
        global host_info_event
        global host_info_str
        host_info_str = ""
        host_info_event.clear()
        host_info_event.wait(timeout)
        return host_info_str

    @log
    def read_config_info(self, timeout):
        global config_info_event
        global config_info_str
        config_info_str = ""
        config_info_event.clear()
        config_info_event.wait(timeout)
        print('read_config_info: %s' % config_info_str)
        return config_info_str

    @log
    def read_update_config_ack(self, timeout):
        global update_config_ack_event
        global update_config_ack
        update_config_ack = ""
        update_config_ack_event.clear()
        update_config_ack_event.wait(timeout)
        print('read_update config ack: %s' % update_config_ack)
        return update_config_ack
    # @log
    # def waitformsg(self):
    #     msg_len = c_int(0)
    #     result = self.lib_jwumq.amtpsa_waitformsg(self.handle, byref(msg_len), 5000)
    #     return result, msg_len.value
    #
    # @log
    # def readmsg(self, msg_len):
    #     recv_buf = bytes([0 for x in range(0, msg_len)])
    #     src_id_buf = bytes([0 for x in range(0, 256)])
    #     result = self.lib_jwumq.amtpsa_readmsg(self.handle, src_id_buf, recv_buf, c_int(msg_len))
    #     src_id = bytes.decode(src_id_buf)
    #     return result, src_id, recv_buf
    #

    @log
    def release(self):
        self.lib_jwumq.jwumq_release(self.handle)
