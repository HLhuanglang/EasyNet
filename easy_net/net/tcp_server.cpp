#include "tcp_server.h"
#include "acceptor.h"
#include "def.h"
#include "event_loop.h"
#include "inet_addr.h"
#include "non_copyable.h"
#include "server_thread.h"
#include "socket_opt.h"
#include "spdlog/spdlog.h"
#include "tcp_connection.h"
#include <memory>
#include <string>
#include <thread>
#include <utility>

using namespace EasyNet;

CallBack TcpServer::m_new_connection_cb;
CallBack TcpServer::m_del_connection_cb;
CallBack TcpServer::m_revc_msg_cb;
CallBack TcpServer::m_write_complete_cb;

TcpServer::TcpServer(unsigned int numEventThreads,
                     const InetAddress &listenAddr,
                     const std::string &nameArg,
                     bool isReusePort, EventLoop *pMainLoop)
    : m_loop(pMainLoop),
      m_addr(listenAddr),
      m_name(nameArg),
      m_thread_cnt(numEventThreads) {
    if (pMainLoop != NULL) {
        m_acceptor = make_unique<Acceptor>(this, listenAddr, isReusePort);
    }
}

TcpServer::~TcpServer() {
    for (auto &conn : m_connections_map) {
        conn.second->RemoveEvent();
    }
    m_connections_map.clear();
}

void TcpServer::start() {
    // 1,开启子线程(如果是子线程,这里直接跳过)
    startThreadPool();

    // 2,开启主线程
    if (m_acceptor) {
        m_acceptor->StartListen();
    }
}

void TcpServer::join_thread() {
    for (auto &n : m_child_svr_vec) {
        n->Join();
    }
}

void TcpServer::detach_thread() {
    for (auto &n : m_child_svr_vec) {
        n->Detach();
    }
}

void TcpServer::NewConn(int fd, const InetAddress &peerAddr) {
    auto tcp_conn = std::make_shared<TcpConn>(this, fd, peerAddr);
    m_connections_map[tcp_conn->GetConnName()] = tcp_conn;
    tcp_conn->EnableRead();
    if (m_new_connection_cb != nullptr) {
        m_new_connection_cb(tcp_conn);
    }
}

void TcpServer::DelConn(const tcp_connection_t &conn) {
    if (m_del_connection_cb != nullptr) {
        m_del_connection_cb(conn);
    }
    conn->RemoveEvent();
    m_connections_map.erase(conn->GetConnName());
}

void TcpServer::RecvMsg(const tcp_connection_t &conn) {
    if (m_revc_msg_cb != nullptr) {
        m_revc_msg_cb(conn);
    }
}

void TcpServer::WriteComplete(const tcp_connection_t &conn) {
    if (m_write_complete_cb != nullptr) {
        m_write_complete_cb(conn);
    }
}

EventLoop *TcpServer::GetEventLoop() const {
    return m_loop;
}

void TcpServer::startThreadPool() {
    for (int i = 0; i < m_thread_cnt; i++) {
        std::unique_ptr<ServerThread> svr_thread = make_unique<ServerThread>("child_svr_" + std::to_string(i), this->m_addr);
        auto tmp_loop = svr_thread->StartServerThread();
        m_child_svr_vec.push_back(std::move(svr_thread));
        m_child_loop_vec.push_back(tmp_loop);
    }
}