
#pragma once
#include "config.h"
#include "mysocket.h"
#include "proto/server.pb.h"
#include "proto/project.pb.h"
#include "tool/epoll_manager.h"
#include "tool/circularbuffer.h"
#include "tool/pbhelper.h"


#include <map>
#include <list>
#include <vector>
#include <chrono>
#include <unordered_set>
#include <unordered_map>
#include <iostream>




enum ServerType{
    gateserver,
    gameserver,
    controlserver,
    dbserver,
    none
};

const std::vector<std::string> ServerTypeNames = {
    "gateserver",
    "gameserver",
    "controlserver",
    "dbserver",
    "none"
};

ServerType get_server_typebyname(const std::string &servername);


std::string get_server_namebytype(ServerType type);

class Header { 

public:

    static const size_t header_length = 9;
    Header(size_t length=0, int value=0, uint8_t type=0) : length(length), value(value), type(type) {}
    void read(const char* buf);
    void write(char* buf);
    void init();
    int length;
    int value;
    uint8_t type;

};

template<typename PB>
bool SerializeWithHeader(Header* header, const PB &pb, char *pkg_buf, int len) {
    int tot = Header::header_length + pb.ByteSizeLong();
    if (tot > len) 
        return 0;
    header->write(pkg_buf);
    return pb.SerializeToArray(pkg_buf + Header::header_length, pb.ByteSizeLong());
}


struct ServerInfo {
  std::string ip;
  int port;
  ServerType type;
  ServerInfo() {}
  ServerInfo(std::string ip, int port, ServerType type): ip(ip), port(port), type(type) {}  
};


class NetworkManager {

public:
    NetworkManager() {
        serverfd.resize(ServerTypeNames.size(), -1);
    }
    ~NetworkManager() {}
    int open_as_server(int port);
    int connect_as_client(const ServerInfo& serverinfo);
    int accpect_client(int listenfd);
    void set_fdtype(int fd, const ServerType& type);
    void close_fd(int fd);
    int request_serverfd(const ServerType& type);
    ServerType request_fdtype(int fd);

private:
    std::vector<int> serverfd;
    std::unordered_map<int, ServerType> fd_type;
};

class ServerBase {
public:
    ServerBase(ServerType type=ServerType::none) : selftype(type) {
        net_mgr = new NetworkManager();
        epoll_mgr = new epoll_manager();
    } 
    ~ServerBase() {
        delete net_mgr;
        delete epoll_mgr;
    }
    void on_accept();
    void on_message(int fd, const char *buf, int len);
    virtual void on_close(int fd);
    virtual void solve_pkg(int fd, Header* header);  
    void try_solve(int fd, CircularBuffer* cbuf, Header* header);
    void send_to_server(ServerType type, const char* buf, int len);
    void send_to_server(ServerType type, const Header *header, char *buf);
    bool connect_as_client(ServerInfo& info, bool reconnect);
    void open_as_server(int port);
    void try_reconnect();
    void epoll_step(int timeout);
    virtual void run();



protected:
    char pkg_buf[PKGSIZE];
    int listenfd;
    ServerType selftype;
    NetworkManager *net_mgr;
    epoll_manager *epoll_mgr;
    std::map<int, std::pair<CircularBuffer*, Header*>> map_fd_cbufheader;
    std::map<ServerType, ServerInfo> map_type_info;
    std::list<ServerType> list_reconnect;
    std::unordered_set<int> uset_clientfd;
};

