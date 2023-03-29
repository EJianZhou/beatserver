#include "mysocket.h"
#include "serverbase.h"
#include "tool/circularbuffer.h"
#include "tool/pbhelper.h"

#include <thread>




class GateServer: public ServerBase {

public:
    GateServer() : ServerBase(ServerType::gateserver) {}
    bool add_login(int id, int fd) {
        if (!umap_id2fd.count(id)) {
            umap_id2fd[id] = fd;
            umap_fd2id[fd] = id;
            return 1;
        } else {
            my_log("add login error", id, fd);
            return 0;
        }
    }
    void logout(int id, int fd) {
        my_log("logout", id, fd);
        umap_id2fd.erase(id);
        umap_fd2id.erase(fd);
        Header hd(0, id, GATELOGOUT);
        hd.write(pkg_buf);
        send_to_server(ServerType::controlserver, pkg_buf, Header::header_length);
        send_to_server(ServerType::gameserver, pkg_buf, Header::header_length);
    }
    
    bool check_login(int id, int fd) {
        return umap_id2fd.count(id) && umap_id2fd[id] == fd;
    }
    void on_server_close(ServerType type) override {
        if (type == ServerType::controlserver) {
            umap_id2fd.clear();
        } else if (type == ServerType::gameserver) {
            
        }
    }
    void on_close(int fd) override {
        my_log("gate close", fd, umap_fd2id.count(fd));
        if (umap_fd2id.count(fd)) {
            int id = umap_fd2id[fd];
            logout(id, fd);
        }
        ServerBase::on_close(fd);
    }
    void solve_pkg(int fd, Header *header) override {
        ServerBase::solve_pkg(fd, header);
        auto type = header->type;
        if (type == LOGIN) {
            header->value = fd;
            header->write(pkg_buf);
            send_to_server(ServerType::controlserver, header, pkg_buf);
        } else if (type == MATCH) {
            if (check_login(header->value, fd)) {
                send_to_server(ServerType::controlserver, header, pkg_buf);
            } else {
                my_log(header->value, fd); 
                my_log("MATCH", "not login");
            }
        } else if (type == OPERATION) {
            if (check_login(header->value, fd)) {
                send_to_server(ServerType::gameserver, header, pkg_buf);
            } else {
                my_log(header->value, fd);
                my_log("OPERATION", "not login");
            }
        } else {
            if (this->uset_clientfd.count(fd)) {
                return;
            }
            if (type == RLOGIN) {
                beatsgame::RLogin rl;
                if (rl.ParseFromArray(pkg_buf + Header::header_length, header->length)) {
                    int id = rl.id();
                    int clientfd = header->value;
                    my_log("RLogin", rl.success(), rl.id(), clientfd);
                    if (rl.success() == 1) {
                        add_login(id, clientfd);
                    }
                    Send(clientfd, pkg_buf, header->length + Header::header_length, 0);
                } else {
                    my_log("RLOGIN", "parser error");
                }
            } else {
                int id = header->value;
                if (umap_id2fd.count(id)) {
                    my_log("send", getTypeName(header->type));
                    int clientfd = umap_id2fd[id];
                    Send(clientfd, pkg_buf, header->length + Header::header_length, 0);
                }
            }
        }
    }
    void run() override {
        while (1) {
            this->epoll_step(0);
            this->try_reconnect();
        }
    }
private: 
    std::unordered_map<int, int> umap_id2fd;
    std::unordered_map<int, int> umap_fd2id;
};



int main (int argc, char *argv[])  {
    GateServer gateserver;
    gateserver.open_as_server(GATE_PORT);
    std::vector<ServerInfo> info_list = {
        ServerInfo("127.0.0.1", CONTROL_PORT, ServerType::controlserver),
        ServerInfo("127.0.0.1", GAME_PORT, ServerType::gameserver),
    };
    for (auto &info : info_list) {
        gateserver.connect_as_client(info, false);
    }
    gateserver.run();
}