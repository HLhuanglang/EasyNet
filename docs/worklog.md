# EasyNet开发日志

## 2022年9月17日
今天从较高的层面来看怎么设计api，站在一个库的使用者角度。初步的样子：

服务端：
``` cpp
#include "../../net/easy_net.h"

#include <iostream>

int main() {
    event_loop loop;
    tcp_server server(&loop, "127.0.0.1", 8888);

    // 1,设置连接建立回调
    server.set_build_connection_cb([]() {
        // 1,1打印日志
        std::cout << "connection build!" << std::endl;
        // 1,2记录对端信息
    });

    // 2,设置断开连接回调
    server.set_close_connection_cb([]() {
        // 2,1打印日志
        std::cout << "connection close!" << std::endl;
        // 2,2记录对端信息
    });

    // 3,设置消息处理回调
    server.set_recv_msg_cb([&](tcp_connection* conn, buffer* buf) {
        // 3,1 处理数据：todo handle buf
        // 2.2 回复客户端
        std::string ret = "hello,client!";
        conn->send_data(ret.c_str(), ret.size());
    });

    // 4,启动
    server.set_thread_cnt(4);
    server.start();
}
```
客户端：
``` cpp
#include "../../net/easy_net.h"

#include <iostream>

int main() {
    event_loop loop;
    tcp_client client(&loop, "127.0.0.1", 8888);

    // 1,设置连接建立回调
    client.set_build_connection_cb([](tcp_client* cli, void* args) {
        //客户端主动发送数据的地方!
        std::string resp = "hello,server!";
        cli->send_data(resp.c_str(), resp.size());
    });

    // 2,设置连接断开回调
    client.set_close_connection_cb([]() {
        // 2,1打印日志
        std::cout << "connection close!" << std::endl;
        // 2,2记录对端信息
    });

    // 3,设置消息处理回调
    client.set_recv_msg_cb([](tcp_connection* conn, buffer* buf) {
        // 3.1 处理数据：todo handle buf
    });

    // 4,启动
    client.start();
}

/*
    上面这种接口模式,如果client想在任意逻辑中发送消息,那怎么实现？
    不可能每次想发送消息，都得重新创建一个tcp_client，然后设置一大堆操作回调，这样会显得很呆
    而且代码也不好实现.
*/
```

## 2022年9月18日
关于buffer设计这一块的思考。目前的设想是采用内存池分配预设大小的内存块，假设当前申请了一块64kb的内存块，如果还有数据需要读取的话，就得进行扩容了。扩容的过程就是从内存池中换一块更大的内存块，从64变成128kb，然后需要经历一次拷贝操作。如果频繁的出现拷贝，这个拷贝开销不能忽略...

重新阅读了一下muduo网络库，以及陈硕写的书，里面的很多问题和解决思路值得深究，最初阅读的时候，因为并没有动手实践写过socket，所以很多问题的场景和解决手段的理解压根就不明白。准备先好好阅读一下再来继续写代码。

## 2022年9月19日
尝试使用libuv、libevent写点echo、chat、proxy的demo，感受一下api上的设计。

## 2022年9月23日
创建tcp_server后注册的一堆回调，怎么和sub_reactor联系起来呢？

每一个tcp连接都是一个tcp_connection对象，真正的数据收发都在tcp_connection中进行操作。此时如果多个连接都有数据到达，那么recv_msg_cb应该返回哪一个连接的数据？

假设目前是使用http协议来进行通信，客户端主动send_data，客户端自己只要管好自己一个socket就行，发送的数据是一个完整的http包。

此时服务端的某个acceptfd收到了数据，将数据全部读取到read_buf中后触发recv_msg_cb，使用http服务肯定是不会让用户注册这些回调的，要像下面一样：
```cpp

http_server server;

server.on_msg("/hello", [](http_request* req, http_response* res){
    if(req->status_code == 200){
        //todo
    }else{
        res->set_body("json数据");
    }
});

```

完事之后，res肯定会根据acceptfd来找到对应的tcp_connection，然后再调用send_data把数据发给客户端。


## 2022年12月10日

## 2023年1月3日

tcp_client设计

