# 导入Flask类
from flask import Flask
from flask import render_template
from flask import request
import time
import socket
import json
from jwumq_manager_net import LibJwumq

# 实例化，可视为固定格式
app = Flask(__name__)


# route()方法用于设定路由；类似spring路由配置
@app.route('/jwumq')
def hello_world():
    return 'Hello, Jwumq!'


# 配置路由，当请求get.html时交由get_html()处理
@app.route('/get.html')
def get_html():
    # 使用render_template()方法重定向到templates文件夹下查找get.html文件
    return render_template('get.html')


# 配置路由，当请求post.html时交由post_html()处理
@app.route('/post.html')
def post_html():
    # 使用render_template()方法重定向到templates文件夹下查找post.html文件
    return render_template('post.html')


# 配置路由，当请求deal_request时交由deal_request()处理
# 默认处理get请求，我们通过methods参数指明也处理post请求
# 当然还可以直接指定methods = ['POST']只处理post请求, 这样下面就不需要if了
@app.route('/deal_request', methods = ['GET', 'POST'])
def deal_request():
    if request.method == "GET":
        # get通过request.args.get("param_name","")形式获取参数值
        get_q = request.args.get("q","")
        return render_template("result.html", result=get_q)
    elif request.method == "POST":
        # post通过request.form["param_name"]形式获取参数值
        post_q = request.form["q"]
        return render_template("result.html", result=post_q)


@app.route('/test_method/', methods = ['GET', 'POST'])
def test_method():
    if request.method == 'GET':
        return render_template('get.html')


lib_jwumq = None


@app.route('/index.html/start_service', methods=['GET', 'POST'])
def start_service():
    data = request.get_json()
    url = data['url']
    print('Recv request to start service:', url)
    global lib_jwumq
    if lib_jwumq:
        lib_jwumq.send(4, "service start", 13)
        host_info = lib_jwumq.read_host_info(5)
        if len(host_info) > 0:
            return host_info
        return "false"
    else:
        lib_jwumq = LibJwumq('./libjwumq.so', 'jwumq_web', url)
        lib_jwumq.open()
        lib_jwumq.version()
        lib_jwumq.load()
        lib_jwumq.setup()
        time.sleep(1)
        lib_jwumq.send(4, "service start", 13)
        host_info = lib_jwumq.read_host_info(5)
        if len(host_info) > 0:
            return host_info
        else:
            lib_jwumq.release()
            lib_jwumq = None
    return "false"


@app.route('/index.html/stop_service', methods=['GET', 'POST'])
def stop_service():
    print('Recv request for stop_service.')
    global lib_jwumq
    if lib_jwumq:
        lib_jwumq.release()
        lib_jwumq = None
        return "true"

    return "true"


@app.route('/index.html/get_hostname', methods=['GET', 'POST'])
def get_hostname():
    print('Recv request to get server hostname.')
    hostname = socket.getfqdn(socket.gethostname())
    h = json.dumps({'hostname': hostname})
    print(h)
    return h


# @app.route('/index.html/get_hostip', methods=['GET', 'POST'])
# def get_hostip():
#     print('Recv request to get server host ip.')
#     eths, macs, ips = find_all_ip()
#     ip_info_list = list()
#     for eth, mac, ip in zip(eths, macs, ips):
#         ip_info_list.append({'eth': eth, 'mac': mac, 'ip': ip})
#     print(ip_info_list)
#     return json.dumps(ip_info_list)


@app.route('/index.html/request_config', methods=['GET', 'POST'])
def request_config():
    print('Recv request for request_config .')
    global lib_jwumq
    if lib_jwumq:
        lib_jwumq.send(6, "request config", 14)
        config_info = lib_jwumq.read_config_info(5)
        if len(config_info) > 0:
            return config_info
        return "false"
    else:
        return "false"


@app.route('/index.html/update_config', methods=['POST'])
def update_config():
    data = request.get_json()
    # data_str = str(data.decode())
    print('Recv request for update_config: %s.' % data)
    global lib_jwumq
    if lib_jwumq:
        lib_jwumq.send(8, json.dumps(data), len(json.dumps(data)))
        result = lib_jwumq.read_update_config_ack(5)
        return result
    else:
        return "false"


@app.route("/")
@app.route('/index.html')
def index_html():
    return render_template('index.html')


if __name__ == '__main__':
    # app.run(host, port, debug, options)
    # 默认值：host="127.0.0.1", port=5000, debug=False
    app.run(host="10.211.55.9", port=12121, debug=True)

